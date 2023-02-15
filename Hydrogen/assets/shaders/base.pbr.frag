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

// Skybox
uniform samplerCube Skybox;

// Fragment Output
out vec4 ResultColor;

// Constants
const float PI = 3.14159265359;

vec3 FresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    // Square roughness based on observations from Disney and Epic Games
    float alpha = roughness * roughness;
    float alpha2 = alpha * alpha;

    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
	
    float denominator = (NdotH2 * (alpha2 - 1.0) + 1.0);
    denominator = PI * denominator * denominator;
	
    return alpha2 / denominator;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    // Using K from direct lighting
    float k = (r * r) / 8.0;

    float denominator = NdotV * (1.0 - k) + k;
    return NdotV / denominator;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);

    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);

    return ggx1 * ggx2;
}

void main() { 
    // ============================
    // Define Material properties
    // ============================
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;

#if defined(albedo_texture)
    albedo = texture(Material.albedo_map, FragTextureCoords).rgb;
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

#if !defined(metallic_roughness_ao_texture)
    #if defined(ao_texture)
        ao = texture(Material.ao_map, FragTextureCoords).r;
    #elif defined(ao_value)
        ao = Material.ao;
    #endif
#endif

#if defined(normal_texture)
    vec3 N = texture(Material.normal_map, FragTextureCoords).rgb;
    N = N * 2.0 - 1.0; // convert from [0,1] to [-1,1]
    N = normalize(FragTBN * N);
#else
    vec3 N = normalize(FragNormal);
#endif
    // ============================

    vec3 V = normalize(FragCameraPosition - FragPosition);

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < NumberPointLights; ++i) {
        PointLightStruct light = PointLights[i];
        
        vec3 L = normalize(light.position - FragPosition);
        vec3 H = normalize(V + L);

        float dist = length(light.position - FragPosition);
        float attenuation = 1.0 / (dist * dist);
        vec3 radiance = light.diffuse * attenuation;

        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = FresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

        // Cook-Torrence BRDF
        vec3 numerator = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        // Because energy conservation, the diffuse and specular light can't be above 1.0
        vec3 kD = vec3(1.0) - kS;
        // Enforce that metallic sufraces dont refract light
        kD *= 1.0 - metallic;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
        // we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // Gamma correction 
    color = pow(color, vec3(1.0 / 2.2)); 

    ResultColor = vec4(color, 1.0);
}
