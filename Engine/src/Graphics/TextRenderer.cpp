#include <Graphics/TextRenderer.h>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Core/log.h>

TextRenderer::TextRenderer(Shader& shader, const glm::mat4& projection)
    : m_shader(shader), m_quadVAO(), m_quadVBO(), m_projection(projection) {
    initRenderData();
}

TextRenderer::~TextRenderer() {
    // Vertex buffers and arrays will clean up themselves via RAII
}

void TextRenderer::initRenderData() {
    // Vertex data for a quad: position (x, y) and texture coordinates (s, t)
    // Same as SpriteRenderer for consistency
    float vertices[] = {
        // pos      // tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    m_quadVAO.bind();
    m_quadVBO.setBuffer(vertices, sizeof(vertices));

    // Vertex buffer layout: 4 floats per vertex (xy = position, zw = tex coords)
    VertexBufferLayout layout;
    layout.push<float>(4); // single vec4 attribute

    m_quadVAO.addBuffer(m_quadVBO, layout);
    m_quadVAO.unbind();
    m_quadVBO.unbind();
}

void TextRenderer::renderText(Font& font, const std::string& text,
                             glm::vec2 position, float scale,
                             glm::vec3 color) {
    // Debug logging (first call only)
    static bool firstCall = true;
    if (firstCall) {
        firstCall = false;
    }

    // Ensure alpha blending is enabled so glyph quads only show glyph pixels
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader.use();
    m_shader.setMatrix4f("projection", m_projection);
    m_shader.setVec3f("textColor", color);
    m_shader.setInt("text", 0); // texture unit 0

    // Bind the font atlas texture
    Texture& atlas = font.getAtlasTexture();
    atlas.bind(0);

    m_quadVAO.bind();

    // Debug: check first character
    static bool checkedFirstChar = false;
    if (!checkedFirstChar && !text.empty()) {
        const Character& firstChar = font.getCharacter(text[0]);
        checkedFirstChar = true;
    }

    // Iterate through characters
    float x = position.x;
    float y = position.y;

    int charCount = 0;
    for (char c : text) {
        charCount++;
        const Character& ch = font.getCharacter(c);

        if (c == ' ') {
            // Space character: just advance
            x += (ch.advance >> 6) * scale;
            continue;
        }

        if (c == '\n') {
            // Newline: reset x, move y down
            x = position.x;
            y -= font.getLineHeight(scale);
            continue;
        }

        // Calculate quad position and size (screen coordinates: y down)
        float xpos = x + ch.bearing.x * scale;
        float ypos = y - ch.bearing.y * scale; // bearing.y is positive, moving up from baseline

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        // Update VBO for each character (could be optimized with instancing)
        // For now, we'll update the vertex data and draw each character separately

        // Create model matrix
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xpos, ypos, 0.0f));
        model = glm::scale(model, glm::vec3(w, h, 1.0f));

        m_shader.setMatrix4f("model", model);

        // Set texture coordinates for this character
        // We need to pass texture coordinates to the shader
        // Since our quad uses 0-1 tex coords, we'll use a uniform for the atlas region
        m_shader.setVec4f("texCoordsRect",
                         glm::vec4(ch.texCoords.x,
                                  ch.texCoords.y,
                                  ch.texSize.x,
                                  ch.texSize.y));

        // Draw quad
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Advance to next character
        x += (ch.advance >> 6) * scale; // advance is in 1/64 pixels
    }

    // Debug log
    static int renderCallCount = 0;
    renderCallCount++;


    m_quadVAO.unbind();

    // Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        ENGINE_CORE_ERROR("OpenGL error in TextRenderer::renderText: {}", err);
    }
}