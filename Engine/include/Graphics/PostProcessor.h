#ifndef __ZSH_ENGINE_POST_PROCESSOR_H__
#define __ZSH_ENGINE_POST_PROCESSOR_H__

#include <Texture.h>

class PostProcessor {
    using uint = unsigned int;
public:
    PostProcessor(class Shader& shader, uint width, uint height);
    ~PostProcessor();

    void BeginRender();
    void EndRender();
    void Render(float time);

    void Shake(bool shake) { this->shake = shake; }
    void Confuse(bool confuse) { this->confuse = confuse; }
    void Chaos(bool chaos) { this->chaos = chaos; }

private:
    class Shader* PostProcessingShader;
    uint texture;
    uint width, height;
    uint MSFBO, FBO, RBO;
    uint VAO;
    bool confuse, chaos, shake;
    void initRenderData();
};

#endif