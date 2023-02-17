#version 330 core

layout(location = 0) in vec3 aPosition;

uniform mat4 Projection;
uniform mat4 View;

out vec3 FragPosition;

void main() {
    FragPosition = aPosition;
    gl_Position =  Projection * View * vec4(aPosition, 1.0);
}
