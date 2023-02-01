#include "model.h"

#include <iostream>

namespace Hydrogen {

Model::Model(const std::string& path, bool flip_uvs) {
    Assimp::Importer importer;

    unsigned int flags = aiProcess_Triangulate;
    if (flip_uvs)
        flags |= aiProcess_FlipUVs;

    const aiScene* scene = importer.ReadFile(path, flags);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        HG_LOG_ERROR("Error loading model {} with error: {}", path, importer.GetErrorString());
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

const std::vector<Mesh*>& Model::get_meshes() const {
    return m_meshes;
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

} // namespace Hydrogen