// #version 330 core

struct MaterialStruct {
    vec3 ambient;

#ifdef diffuse_color
    vec3 diffuse;
#endif

#ifdef specular_color
    vec3 specular;
#endif

#ifdef shininess
    float shininess;
#endif

#ifdef diffuse_texture
    sampler2D diffuse_map;
#endif

#ifdef specular_texture
    sampler2D specular_map;
#endif
};

uniform MaterialStruct Material;

out vec4 ResultColor;

void main() {
#ifdef diffuse_color
    ResultColor = vec4(Material.diffuse, 1.0f);
#else
    ResultColor = vec4(Material.ambient, 1.0f);
#endif
}