#ifndef __ZSH_ENGINE_PARTICLESYSTEM_H
#define __ZSH_ENGINE_PARTICLESYSTEM_H

#include "VertexArray.h"
#include <glm/glm.hpp>

// 粒子描述
struct ParticleProps {
    glm::vec2 Position;
    glm::vec2 Velocity, VelocityVariation;
    glm::vec4 ColorBegin, ColorEnd;
    float SizeBegin, SizeEnd, SizeVariation;
    float LifeTime = 1.0f;
};

// 内部粒子状态
struct Particle {
    glm::vec2   Position;
    glm::vec2   Velocity;
    glm::vec4   ColorBegin, ColorEnd;
    float       SizeBegin, SizeEnd;

    float       LifeTime = 1.0f;
    float       LifeRemaining = 0.0f; 
    bool        Active;
};

class ParticleSystem{
public:
    // 单例模式
    static ParticleSystem& GetInstance();

    // 初始化单例（在游戏初始化时调用）
    static void Initialize(class Shader& shader, class Texture& texture, uint32_t maxParticle = 1000);

    // 静态方法包装器
    static void Update(float deltaTime);
    static void Draw();
    static void Emit(const ParticleProps& particleProps);

    // 防止复制和移动
    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem& operator=(const ParticleSystem&) = delete;

private:
    // 私有构造函数
    ParticleSystem(class Shader* shader, class Texture* texture, uint32_t maxParticle);

    // 私有析构函数
    ~ParticleSystem();

    // 实例方法
    void _Update(float deltaTime);
    void _Draw();
    void _Emit(const ParticleProps& particleProps);

    void init();

private:
    static ParticleSystem* m_instance;

    std::vector<Particle> m_ParticlePool;
    uint32_t m_PoolIndex = 999;

    Shader* m_shader;
    Texture* m_texture;
    VertexArray m_VAO;
    VertexBuffer m_VBO;

};


#endif