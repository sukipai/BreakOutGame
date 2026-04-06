#include <SpriteRenderer.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

SpriteRenderer::SpriteRenderer(Shader& shader) : shader(shader), quadVAO(), quadVBO() {
    initRenderData();
}

void SpriteRenderer::DrawSprite(Texture& texture, glm::vec2 pos, glm::vec2 size, glm::vec3 color, float rotate) {
    this->shader.use();

    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(pos, 0.0f));

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f));
    model = glm::rotate(model ,rotate, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));

    model= glm::scale(model, glm::vec3(size, 1.0f));

    this->shader.setMatrix4f("model", model);
    this->shader.setVec3f("spriteColor", color);

    texture.bind(0);
    quadVAO.bind();
    glDrawArrays(GL_TRIANGLES, 0, 6);
    quadVAO.unbind();
}

void SpriteRenderer::initRenderData() {
    float vertices[] = {
        // 位置             // 纹理
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f, 

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    quadVAO.bind();
    
    quadVBO.setBuffer(vertices, sizeof(vertices));
    VertexBufferLayout layout;
    layout.push<float>(4);

    quadVAO.addBuffer(quadVBO, layout);

    quadVAO.unbind();
    quadVBO.unbind();
}

SpriteRenderer::~SpriteRenderer() {
    
}