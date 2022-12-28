#include "model.h"

#include <iostream>

namespace Hydrogen {

Model::Model(const std::string& path) {
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        std::cout << "Error loading model " << path << ": " << importer.GetErrorString() << std::endl;
        return;
    }

    m_directory = path.substr(0, path.find_last_of('/')) + "/";
    process_node_r(scene->mRootNode, scene);
}

Model::~Model() {
    for (auto* mesh : m_meshes) {
        delete mesh;
    }
}

void Model::draw(Shader* shader) {
    for (auto& mesh : m_meshes) {
        mesh->draw(shader);
    }
}

void Model::process_node_r(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        const aiMesh* m = scene->mMeshes[node->mMeshes[i]];

        auto* mesh = new Mesh(m, scene, m_directory);
        m_meshes.push_back(mesh);
    }

    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        process_node_r(node->mChildren[i], scene);
    }
}

}
