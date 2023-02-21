#version 330 core

layout(location = 0) in vec3 aPosition;

layout(std140) uniform Camera {
    uniform mat4 Projection;
    uniform mat4 View;
    uniform vec3 CameraPosition;
};

uniform mat4 Model;

out vec3 FragTextureCoords;

void main() {
    FragTextureCoords = aPosition;

    mat4 view = mat4(mat3(View));
    vec4 pos = Projection * view * Model * vec4(aPosition, 1.0f);
    gl_Position = pos.xyww;
}