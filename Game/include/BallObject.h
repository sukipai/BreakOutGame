#ifndef ZSH_BREAKOUT_BALLOBJECT_H
#define ZSH_BREAKOUT_BALLOBJECT_H

#include "Shader.h"
#include <GameObject.h>

class BallObject : public GameObject {
private:
    float   Radius;
    bool    Stuck; // 初始时球被固定到板子上面

public:
    BallObject();

    BallObject(glm::vec2 pos, float radius, glm::vec2 velocity, const std::string& textureName);

    glm::vec2   Move(float deltaTime, uint window_width);
    void        Reset(glm::vec2 position, glm::vec2 velocity);
    float       radius() { return Radius; }

    bool isStucked() const { return Stuck; }
    void launch();
    void stop();
};



#endif