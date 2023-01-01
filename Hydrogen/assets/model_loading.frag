#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPos;

struct PointLightStruct {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
#define NR_POINT_LIGHTS 4
uniform PointLightStruct PointLights[NR_POINT_LIGHTS];

struct DirLightStruct {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform DirLightStruct DirLight;

uniform vec3 CameraPosition;

struct MaterialStruct {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform MaterialStruct Material;

vec3 CalcDirLight(DirLightStruct light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLightStruct light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(CameraPosition - FragPos);

    // Directional light
    vec3 result = CalcDirLight(DirLight, norm, viewDir);
    // Point lights
    for (int i = 0; i < 2; i++) {
        result += CalcPointLight(PointLights[i], norm, FragPos, viewDir);
    }

    FragColor = vec4(result, 1.0f);


/*
// vec4 color = texture(Texture, TexCoords);
vec3 diffuseColor = vec3(texture(Material.diffuse, TexCoords));
vec3 specularColor = vec3(texture(Material.specular, TexCoords));

// Ambient
vec3 ambient = Light.ambient * diffuseColor;

// Diffuse
vec3 norm = normalize(Normal);
vec3 lightDirection = normalize(Light.position - FragPos);
float diff = max(dot(norm, lightDirection), 0.0);
vec3 diffuse = Light.diffuse * diff * diffuseColor;

// Specular
vec3 viewDirection = normalize(CameraPosition - FragPos);
vec3 reflectDirection = reflect(-lightDirection, norm);
float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), Material.shininess);
vec3 specular = Light.specular * spec * specularColor;

FragColor = vec4(ambient + diffuse + specular, 1.0);
//FragColor = vec4(ambient + diffuse + specular, 1.0) * color;
// FragColor = vec4(0.8, 0.8, 0.8, 1.0f);
*/
}

vec3 CalcDirLight(DirLightStruct light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(- light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(Material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(Material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(Material.specular, TexCoords));
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLightStruct light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), Material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance +
    light.quadratic * (distance * distance));
    // combine results
    vec3 ambient = light.ambient * vec3(texture(Material.diffuse, TexCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(Material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(Material.specular, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}