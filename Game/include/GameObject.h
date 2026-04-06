#ifndef ZSH_BREAKOUT_GAMEOBJECT_H
#define ZSH_BREAKOUT_GAMEOBJECT_H

#include <glm/glm.hpp>
#include <string>

class GameObject {
private:
    glm::vec2       m_position, m_size, m_velocity;
    glm::vec3       m_color;
    float           m_rotation;
    bool            m_isSolid;
    bool            m_destroyed;
    class Texture*  m_sprite;
    std::string     m_textureName;

public:
    GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, const std::string& textureName = "", glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.0f, 0.0f));

    void setSolid(bool isSolid);

    bool destroyed() { return m_destroyed; }
    bool isSolid() { return m_isSolid; }

    virtual void Draw(class SpriteRenderer& renderer);
};



#endif