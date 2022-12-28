#pragma once

#include "core.h"

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "renderer/shader.h"
#include "models/mesh.h"

namespace Hydrogen {

class HG_API Model {
  public:
    Model(const std::string& path);
    ~Model();

    void draw(Shader* shader);

  private:
    std::vector<Mesh*> m_meshes;
    std::string m_directory;

    void process_node_r(aiNode* node, const aiScene* scene);
};

}
