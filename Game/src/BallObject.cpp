#include <BallObject.h>

BallObject::BallObject() : GameObject() { }

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, const std::string& textureName)
    :   GameObject(pos, glm::vec2(radius * 2, radius * 2), textureName, glm::vec3(1.0f), velocity), 
        Radius(radius), Stuck(true) 
{
    // 初始化小球专属的尾迹配方
    m_TrailParticleProps.ColorBegin = { 0.5f, 0.8f, 1.0f, 1.0f }; 
    m_TrailParticleProps.ColorEnd   = { 0.0f, 0.0f, 0.0f, 0.0f };
    m_TrailParticleProps.SizeBegin  = radius; 
    m_TrailParticleProps.SizeEnd    = 0.0f;
    m_TrailParticleProps.SizeVariation = 2.0f;
    m_TrailParticleProps.LifeTime   = 0.3f; 
    m_TrailParticleProps.VelocityVariation = { 10.0f, 10.0f };
}

glm::vec2 BallObject::Move(float deltaTime, uint window_width) {
    if (!this->Stuck) {
        this->m_position += this->m_velocity * deltaTime;

        if (this->m_position.x <= 0.0f) {
            this->m_velocity.x = -this->m_velocity.x;
            this->m_position.x = 0.0f;
        }
        else if (this->m_position.x + this->m_size.x >= window_width) {
            this->m_velocity.x = -this->m_velocity.x;
            this->m_position.x = window_width - this->m_size.x;
        }

        if (this->m_position.y <= 0.0f) {
            this->m_velocity.y = -this->m_velocity.y;
            this->m_position.y = 0.0f;
        }

        for (int i = 0; i < 2; ++i) {
            m_TrailParticleProps.Position = this->m_position + this->m_size / 2.0f;
            m_TrailParticleProps.Velocity = -this->m_velocity * 0.5f;
            ParticleSystem::Emit(m_TrailParticleProps);
        }

    }
    return this->m_position;
}

void BallObject::Reset(glm::vec2 position, glm::vec2 velocity) {
    this->m_position = position;
    this->m_velocity = velocity;
    this->Stuck = true;
}

void BallObject::launch() {
    this->Stuck = false;
}

void BallObject::stop() {
    this->Stuck = true;
}