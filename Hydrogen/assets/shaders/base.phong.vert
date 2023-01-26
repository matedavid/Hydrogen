#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTextureCoords;

layout (std140) uniform Camera {
    uniform mat4 ViewProjection;
};

uniform mat4 Model;

void main() {
    gl_Position = ViewProjection * Model * vec4(aPosition, 1.0f);
}