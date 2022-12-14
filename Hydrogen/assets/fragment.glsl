#version 330 core

in vec2 v_TexCoord;
out vec4 FragColor;

uniform vec3 Color;
uniform sampler2D Texture;

void main()
{
    FragColor = texture(Texture, v_TexCoord) * vec4(Color, 1.0f);
}
