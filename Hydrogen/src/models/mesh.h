#pragma once

#include "core.h"

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <assimp/scene.h>

#include "material.h"
#include "renderer/vertex_array.h"
#include "renderer/buffers.h"
#include "renderer/shader.h"
#include "renderer/texture.h"

namespace Hydrogen {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinates;
};

class HG_API Mesh {
  public:
    VertexArray* VAO;
    Material material;

    Mesh(const aiMesh* mesh, const aiScene* scene, const std::string& directory);
    ~Mesh();

  private:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    void setup_mesh();
};

}
