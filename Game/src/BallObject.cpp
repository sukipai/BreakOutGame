#include <BallObject.h>

BallObject::BallObject() : GameObject() { }

BallObject::BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, const std::string& textureName)
    :   GameObject(pos, glm::vec2(radius * 2, radius * 2), textureName, glm::vec3(1.0f), velocity), 
        Radius(radius), Stuck(true) 
{

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