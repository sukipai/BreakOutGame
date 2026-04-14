#include <PostProcessor.h>
#include <Shader.h>
#include <glad/glad.h>
#include <Core/log.h>

PostProcessor::PostProcessor(Shader& shader, uint width, uint height) : PostProcessingShader(&shader), texture(), width(width), height(height) {
    // 初始化 MSAA framebuffer
    glGenFramebuffers(1, &this->MSFBO);
    glGenRenderbuffers(1, &this->RBO);
   
    // Initialize renderbuffer storage with a multisampled color buffer (don't need a depth/stencil buffer)
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
    glBindRenderbuffer(GL_RENDERBUFFER, this->RBO);
    
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA8, width, height); 
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, this->RBO); 
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ENGINE_CORE_ERROR("POSTPROCESSOR: Failed to initialize MSFBO. Error Status: {:#x}", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
        
    // Also initialize the FBO/texture to blit multisampled color-buffer to; used for shader operations (for postprocessing effects)
    glGenFramebuffers(1, &this->FBO); // 生成 FBO（之前漏掉了这一行）
    glBindFramebuffer(GL_FRAMEBUFFER, this->FBO);
    glGenTextures(1, &this->texture);
    glBindTexture(GL_TEXTURE_2D, this->texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // 帧缓冲纹理不需要 Mipmap，用 GL_LINEAR 即可
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->texture, 0); 
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        ENGINE_CORE_ERROR("POSTPROCESSOR: Failed to initialize FBO. Error Status: {:#x}", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // Initialize render data and uniforms
    this->initRenderData();

    this->PostProcessingShader->use();
    this->PostProcessingShader->setInt("scene", 0);
    GLfloat offset = 1.0f / 300.0f;
    glm::vec2 offsets[9] = {
        { -offset,  offset  },  // top-left
        {  0.0f,    offset  },  // top-center
        {  offset,  offset  },  // top-right
        { -offset,  0.0f    },  // center-left
        {  0.0f,    0.0f    },  // center-center
        {  offset,  0.0f    },  // center - right
        { -offset, -offset  },  // bottom-left
        {  0.0f,   -offset  },  // bottom-center
        {  offset, -offset  }   // bottom-right    
    };

    for (uint i = 0; i < 9; ++i) {
        this->PostProcessingShader->setVec2f(std::format("offsets[{}]", i), offsets[i]);
    }

    int edge_kernel[9] = {
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    };

    for (uint i = 0; i < 9; ++i) {
        this->PostProcessingShader->setInt(std::format("edge_kernel[{}]", i), edge_kernel[i]);
    }

    float blur_kernel[9] = {
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    };
    for (uint i = 0; i < 9; ++i) {
        this->PostProcessingShader->setFloat(std::format("blur_kernel[{}]", i), blur_kernel[i]);
    }    
}

void PostProcessor::BeginRender() {
    glBindFramebuffer(GL_FRAMEBUFFER, this->MSFBO);
    glViewport(0, 0, this->width, this->height); // 关键：确保视口与 FBO 尺寸一致
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::EndRender() {
    // Now resolve multisampled color-buffer into intermediate FBO to store the resolved texture
    glBindFramebuffer(GL_READ_FRAMEBUFFER, this->MSFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, this->FBO);
    glBlitFramebuffer(0, 0, this->width, this->height, 0, 0, this->width, this->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // 注意：这里的渲染会回到默认 Framebuffer，由于 Render() 紧接着被调用，
    // 我们可以在 Render() 里面设置视口，或者在这里恢复
}

void PostProcessor::Render(float time) {
    // Set uniforms/options
    this->PostProcessingShader->use();
    this->PostProcessingShader->setFloat("time", time);
    this->PostProcessingShader->setInt("confuse", this->confuse);
    this->PostProcessingShader->setInt("chaos", this->chaos);
    this->PostProcessingShader->setInt("shake", this->shake);
    this->PostProcessingShader->setInt("scene", 0);

    // 渲染后期处理后的纹理到默认帧缓冲
    glViewport(0, 0, this->width, this->height); // 确保恢复到正确的屏幕/目标视口

    // Render textured quad
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->texture);
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessor::initRenderData() {
    uint VBO;
    GLfloat vertices[] = {
        // Pos        // Tex
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,

        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f
    };

    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(this->VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

PostProcessor::~PostProcessor() {
    glDeleteFramebuffers(1, &this->MSFBO);
    glDeleteFramebuffers(1, &this->FBO);
    glDeleteRenderbuffers(1, &this->RBO);
    glDeleteTextures(1, &this->texture);
}