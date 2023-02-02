// Info from Vertex Shader
in vec3 FragPosition;
in vec3 FragNormal;
in vec2 FragTextureCoords;
in vec3 FragCameraPosition;
in mat3 FragTBN;

// Material definition and uniform
struct MaterialStruct {
    vec3 ambient;

#ifdef diffuse_color
    vec3 diffuse;
#endif

#ifdef specular_color
    vec3 specular;
#endif

#ifdef shininess_def
    float shininess;
#endif

#ifdef diffuse_texture
    sampler2D diffuse_map;
#endif

#ifdef specular_texture
    sampler2D specular_map;
#endif

#ifdef normal_texture
    sampler2D normal_map;
#endif
};
uniform MaterialStruct Material;

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

// Function definitions
vec3 CalcPointLight(PointLightStruct light, vec3 normal, vec3 fragPos, vec3 viewDirection);

void main() {
#if defined(normal_texture)
    vec3 normal = texture(Material.normal_map, FragTextureCoords).rgb;
    normal = normal * 2.0f - 1.0f; // convert from [0,1] to [-1,1]
    normal = normalize(FragTBN * normal);
#else
    vec3 normal = normalize(FragNormal);
#endif

    vec3 viewDirection = normalize(FragCameraPosition - FragPosition);

    vec3 result = vec3(0.0f, 0.0f, 0.0f);
    for (int i = 0; i < NumberPointLights; ++i) {
        result += CalcPointLight(PointLights[i], normal, FragPosition, viewDirection);
    }

    ResultColor = vec4(result, 1.0f);
}

vec3 CalcPointLight(PointLightStruct light, vec3 normal, vec3 fragPos, vec3 viewDirection) {
    vec3 lightDirection = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDirection), 0.0);

    float spec = 1.0f;
#if defined(shininess_def)
    vec3 halfwayDirection = normalize(lightDirection + viewDirection);
    spec = pow(max(dot(viewDirection, halfwayDirection), 0.0), Material.shininess);
#endif

    // attenuation
    float lightDistance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * lightDistance +
                                light.quadratic * (lightDistance * lightDistance));

    // combine results
    vec3 ambient = vec3(0.0f, 0.0f, 0.0f);
    vec3 diffuse = vec3(0.0f, 0.0f, 0.0f);
    vec3 specular = vec3(0.0f, 0.0f, 0.0f);

#if defined(diffuse_texture)
    ambient = light.ambient * vec3(texture(Material.diffuse_map, FragTextureCoords));
    diffuse = light.diffuse * diff * vec3(texture(Material.diffuse_map, FragTextureCoords));
#elif defined(diffuse_color)
    ambient = light.ambient * Material.diffuse;
    diffuse = light.diffuse * diff * Material.diffuse;
#endif

#if defined(specular_texture)
    specular = light.specular * spec * vec3(texture(Material.specular_map, FragTextureCoords));
#elif defined(specular_color)
    specular = light.specular * spec * Material.specular;
#endif

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}