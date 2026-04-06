#pragma once
#include <IndexBuffer.h>
#include <VertexArray.h>
#include <glm/glm.hpp>
#include <Texture.h>
#include <Shader.h>
#include <memory>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct MeshTexture {
    std::shared_ptr<Texture> texture;
    std::string type;
    std::string path;
    std::string uniformName;
};

class Mesh {
    using uint = unsigned int;
public: 
    std::vector<Vertex>         m_vertices;
    std::vector<uint>           m_indices;
    std::vector<MeshTexture>    m_textures;

    Mesh(std::vector<Vertex> vertices, std::vector<uint> indices, std::vector<MeshTexture> textures);

    Mesh(Mesh&& mesh);

    ~Mesh() = default;

    Mesh& operator=(Mesh&& mesh);

    void Draw(Shader& shader, bool autoSetTexture = false) const;

    void bindVertexArray() const;

    uint indexBufferCount() const;

private:
    void setupMesh();
    VertexArray         VAO;
    VertexBuffer        VBO;
    IndexBuffer         EBO;
};

