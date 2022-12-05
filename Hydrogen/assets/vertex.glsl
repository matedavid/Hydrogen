#version 330 core

layout (location = 0) in vec3 aPos;

uniform vec3 aColor;
uniform mat4 TG;
uniform mat4 PM;

out vec3 fragColor;

void main()
{
    gl_Position = /*PM **/ TG * vec4(aPos, 1.0f);
    fragColor = aColor;
}