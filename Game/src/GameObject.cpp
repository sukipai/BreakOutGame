#include <GameObject.h>
#include <Graphics/Texture.h>
#include <Graphics/SpriteRenderer.h>
#include <Resource/Resource.h>
#include <Core/log.h>

GameObject::GameObject() 
    : m_position(0.0f), m_size(1.0f), m_velocity(0.0f), m_color(1.0f), 
    m_rotation(0.0f), m_isSolid(false), m_destroyed(false), 
    m_textureName(""), m_sprite(nullptr) { }

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, const std::string& textureName, 
        glm::vec3 color, glm::vec2 velocity)
        : m_position(pos), m_size(size), m_velocity(velocity), m_color(color), 
          m_rotation(0.0f), m_isSolid(false), m_destroyed(false), 
          m_textureName(textureName), m_sprite(nullptr)
{ 
    if (textureName.empty()) {
        return;
    }

    auto texture = Engine::ResourceManager::GetTexture(textureName);
    if (!texture.has_value()) {
        APP_ERROR("GameObject: 没有找到对应的纹理文件{}", textureName);
        return;
    }

    m_sprite = &texture->get();
}

void GameObject::Draw(SpriteRenderer& renderer) {
    renderer.DrawSprite(*m_sprite, m_position, m_size, m_color, m_rotation);
}   