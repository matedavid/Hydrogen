// #version 330 core

in vec2 FragTextureCoords;

struct MaterialStruct {
    vec3 ambient;

#ifdef diffuse_color
    vec3 diffuse;
#endif

#ifdef specular_color
    vec3 specular;
#endif

#ifdef shininess_def
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

#if defined(diffuse_texture)
    ResultColor = vec4(Material.ambient, 1.0f) * texture(Material.diffuse_map, FragTextureCoords);
#else
    ResultColor = vec4(Material.ambient, 1.0f);
#endif

#if defined(diffuse_texture)
    ResultColor += texture(Material.diffuse_map, FragTextureCoords);
#endif

#if defined(diffuse_color) && !defined(diffuse_texture)
    ResultColor += vec4(Material.diffuse, 1.0f);
#endif

#if defined(specular_texture)
    ResultColor += texture(Material.specular_map, FragTextureCoords);
#endif

#if defined(specular_color) && !defined(specular_texture)
    ResultColor += vec4(Material.specular, 1.0f);
#endif
}