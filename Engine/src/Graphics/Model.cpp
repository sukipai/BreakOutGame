#include "Mesh.h"
#include <Model.h>
#include <iterator>
#include <memory>
#include <print>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


Model::Model(const char* path) {
    loadModel(path);
}

void Model::Draw(Shader& shader, bool autoSetTexture) {
    for (unsigned int i = 0; i < meshes.size(); i++) {
        meshes[i].Draw(shader, autoSetTexture);
    }
}

void Model::loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::println("Error: ASSIMP::{}", importer.GetErrorString());
        return;
    }

    // std::println("Model: starting to load Node");

    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // std::println("Modle: Process Node: {}", node->mName.C_Str());

    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    // std::println("Model: Process Mesh: {}", mesh->mName.C_Str());
    std::vector<Vertex>         vertices;
    std::vector<unsigned int>   indices;
    std::vector<MeshTexture>    textures;

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumVertices * 3);

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // TODO: 处理顶点位置、法线和纹理坐标
        glm::vec3 vector;
        vector.x =          mesh->mVertices[i].x;
        vector.y =          mesh->mVertices[i].y;
        vector.z =          mesh->mVertices[i].z;
        vertex.Position =   vector;

        vector.x =          mesh->mNormals[i].x;
        vector.y =          mesh->mNormals[i].y;
        vector.z =          mesh->mNormals[i].z;
        vertex.Normal =     vector;

        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
        }
        else {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // TODO: 处理索引
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        
        std::vector<MeshTexture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), 
                        std::make_move_iterator(diffuseMaps.begin()), 
                        std::make_move_iterator(diffuseMaps.end()));
        
        // 反射纹理
        std::vector<MeshTexture> reflectMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_reflect");
        textures.insert(textures.end(), 
                            std::make_move_iterator(reflectMaps.begin()),
                             std::make_move_iterator(reflectMaps.end()));
    
        std::vector<MeshTexture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), 
                        std::make_move_iterator(specularMaps.begin()), 
                        std::make_move_iterator(specularMaps.end()));
    }
    return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

std::vector<MeshTexture> Model::loadMaterialTextures(aiMaterial* mat, unsigned int type, std::string typeName) {
    // std::println("Model: loading material: {}", mat->GetName().C_Str());
    
    std::vector<MeshTexture> textures;
    auto itype = static_cast<aiTextureType>(type);
    for (uint i = 0; i < mat->GetTextureCount(itype); i++) {
        aiString str;
        mat->GetTexture(itype, i, &str);

        bool skip = false;

        for (uint j = 0; j < textures_loaded.size(); j++) {
            if (textures_loaded[j].path == str.C_Str()) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
            
        }

        if (!skip) {
            MeshTexture texture;
            texture.type = typeName;
            texture.path = str.C_Str();
            
            texture.texture = std::make_shared<Texture>();
            texture.texture->setTexture(directory + "/" + str.C_Str());

            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
        
    }

    return textures;
}

const std::vector<Mesh>& Model::getMeshes() const {
    return meshes;
}

std::vector<Mesh>& Model::getMeshes() {
    return meshes;
}
