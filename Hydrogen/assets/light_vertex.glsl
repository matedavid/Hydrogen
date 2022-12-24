#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 v_TexCoord;

uniform mat4 Model;
uniform mat4 ViewProjection;

void main()
{
    gl_Position = ViewProjection * Model * vec4(aPos, 1.0f);
    v_TexCoord = aTexCoord;
}
