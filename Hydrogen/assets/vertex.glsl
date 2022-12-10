#version 330 core

layout (location = 0) in vec3 aPos;

uniform vec3 Color;

uniform mat4 Model;
uniform mat4 ViewProjection;

out vec3 fragColor;

void main()
{
    gl_Position = ViewProjection * Model * vec4(aPos, 1.0f);
    fragColor = Color;
}