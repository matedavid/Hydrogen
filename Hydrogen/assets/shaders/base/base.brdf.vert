#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTextureCoords;

out vec2 FragTextureCoords;

void main()
{
    FragTextureCoords = aTextureCoords;
    gl_Position = vec4(aPosition, 1.0);
}