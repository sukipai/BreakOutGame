#ifndef __ZSH_ENGINE_TEXTRENDERER_H
#define __ZSH_ENGINE_TEXTRENDERER_H

#include <Shader.h>
#include <Font.h>
#include <VertexArray.h>
#include <VertexBuffer.h>
#include <glm/glm.hpp>
#include <string>

class TextRenderer {
private:
    Shader& m_shader;
    VertexArray m_quadVAO;
    VertexBuffer m_quadVBO;
    glm::mat4 m_projection;

    void initRenderData();

public:
    TextRenderer(Shader& shader, const glm::mat4& projection);
    ~TextRenderer();

    // Render text using specified font
    void renderText(Font& font, const std::string& text,
                   glm::vec2 position, float scale = 1.0f,
                   glm::vec3 color = glm::vec3(1.0f));

    // Update projection matrix
    void setProjection(const glm::mat4& projection) { m_projection = projection; }

    // Get the shader reference
    Shader& getShader() { return m_shader; }
};

#endif // __ZSH_ENGINE_TEXTRENDERER_H