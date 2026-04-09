#ifndef __ZSH_ENGINE_GAME_H
#define __ZSH_ENGINE_GAME_H

#include "glm/fwd.hpp"
#include <SpriteRenderer.h>
#include <TextRenderer.h>
#include <Resource.h>
#include "SimpleFont.h"

enum class GameState : int {
    GAME_MENU,       // 主菜单
    LEVEL_SELECT,    // 关卡选择
    GAME_ACTIVE,     // 游戏进行中
    PAUSED,          // 暂停
    GAME_OVER,       // 游戏结束
    GAME_WIN         // 胜利
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
    std::unique_ptr<TextRenderer>           textRenderer; // 文本渲染器
    std::vector<class GameLevel>            levels;
    uint                                    level;

// 菜单状态
private:
    int                                     menuSelection;      // 当前菜单选项索引
    int                                     levelSelection;     // 关卡选择索引
    bool                                    enterPressed;       // Enter键按下状态
    bool                                    escPressed;         // ESC键按下状态
    bool                                    upPressed;          // 上键按下状态
    bool                                    downPressed;        // 下键按下状态
    bool                                    leftPressed;        // 左键按下状态
    bool                                    rightPressed;       // 右键按下状态
    bool                                    m_shouldQuit;       // 是否应该退出游戏
    std::unique_ptr<SimpleFont>             m_menuTitleFont;    // 菜单标题字体示例

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

    // 菜单和状态管理
    void StartGame();
    void PauseGame();
    void ResumeGame();
    void GoToMainMenu();
    void SelectLevel(uint levelIndex);
    void NextLevel();
    void CheckGameOver();
    void CheckWinCondition();

    // 菜单渲染
    void RenderMainMenu();
    void RenderLevelSelect();
    void RenderPauseMenu();
    void RenderGameOver();
    void RenderWinScreen();

    void DoCollisions();

    void ResetLevel();
    void ResetPlayer();

    bool shouldQuit() const { return m_shouldQuit; }
};

#endif