#include "mesh.h"

#include "renderer/renderer_api.h"
#include "systems/texture_system.h"

#include "material/phong_material.h"
#include "material/pbr_material.h"

namespace Hydrogen {

Mesh::Mesh(const aiMesh* mesh, const aiScene* scene, const std::string& directory) {
    // Vertices
    for (u32 i = 0; i < mesh->mNumVertices; ++i) {
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
    for (u32 i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace& face = mesh->mFaces[i];

        for (u32 index = 0; index < face.mNumIndices; ++index) {
            indices.push_back(face.mIndices[index]);
        }
    }

    const aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

    int shading_model;
    if (mat->Get(AI_MATKEY_SHADING_MODEL, shading_model) != aiReturn_SUCCESS) {
        HG_LOG_WARN("Could not determine shading model for mesh, defaulting to PBR");
        shading_model = aiShadingMode_PBR_BRDF;
    }

    if (shading_model == aiShadingMode_PBR_BRDF) {
        material = load_pbr_material(mat, directory);
    } else {
        material = load_phong_material(mat, directory);
    }

    material->build();
    setup_mesh();
}

Mesh::~Mesh() {
    delete material;
    delete VAO;
}

void Mesh::setup_mesh() {
    VAO = new VertexArray();
    VAO->bind();

    auto* VBO = new VertexBuffer(&vertices[0], (u32)vertices.size() * sizeof(Vertex));
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

    const auto* EBO = new IndexBuffer(&indices[0], (u32)indices.size());

    VAO->add_vertex_buffer(VBO);
    VAO->set_index_buffer(EBO);

    VAO->unbind();
    VBO->unbind();
    EBO->unbind();
}

IMaterial* Mesh::load_phong_material(const aiMaterial* mat, const std::string& directory) {
    auto* phong_material = new PhongMaterial();

    // Diffuse texture
    aiString ai_path;
    if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &ai_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(ai_path.C_Str());
        phong_material->diffuse_map = TextureSystem::instance->acquire(path);
    }

    // Specular texture
    ai_path.Clear();
    if (mat->GetTexture(aiTextureType_SPECULAR, 0, &ai_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(ai_path.C_Str());
        phong_material->specular_map = TextureSystem::instance->acquire(path);
    }

    // Normal texture
    ai_path.Clear();
    if (mat->GetTexture(aiTextureType_HEIGHT, 0, &ai_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(ai_path.C_Str());
        phong_material->normal_map = TextureSystem::instance->acquire(path);
    }

    // Ambient, diffuse and specular colors
    aiColor3D color;
    if (mat->Get(AI_MATKEY_COLOR_AMBIENT, color) == aiReturn_SUCCESS) {
        phong_material->ambient = glm::vec3(color.r, color.g, color.b);
    }

    if (mat->Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS) {
        phong_material->diffuse = glm::vec3(color.r, color.g, color.b);
    }

    if (mat->Get(AI_MATKEY_COLOR_SPECULAR, color) == aiReturn_SUCCESS) {
        phong_material->specular = glm::vec3(color.r, color.g, color.b);
    }

    // Shininess
    ai_real shininess;
    if (mat->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
        phong_material->shininess = shininess;
    }

    return phong_material;
}

IMaterial* Mesh::load_pbr_material(const aiMaterial* mat, const std::string& directory) {
    auto* pbr_material = new PBRMaterial();

    // Albedo color
    aiColor3D color;
    if (mat->Get(AI_MATKEY_BASE_COLOR, color) == aiReturn_SUCCESS) {
        pbr_material->albedo = glm::vec3(color.r, color.g, color.b);
    }

    // Metallic value
    ai_real value;
    if (mat->Get(AI_MATKEY_METALLIC_FACTOR, value) == aiReturn_SUCCESS) {
        pbr_material->metallic = value;
    }

    // Roughness value
    if (mat->Get(AI_MATKEY_ROUGHNESS_FACTOR, value) == aiReturn_SUCCESS) {
        pbr_material->roughness = value;
    }

    // AO value
    // TODO: Could not find it in assimp docs

    // Albedo texture
    aiString albedo_path;
    if (mat->GetTexture(AI_MATKEY_BASE_COLOR_TEXTURE, &albedo_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(albedo_path.C_Str());
        pbr_material->albedo_map = TextureSystem::instance->acquire(path);
    }

    // Metallic texture
    aiString metallic_path;
    if (mat->GetTexture(AI_MATKEY_METALLIC_TEXTURE, &metallic_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(metallic_path.C_Str());
        pbr_material->metallic_map = TextureSystem::instance->acquire(path);
    }

    // Roughness texture
    aiString roughness_path;
    if (mat->GetTexture(AI_MATKEY_ROUGHNESS_TEXTURE, &roughness_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(roughness_path.C_Str());
        pbr_material->roughness_map = TextureSystem::instance->acquire(path);
    }

    // AO texture
    aiString ao_path;
    if (mat->GetTexture(aiTextureType_LIGHTMAP, 0, &ao_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(ao_path.C_Str());
        pbr_material->ao_map = TextureSystem::instance->acquire(path);
    }

    // Normal texture
    aiString normal_path;
    if (mat->GetTexture(aiTextureType_NORMALS, 0, &normal_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(normal_path.C_Str());
        pbr_material->normal_map = TextureSystem::instance->acquire(path);
    }

    // Check if metallic and roughness textures are the same image
    pbr_material->metallic_roughness_same_texture = !std::string(metallic_path.C_Str()).empty()
                                                    && metallic_path == roughness_path;

    // Check if metallic, roughness and ao texture are the same image
    pbr_material->metallic_roughness_ao_same_texture = pbr_material->metallic_roughness_same_texture
                                                       && ao_path == metallic_path;

    return pbr_material;
}

} // namespace Hydrogen
