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
    auto* system = TextureSystem::get();

    // Diffuse texture
    aiString ai_path;
    if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &ai_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(ai_path.C_Str());
        material.diffuse_map = system->acquire(path);
    } else {
        material.diffuse_map = system->default_texture();
    }

    // Specular texture
    ai_path.Clear();
    if (mat->GetTexture(aiTextureType_SPECULAR, 0, &ai_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(ai_path.C_Str());
        material.specular_map = system->acquire(path);
    } else {
        material.specular_map = system->default_texture();
    }

    // Ambient, diffuse and specular colors
    aiColor3D color;
    if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == aiReturn_SUCCESS) {
        material.ambient.r = color.r;
        material.ambient.g = color.g;
        material.ambient.b = color.b;
    }

    if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
        material.diffuse.r = color.r;
        material.diffuse.g = color.g;
        material.diffuse.b = color.b;
    }

    if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS) {
        material.specular.r = color.r;
        material.specular.g = color.g;
        material.specular.b = color.b;
    }

    // Shininess
    ai_real shininess;
    if (mat->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
        material.shininess = shininess;
    } else {
        material.shininess = 1.0f;
    }

    setup_mesh();
}

Mesh::~Mesh() {
    delete VAO;

    auto* system = TextureSystem::get();
    if (material.diffuse_map != nullptr) {
        system->release(material.diffuse_map);
    }
    if (material.specular_map != nullptr) {
        system->release(material.specular_map);
    }
}

void Mesh::draw(Shader* shader) {
    material.diffuse_map->bind(0);
    shader->set_uniform_int("Material.diffuse", 0);
    material.specular_map->bind(1);
    shader->set_uniform_int("Material.specular", 1);

    shader->set_uniform_float("Material.shininess", material.shininess);

    RendererAPI::send(VAO, shader);
}

void Mesh::setup_mesh() {
    VAO = new VertexArray();
    VAO->bind();

    auto* VBO = new VertexBuffer(&vertices[0], (unsigned int)vertices.size() * sizeof(Vertex));
    VBO->set_layout({
        // Vertex positions
        {.type = ShaderType::Float32, .count = 3, .normalized = false},
        // Color
        {.type = ShaderType::Float32, .count = 4, .normalized = false},
        // Vertex normals
        {.type = ShaderType::Float32, .count = 3, .normalized = false},
        // Vertex texture coords
        {.type = ShaderType::Float32, .count = 2, .normalized = false}
    });

    const auto* EBO = new IndexBuffer(&indices[0], (int)indices.size());

    VAO->add_vertex_buffer(VBO);
    VAO->set_index_buffer(EBO);

    VAO->unbind();
    VBO->unbind();
    EBO->unbind();
}

} // namespace Hydrogen
