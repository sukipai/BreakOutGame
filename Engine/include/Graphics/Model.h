#pragma once

#include <vector>
#include <Mesh.h>

class Model {
public:
    Model(const char* path);

    void Draw(Shader& shader, bool autoSetTexture=true);

    const std::vector<Mesh> & getMeshes() const;

    std::vector<Mesh>& getMeshes();

private:
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<MeshTexture> textures_loaded;

    void loadModel(const std::string &path);
    void processNode(struct aiNode* node, const struct aiScene* scene);
    Mesh processMesh(struct aiMesh* mesh, const struct aiScene* scene);

    std::vector<MeshTexture> loadMaterialTextures(struct aiMaterial* mat, unsigned int type, std::string typeName);
};