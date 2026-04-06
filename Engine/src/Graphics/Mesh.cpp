#include "VertexBufferLayout.h"
#include <Mesh.h>
#include <print>


Mesh::Mesh(std::vector<Vertex> vertices, std::vector<uint> indices, std::vector<MeshTexture> textures) : 
            m_vertices(std::move(vertices)),
            m_indices(std::move(indices)),
            m_textures(std::move(textures))
{
    uint diffuseNr = 1;
    uint sepcularNr = 1;

    for (auto& tex : m_textures) {
        std::string number;
        if (tex.type == "texture_diffuse") {
            number = std::to_string(diffuseNr++);
        }
        else if (tex.type == "texture_specular") {
            number = std::to_string(sepcularNr);
        }
        tex.uniformName = "material." + tex.type + number;
    }

    setupMesh();
}





Mesh::Mesh(Mesh&& mesh) :   m_vertices(std::move(mesh.m_vertices)), m_indices(std::move(mesh.m_indices)), m_textures(std::move(mesh.m_textures)), 
                            VAO(std::move(mesh.VAO)), VBO(std::move(mesh.VBO)), EBO(std::move(mesh.EBO))
{

}

Mesh& Mesh::operator=(Mesh&& mesh) {
    m_vertices      = std::move(mesh.m_vertices);
    m_indices       = std::move(mesh.m_indices);
    m_textures      = std::move(mesh.m_textures);

    VAO             = std::move(mesh.VAO);
    VBO             = std::move(mesh.VBO);
    EBO             = std::move(mesh.EBO);
    return *this;
}

/* 着色器的纹理应该是这样的
    uniform sample2D texture_diffuse1;
    uniform sample2D texture_diffuse2;
    ...
    uniform sample2D texture_specular1;
    uniform sample2D texture_specular2; 
    ...
*/
void Mesh::Draw(Shader& shader, bool autoSetTexture) const{
    uint diffuseNr = 1;
    uint specularNr = 1;

    shader.use();
    if (autoSetTexture) {
        for (uint i = 0; i < m_textures.size(); i++) {
            shader.setInt(m_textures[i].uniformName, i);
            m_textures[i].texture->bind(i);
        }
    }

    VAO.bind();
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::setupMesh() {
    VAO.bind();

    VBO.setBuffer(&m_vertices[0], m_vertices.size() * sizeof(Vertex));

    EBO.setBuffer(&m_indices[0], m_indices.size() * sizeof(uint));

    VertexBufferLayout layout;
    layout.push<float>(3);
    layout.push<float>(3);
    layout.push<float>(2);

    VAO.addBuffer(VBO, layout);
}

void Mesh::bindVertexArray() const {
    VAO.bind();
}

uint Mesh::indexBufferCount() const{
    return m_indices.size();
}