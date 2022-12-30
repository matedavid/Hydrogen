#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec2 TexCoords;
in vec3 FragPos;

uniform vec3 LightPosition;
uniform vec3 LightColor;
uniform vec3 CameraPosition;

uniform sampler2D Texture;

void main()
{
    vec4 color = texture(Texture, TexCoords);

    // Ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * LightColor;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDirection = normalize(LightPosition - FragPos);
    float diff = max(dot(norm, lightDirection), 0.0);
    vec3 diffuse = diff * LightColor;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDirection = normalize(CameraPosition - FragPos);
    vec3 reflectDirection = reflect(-lightDirection, norm);
    float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), 32);
    vec3 specular = specularStrength * spec * LightColor;

    FragColor = vec4(ambient + diffuse + specular, 1.0) * color;
    // FragColor = vec4(0.8, 0.8, 0.8, 1.0f);
}
