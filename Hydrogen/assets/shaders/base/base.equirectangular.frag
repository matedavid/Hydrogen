#version 330 core

in vec3 FragPosition;
uniform sampler2D EquirectangularMap;

out vec4 ResultColor;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v);

void main() {
    vec2 uv = SampleSphericalMap(normalize(FragPosition));
    vec3 color = texture(EquirectangularMap, uv).rgb;

    ResultColor = vec4(color, 1.0);
}

vec2 SampleSphericalMap(vec3 v) {
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}
