#include "mesh.h"

#include "renderer/renderer_api.h"

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

        // Color
        if (mesh->HasVertexColors(0)) {
            const auto& cs = mesh->mColors[0][i];
            vertex.color.r = cs.r;
            vertex.color.g = cs.g;
            vertex.color.b = cs.b;
            vertex.color.a = cs.a;
        }

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
    if (white_texture == nullptr) {
        white_texture = Texture::white();
    }

    // Diffuse texture
    aiString ai_path;
    if (mat->GetTexture(aiTextureType_DIFFUSE, 0, &ai_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(ai_path.C_Str());
        // auto* texture = new Texture(path);
        // textures.push_back(texture);
        diffuse_texture = new Texture(path);
    } else {
        diffuse_texture = white_texture;
    }

    ai_path.Clear();
    if (mat->GetTexture(aiTextureType_SPECULAR, 0, &ai_path) == aiReturn_SUCCESS) {
        const std::string path = directory + std::string(ai_path.C_Str());
        // auto* texture = new Texture(path);
        // textures.push_back(texture);
        specular_texture = new Texture(path);
    } else {
        specular_texture = white_texture;
    }

    // Material
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

    ai_real shininess;
    if (mat->Get(AI_MATKEY_SHININESS, shininess) == aiReturn_SUCCESS) {
        material.shininess = shininess;
    } else {
        material.shininess = 1.0f;
    }

    std::cout << "Mesh:\n";
    std::cout << material.ambient.r << " " << material.ambient.g << " " << material.ambient.b << "\n";
    std::cout << material.diffuse.r << " " << material.diffuse.g << " " << material.diffuse.b << "\n";
    std::cout << material.specular.r << " " << material.specular.g << " " << material.specular.b << "\n";
    std::cout << material.shininess << "\n";
    std::cout << "\n";

    setup_mesh();
}

Mesh::~Mesh() {
    delete VAO;
//    for (auto* texture : textures) {
//        delete texture;
//    }

    if (diffuse_texture != white_texture) {
        delete diffuse_texture;
    }
    if (specular_texture != white_texture) {
        delete white_texture;
    }
}

void Mesh::draw(Shader* shader) {
    /*
    if (textures.size() > 0) {
        textures[0]->bind(0);
        shader->set_uniform_int(0, "Texture");
    }
     */

    diffuse_texture->bind(0);
    shader->set_uniform_int(0, "Material.diffuse");
    specular_texture->bind(1);
    shader->set_uniform_int(1, "Material.Specular");

    // shader->set_uniform_vec3(material.ambient, "Material.ambient");
    // shader->set_uniform_vec3(material.diffuse, "Material.diffuse");
    // shader->set_uniform_vec3(material.specular, "Material.specular");
    shader->set_uniform_float(material.shininess, "Material.shininess");

    RendererAPI::send(VAO, shader);

    /*
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;

    for(unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i); // activate texture unit first
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name = textures[i].type;
        if(name == "texture_diffuse")
            number = std::to_string(diffuseNr++);
        else if(name == "texture_specular")
            number = std::to_string(specularNr++);

        std::cout << name + number << std::endl;
        shader->set_uniform_int((int)i, name + number);
        glBindTexture(GL_TEXTURE_2D, textures[i].ID);
    }
    glActiveTexture(GL_TEXTURE0);

    // draw mesh
    RendererAPI::send(VAO, shader);
     */
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

}
