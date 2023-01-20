#version 330 core

layout (location = 0) in vec3 aPosition;

layout(std140) uniform Camera {
    uniform mat4 ViewProjection;
    uniform vec3 Color;
};

uniform mat4 Model;

out vec3 VertexColor;

void main() {
    gl_Position = ViewProjection * Model * vec4(aPosition, 1.0f);
    VertexColor = Color;
}