#ifndef __ZSH_ENGINE_GAME_H
#define __ZSH_ENGINE_GAME_H

#include "glm/fwd.hpp"
#include <SpriteRenderer.h>

enum class GameState : int {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

class Game {
    using uint = unsigned int;
private:
    GameState                           State;
    bool                                Keys[1024];
    uint                                Width, Height;
    std::unique_ptr<SpriteRenderer>     renderer;
    std::vector<class GameLevel>        levels;
    uint                                level; 

private:
    std::unique_ptr<class GameObject>   Player;
    const glm::vec2                     PLAYER_SIZE{100, 20};
    const float                         PLAYER_VELOCITY{500.0f};

    
public:
    Game(uint width, uint height);
    ~Game();

    // 初始化游戏状态(加载所有的着色器/纹理/关卡)
    void Init();

    // 游戏循环
    void ProcessInput(float deltaTime);
    void Update(float deltaTime);
    void Render();

    // 按键设置
    void setKey(uint index, bool state);
    void setGameState(GameState state);
};

#endif