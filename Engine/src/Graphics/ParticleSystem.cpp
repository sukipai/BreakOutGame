#include <ParticleSystem.h>
#include <random>
#include <Shader.h>
#include <Texture.h>

// 静态成员初始化
ParticleSystem* ParticleSystem::m_instance = nullptr;

// 获取单例实例
ParticleSystem& ParticleSystem::GetInstance() {
    if (m_instance == nullptr) {
        throw std::runtime_error("ParticleSystem not initialized! Call Initialize() first.");
    }
    return *m_instance;
}

// 初始化单例
void ParticleSystem::Initialize(Shader& shader, Texture& texture, uint32_t maxParticle) {
    if (m_instance != nullptr) {
        delete m_instance;
    }
    m_instance = new ParticleSystem(&shader, &texture, maxParticle);
}

// 静态方法包装器
void ParticleSystem::Update(float deltaTime) {
    GetInstance()._Update(deltaTime);
}

void ParticleSystem::Draw() {
    GetInstance()._Draw();
}

void ParticleSystem::Emit(const ParticleProps& particleProps) {
    GetInstance()._Emit(particleProps);
}

// 私有构造函数
ParticleSystem::ParticleSystem(Shader* shader, Texture* texture, uint32_t maxParticle)
    : m_shader(shader), m_texture(texture)
{
    m_ParticlePool.resize(maxParticle);
    m_PoolIndex = maxParticle - 1;
    this->init();
}

// 私有析构函数
ParticleSystem::~ParticleSystem() {
}

void ParticleSystem::_Update(float deltaTime) {
    for (auto& particle : m_ParticlePool) {
        if (!particle.Active) {
            continue;
        }

        if (particle.LifeRemaining <= 0.0f) {
            particle.Active = false;
            continue;
        }

        particle.LifeRemaining -= deltaTime;
        particle.Position += particle.Velocity * deltaTime;
    }
}

void ParticleSystem::_Emit(const ParticleProps& particleProps) {
    Particle& particle = m_ParticlePool[m_PoolIndex];
    particle.Active = true;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dist(-0.5f, 0.5f);

    particle.Position = particleProps.Position;
    particle.Velocity = particleProps.Velocity + particleProps.VelocityVariation * glm::vec2(dist(gen), dist(gen));

    particle.ColorBegin = particleProps.ColorBegin;
    particle.ColorEnd = particleProps.ColorEnd;

    particle.SizeBegin = particleProps.SizeBegin + particleProps.SizeVariation * dist(gen);
    particle.SizeEnd = particleProps.SizeEnd;

    particle.LifeTime = particleProps.LifeTime;
    particle.LifeRemaining = particleProps.LifeTime;

    m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}

void ParticleSystem::_Draw() {
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    this->m_shader->use();
    this->m_texture->bind(0);
    this->m_VAO.bind();

    for (auto &particle : m_ParticlePool) {
        if (!particle.Active) continue;

        float life = particle.LifeRemaining / particle.LifeTime;

        glm::vec4 color = glm::mix(particle.ColorEnd, particle.ColorBegin, life);
        float size = glm::mix(particle.SizeEnd, particle.SizeBegin, life);

        this->m_shader->setVec2f("offset", particle.Position);
        this->m_shader->setVec4f("color", color);
        this->m_shader->setFloat("scale", size);

        glDrawArrays(GL_TRIANGLES,0, 6);
    }
    this->m_VAO.unbind();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem::init() {
    // 位置 (x, y) 范围 -0.5 到 0.5 
    // 纹理 (u, v) 范围依然是 0.0 到 1.0 (保持纹理映射不变)
    // 让position位置信息描述的是粒子的中心
    float particle_quad[] = {
        -0.5f,  0.5f,   0.0f, 1.0f,
         0.5f, -0.5f,   1.0f, 0.0f,
        -0.5f, -0.5f,   0.0f, 0.0f,

        -0.5f,  0.5f,   0.0f, 1.0f,
         0.5f,  0.5f,   1.0f, 1.0f,
         0.5f, -0.5f,   1.0f, 0.0f
    };

    this->m_VAO.bind();
    this->m_VBO.setBuffer(particle_quad, sizeof(particle_quad));

    VertexBufferLayout layout;
    layout.push<float>(4);
    this->m_VAO.addBuffer(m_VBO, layout);
}