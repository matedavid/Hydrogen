#version 330 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform vec3 Color;
uniform sampler2D Texture;

uniform vec3 LightColor;

void main()
{
    FragColor = vec4(Color * LightColor, 1.0f);
}
