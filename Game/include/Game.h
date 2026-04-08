#ifndef __ZSH_ENGINE_GAME_H
#define __ZSH_ENGINE_GAME_H

#include "ParticleSystem.h"
#include "glm/fwd.hpp"
#include <SpriteRenderer.h>

enum class GameState : int {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

enum class Direction : uint {
    UP = 0,
    RIGHT,
    DOWN,
    LEFT,

    NONE
};

class Game {
    using uint = unsigned int;
// 主窗口和辅助设置
private:
    GameState                               State;
    bool                                    Keys[1024]; // 键盘
    uint                                    Width, Height;
    std::unique_ptr<SpriteRenderer>         renderer;   // 渲染器
    std::vector<class GameLevel>            levels;
    uint                                    level;

// 玩家
private:
    std::unique_ptr<class GameObject>   Player;
    const glm::vec2                     PLAYER_SIZE{100, 20};
    const float                         PLAYER_VELOCITY{500.0f};

// 球
private:
    const glm::vec2 INITIAL_BALL_VELOCITY{100.0f, -350.0f};
    const float     BALL_RADIUS{12.5f};
    std::unique_ptr<class BallObject> Ball;

    bool CheckCollision(GameObject& one, GameObject& two);
    std::tuple<bool, Direction, glm::vec2> CheckCollision(BallObject& one, GameObject& two);

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

    void DoCollisions();

    void ResetLevel();
    void ResetPlayer();
};

#endif