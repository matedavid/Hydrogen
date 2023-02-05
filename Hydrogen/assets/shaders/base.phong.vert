#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTextureCoords;
layout(location = 3) in vec3 aTangent;

layout(std140) uniform Camera {
    uniform mat4 Projection;
    uniform mat4 View;
    uniform vec3 CameraPosition;
};

uniform mat4 Model;

out vec3 FragPosition;
out vec3 FragNormal;
out vec2 FragTextureCoords;
out vec3 FragCameraPosition;
out mat3 FragTBN;

void main() {
    mat4 ViewProjection = Projection * View;
    gl_Position = ViewProjection * Model * vec4(aPosition, 1.0f);

    FragPosition = vec3(Model * vec4(aPosition, 1.0f));
    FragNormal = mat3(transpose(inverse(Model))) * aNormal;
    FragTextureCoords = aTextureCoords;
    FragCameraPosition = CameraPosition;

    vec3 T = normalize(vec3(Model * vec4(aTangent, 0.0f)));
    vec3 N = normalize(vec3(Model * vec4(aNormal, 0.0f)));
    vec3 B = cross(N, T);

    FragTBN = mat3(T, B, N);
}