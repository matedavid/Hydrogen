#version 330 core

out vec4 FragColor;

in vec4 Color;
in vec2 TexCoords;

uniform sampler2D Texture;

void main()
{
    FragColor = texture(Texture, TexCoords);
    // FragColor = vec4(0.8, 0.8, 0.8, 1.0f);
}
