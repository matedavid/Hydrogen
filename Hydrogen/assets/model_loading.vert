#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

out vec4 Color;
out vec2 TexCoords;

uniform mat4 Model;
uniform mat4 ViewProjection;

void main()
{
    Color = aColor;
    TexCoords = aTexCoords;
    gl_Position = ViewProjection * Model * vec4(aPos, 1.0);
}