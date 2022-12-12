#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 Model;
uniform mat4 ViewProjection;

void main()
{
    gl_Position = ViewProjection * Model * vec4(aPos, 1.0f);
}