#version 330 core

in vec3 FragTextureCoords;

uniform samplerCube Skybox;

out vec4 ResultColor;

void main() {
    ResultColor = texture(Skybox, FragTextureCoords);
}