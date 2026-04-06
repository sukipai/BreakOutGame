#ifndef __ZSH_ENGINE_SPRITERENDERER_H
#define __ZSH_ENGINE_SPRITERENDERER_H

#include <Shader.h>
#include <Texture.h>
#include <VertexArray.h>

class SpriteRenderer {
public:
    SpriteRenderer(Shader & shader);
    ~SpriteRenderer();

    void DrawSprite(Texture& texture, glm::vec2 pos, 
                    glm::vec2 size = glm::vec2(10, 10), glm::vec3 color = glm::vec3(1.0f),
                    float rotate = 0.0f);

private:
    Shader& shader;
    VertexArray quadVAO;
    VertexBuffer quadVBO;

    void initRenderData();
};


#endif