// Info from Vertex Shader
in vec3 FragPosition;
in vec3 FragNormal;
in vec2 FragTextureCoords;
in vec3 FragCameraPosition;
in mat3 FragTBN;

// PBR Material definition
struct PBRMaterial {
#ifdef albedo_color
    vec3 albedo;
#endif

#ifdef metallic_value
    float metallic;
#endif

#ifdef roughness_value
    float roughness;
#endif

#ifdef ao_value
    float ao;
#endif

#ifdef albedo_texture
    sampler2D albedo_map;
#endif

#ifdef metallic_texture
    sampler2D metallic_map;
#endif

#ifdef roughness_texture
    sampler2D roughness_map;
#endif

#ifdef ao_texture
    sampler2D ao_map;
#endif

#ifdef normal_texture
    sampler2D normal_map;
#endif
};
uniform PBRMaterial Material;

// PointLight definition and values
struct PointLightStruct {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define MAX_NUMBER_POINT_LIGHTS 20
uniform int NumberPointLights;
uniform PointLightStruct[MAX_NUMBER_POINT_LIGHTS] PointLights;

// Fragment Output
out vec4 ResultColor;

// Forward declare functions
vec3 CalcPointLight(
    PointLightStruct light, 
    vec3 albedo, float metallic, float roughness, float ao,
    vec3 N, vec3 V, vec3 F0);

vec3 FresnelSchlick(float cosTheta, vec3 F0);
float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);

// Constants
const float PI = 3.14159265359;

void main() {
#if defined(normal_texture)
    vec3 N = texture(Material.normal_map, FragTextureCoords).rgb;
    N = N * 2.0f - 1.0f; // convert from [0,1] to [-1,1]
    N = normalize(FragTBN * N);
#else
    vec3 N = normalize(FragNormal);
#endif

    vec3 V = normalize(FragCameraPosition - FragPosition);

    vec3 albedo;
    float metallic = 0.0f;
    float roughness = 0.0f;
    float ao = 0.0f;

#if defined(albedo_texture)
    albedo = pow(texture(Material.albedo_map, FragTextureCoords).rgb, vec3(2.2));
#elif defined(albedo_color)
    albedo = Material.albedo;
#else
#error Albedo color or texture is required
#endif

#if defined(metallic_roughness_ao_texture)
    ao = texture(Material.ao_map, FragTextureCoords).r;
    roughness = texture(Material.roughness_map, FragTextureCoords).g;
    metallic = texture(Material.metallic_map, FragTextureCoords).b;
#endif

#if defined(metallic_roughness_texture) && !defined(metallic_roughness_ao_texture)
    roughness = texture(Material.roughness_map, FragTextureCoords).r;
    metallic = texture(Material.metallic_map, FragTextureCoords).g;
#endif

#if !defined(metallic_roughness_texture) && !defined(metallic_roughness_ao_texture)
    #if defined(metallic_texture)
        metallic = texture(Material.metallic_map, FragTextureCoords).r;
    #elif defined(metallic_value)
        metallic = Material.metallic;
    #endif

    #if defined(roughness_texture)
        roughness = texture(Material.roughness_map, FragTextureCoords).r;
    #elif defined(roughness_value)
        roughness = Material.roughness;
    #endif
#endif

#if defined(ao_texture)
    ao = texture(Material.ao_map, FragTextureCoords).r;
#elif defined(ao_value)
    ao = Material.ao;
#endif

    // Reflectance at normal incidence
    vec3 F0 = vec3(0.04f);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0f);
    for (int i = 0; i < NumberPointLights; ++i) {
        Lo += CalcPointLight(PointLights[i], albedo, metallic, roughness, ao, N, V, F0);
    }

    vec3 ambient = vec3(0.03f) * albedo * ao;
    vec3 color = ambient + Lo;

    // HDR tonemapping & gamma correct
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));  

    ResultColor = vec4(color, 1.0);
}

vec3 CalcPointLight(
    PointLightStruct light, 
    vec3 albedo, float metallic, float roughness, float ao,
    vec3 N, vec3 V, vec3 F0) 
{
    vec3 L = normalize(light.position - FragPosition);
    vec3 H = normalize(V + L);

    float distance = length(light.position - FragPosition);
    float attenuation = 1.0f / (distance * distance);
    vec3 radiance = light.diffuse * attenuation;

    vec3 F = FresnelSchlick(max(dot(H, V), 0.0f), F0);

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) +
                        0.0001f; // + 0.0001f to prevent divide by 0
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;

    kD *= 1.0f - metallic;

    float NdotL = max(dot(N, L), 0.0);
    return (kD * albedo / PI + specular) * radiance * NdotL;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
