#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec2 aTexCoords;

out vec3 Normal;
out vec2 TexCoords;
out vec3 FragPos;

uniform mat4 Model;
uniform mat4 ViewProjection;

void main()
{
    Normal = mat3(transpose(inverse(Model))) * aNormal;
    TexCoords = aTexCoords;
    FragPos = vec3(Model * vec4(aPos, 1.0));
    gl_Position = ViewProjection * Model * vec4(aPos, 1.0);
}