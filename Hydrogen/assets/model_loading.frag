#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPos;

struct LightStruct {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform LightStruct Light;
uniform vec3 CameraPosition;

struct MaterialStruct {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform MaterialStruct Material;
uniform sampler2D Texture;

void main()
{
    vec4 color = texture(Texture, TexCoords);

    // Ambient
    // float ambientStrength = 0.1;
    vec3 ambient = Light.ambient * Material.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(Light.position - FragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = Light.diffuse * (diff * Material.diffuse);

    // Specular
    vec3 viewDirection = normalize(CameraPosition - FragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), Material.shininess);
    vec3 specular = Light.specular * (spec * Material.specular);

    FragColor = vec4(ambient + diffuse + specular, 1.0) * color;
    // FragColor = vec4(0.8, 0.8, 0.8, 1.0f);
}
