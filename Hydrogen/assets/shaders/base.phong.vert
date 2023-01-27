#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoords;

layout(std140) uniform Camera {
    uniform mat4 ViewProjection;
};

uniform mat4 Model;

out vec3 FragPosition;
out vec3 FragNormal;
out vec2 FragTextureCoords;

void main() {
    gl_Position = ViewProjection * Model * vec4(aPosition, 1.0f);

    FragPosition = aPosition;
    FragNormal = mat3(transpose(inverse(Model))) * aNormal;
    FragTextureCoords = aTextureCoords;
}