#version 330 core

in vec3 FragTextureCoords;

uniform samplerCube Skybox;
uniform int PostProcessing = 0;

out vec4 ResultColor;

void main() {
    vec3 color = texture(Skybox, FragTextureCoords).rgb;

    if (PostProcessing == 1) {
        // HDR Tonemapping
        color = color / (color + vec3(1.0));
        // Gamma Correction
        color = pow(color, vec3(1.0 / 2.2));
    }

    ResultColor = vec4(color, 1.0);
}