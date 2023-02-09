#pragma once

#include "core.h"

#include <string>
#include <vector>

#include "glm/glm.hpp"
#include "assimp/scene.h"

#include "renderer/vertex_array.h"
#include "renderer/buffers.h"
#include "renderer/shader.h"
#include "renderer/texture.h"

#include "material/material.h"

namespace Hydrogen {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texture_coordinates;
    glm::vec3 tangent;
};

class HG_API Mesh {
  public:
    VertexArray* VAO;
    IMaterial* material = nullptr;

    Mesh(const aiMesh* mesh, const aiScene* scene, const std::string& directory);
    ~Mesh();

  private:
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    void setup_mesh();

    // Material loaders
    IMaterial* load_phong_material(const aiMaterial* mat, const std::string& directory);
    IMaterial* load_pbr_material(const aiMaterial* mat, const std::string& directory);
};

}
