#pragma once

#include "core.h"

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "renderer/vertex_array.h"
#include "renderer/buffers.h"
#include "renderer/shader.h"
#include "renderer/texture.h"

namespace Hydrogen {

struct Vertex {
    glm::vec3 position;
    glm::vec4 color{1.0f};
    glm::vec3 normal;
    glm::vec2 texture_coordinates;
};

struct Material {
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};

class HG_API Mesh {
  public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Texture* diffuse_texture;
    Texture* specular_texture;

    Material material;

    Mesh(const aiMesh* mesh, const aiScene* scene, const std::string& directory);
    ~Mesh();

    void draw(Shader* shader);

  private:
    VertexArray* VAO;

    void setup_mesh();
};

}
