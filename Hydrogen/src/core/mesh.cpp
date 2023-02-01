#include "mesh.h"

#include "renderer/renderer_api.h"
#include "systems/texture_system.h"

namespace Hydrogen {

Mesh::Mesh(const aiMesh* mesh, const aiScene* scene, const std::string& directory) {
    // Vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex{};

        // Position
        const auto& vs = mesh->mVertices[i];
        vertex.position.x = vs.x;
        vertex.position.y = vs.y;
        vertex.position.z = vs.z;

        // Normals
        if (mesh->HasNormals()) {
            const auto& ns = mesh->mNormals[i];
            vertex.normal.x = ns.x;
            vertex.normal.y = ns.y;
            vertex.normal.z = ns.z;
        }

        // Texture coordinates
        if (mesh->HasTextureCoords(0)) {
            const auto& tc = mesh->mTextureCoords[0][i];
            vertex.texture_coordinates.x = tc.x;
            vertex.texture_coordinates.y = tc.y;
        }

        // Tangents
        if (mesh->HasTangentsAndBitangents()) {
            const auto ts = mesh->mTangents[i];
            vertex.tangent.x = ts.x;
            vertex.tangent.y = ts.y;
            vertex.tangent.z = ts.z;
        }

        vertices.push_back(vertex);
    }

    // Indices
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];

        for (unsigned int index = 0; index < face.mNumIndices; ++index) {
            indices.push_back(face.mIndices[index]);
        }
    }

    const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

    // Diffuse texture
    aiString ai_path;
    if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &ai_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(ai_path.C_Str());
        material.values.diffuse_map = TextureSystem::instance->acquire(path);
    }

    // Specular texture
    ai_path.Clear();
    if (mat->GetTexture(aiTextureType_SPECULAR, 0, &ai_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(ai_path.C_Str());
        material.values.specular_map = TextureSystem::instance->acquire(path);
    }

    // Normal texture
    ai_path.Clear();
    if (mat->GetTexture(aiTextureType_HEIGHT, 0, &ai_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(ai_path.C_Str());
        material.values.normal_map = TextureSystem::instance->acquire(path);
    }

    // Ambient, diffuse and specular colors
    aiColor3D color;
    if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == aiReturn_SUCCESS) {
        material.values.ambient = glm::vec3(color.r, color.g, color.b);
    }

    if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
        material.values.diffuse = glm::vec3(color.r, color.g, color.b);
    }

    if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS) {
        material.values.specular = glm::vec3(color.r, color.g, color.b);
    }

    // Shininess
    ai_real shininess;
    if (mat->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
        material.values.shininess = shininess;
    }

    material.build();

    setup_mesh();
}

Mesh::~Mesh() {
    delete VAO;
}

void Mesh::setup_mesh() {
    VAO = new VertexArray();
    VAO->bind();

    auto* VBO = new VertexBuffer(&vertices[0], (unsigned int)vertices.size() * sizeof(Vertex));
    VBO->set_layout({
        // Vertex positions
        {.type = ShaderType::Float32, .count = 3, .normalized = false},
        // Vertex normals
        {.type = ShaderType::Float32, .count = 3, .normalized = false},
        // Vertex texture coords
        {.type = ShaderType::Float32, .count = 2, .normalized = false},
        // Vertex tangents
        {.type = ShaderType::Float32, .count = 3, .normalized = false}
    });

    const auto* EBO = new IndexBuffer(&indices[0], (int)indices.size());

    VAO->add_vertex_buffer(VBO);
    VAO->set_index_buffer(EBO);

    VAO->unbind();
    VBO->unbind();
    EBO->unbind();
}

} // namespace Hydrogen
