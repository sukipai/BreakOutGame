#include <Game.h>
#include <Resource.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Core/log.h>
#include <GameLevel.h>
#include <GameObject.h>
#include <BallObject.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <ParticleSystem.h>
#include "SimpleFont.h"

static const std::string SHADER_RESOURCES_PATH = "Game/Resources/Shaders/";
static const std::string TEXTURE_RESOURCES_PATH = "Game/Resources/Textures/";

static Direction VectorDirection(glm::vec2 target);


Game::Game(uint width, uint height) : State(GameState::GAME_MENU), Width(width), Height(height), levels(), level(0),
    menuSelection(0), levelSelection(0), enterPressed(false), escPressed(false), upPressed(false), downPressed(false), leftPressed(false), rightPressed(false) {
    m_shouldQuit = false;
}

void Game::Init() {
    auto spriteShaderOpt = Engine::ResourceManager::LoadShader("sprite", SHADER_RESOURCES_PATH + "sprite.vert", SHADER_RESOURCES_PATH + "sprite.frag");
    Engine::ResourceManager::LoadTexture("face", TEXTURE_RESOURCES_PATH + "awesomeface.png", true);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    if (!spriteShaderOpt.has_value()) {
        APP_ERROR("Game: 无法加载着色器 sprite");
        exit(0);
    }

    Shader& spriteShader = spriteShaderOpt->get();

    spriteShader.use();
    spriteShader.setInt("image", 0);
    spriteShader.setMatrix4f("projection", projection);

    renderer = std::make_unique<SpriteRenderer>(spriteShader);    

    Engine::ResourceManager::LoadTexture("background", TEXTURE_RESOURCES_PATH + "background.jpg", false);
    Engine::ResourceManager::LoadTexture("block", TEXTURE_RESOURCES_PATH + "block.png");
    Engine::ResourceManager::LoadTexture("block_solid", TEXTURE_RESOURCES_PATH + "block_solid.png");
    Engine::ResourceManager::LoadTexture("paddle", TEXTURE_RESOURCES_PATH + "paddle.png", true);  

    // 读取关卡信息
    GameLevel one;
    one.Load("Game/Resources/Levels/one.lvl", this->Width, this->Height * 0.5);
    GameLevel two;
    two.Load("Game/Resources/Levels/two.lvl", this->Width, this->Height * 0.5);
    GameLevel three;
    three.Load("Game/Resources/Levels/three.lvl", this->Width, this->Height * 0.5);
    GameLevel four;
    four.Load("Game/Resources/Levels/four.lvl", this->Width, this->Height * 0.5);

    levels.push_back(one);
    levels.push_back(two);
    levels.push_back(three);
    levels.push_back(four);
    level = 0;

    // 设置玩家信息
    glm::vec2 playerPos = glm::vec2(
        this->Width / 2.0f - PLAYER_SIZE.x / 2, 
        this->Height - PLAYER_SIZE.y
    );

    Player = std::make_unique<GameObject>(playerPos, PLAYER_SIZE, "paddle", glm::vec3(1.0f), glm::vec2(PLAYER_VELOCITY, 0.0f));

    // 设置球的信息
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);

    Ball = std::make_unique<BallObject>(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, "face");

    // 粒子着色器
    auto particleShaderOpt = Engine::ResourceManager::LoadShader("particle", SHADER_RESOURCES_PATH + "particle.vert", SHADER_RESOURCES_PATH + "particle.frag");

    if (!particleShaderOpt.has_value()) {
        ENGINE_CORE_ERROR("GAME: 无法加载着色器: sprite");
        exit(0);
    }

    auto & particleShader = particleShaderOpt->get();

    particleShader.use();
    particleShader.setInt("sprite", 0);
    particleShader.setMatrix4f("projection", projection);

    // 粒子纹理
    auto particleTextureOpt = Engine::ResourceManager::LoadTexture("particle", TEXTURE_RESOURCES_PATH + "particle.png", true);

    if (!particleTextureOpt.has_value()) {
        APP_ERROR("GAME: 无法找到纹: particle");
    }

    auto& particleTexture = particleTextureOpt->get();

    ParticleSystem::Initialize(particleShader, particleTexture);

    // 文本渲染初始化
    auto textShaderOpt = Engine::ResourceManager::LoadShader("text",
        SHADER_RESOURCES_PATH + "text.vert",
        SHADER_RESOURCES_PATH + "text.frag");

    if (!textShaderOpt.has_value()) {
        APP_ERROR("Game: 无法加载文本着色器");
        exit(0);
    }

    Shader& textShader = textShaderOpt->get();
    textShader.use();
    textShader.setInt("text", 0);
    textShader.setMatrix4f("projection", projection);

    // 加载字体
    auto fontOpt = Engine::ResourceManager::LoadFont("main",
        "Game/Resources/Fonts/Arial Unicode.ttf", 48);

    if (!fontOpt.has_value()) {
        APP_ERROR("Game: 无法加载字体");
        exit(0);
    }

    textRenderer = std::make_unique<TextRenderer>(textShader, projection);

    // 初始化菜单标题字体示例
    Font& font = fontOpt->get();
    m_menuTitleFont = std::make_unique<SimpleFont>(
        *textRenderer, font, "Breakout Menu", glm::vec3(1.0f, 0.8f, 0.2f), 1.2f);
}

void Game::ProcessInput(float deltaTime) {
    switch (this->State) {
        case GameState::GAME_ACTIVE:
        {
            glm::vec2 velocity = Player->velocity() * deltaTime;
            glm::vec2 player_position = Player->position();
            glm::vec2 player_size = Player->size();

            if (this->Keys[GLFW_KEY_A]) {
                player_position -= velocity;
                if (player_position.x < 0) player_position.x = 0;
                else if (Ball->isStucked()) Ball->setPosition(Ball->position() - velocity);
            }

            if (this->Keys[GLFW_KEY_D]) {
                player_position += velocity;
                if (player_position.x + player_size.x > this->Width) player_position.x = this->Width - player_size.x;
                else if (Ball->isStucked()) Ball->setPosition(Ball->position() + velocity);
            }

            Player->setPosition(player_position);

            if (this->Keys[GLFW_KEY_SPACE]) {
                Ball->launch();
            }

            // ESC键暂停游戏
            if (this->Keys[GLFW_KEY_ESCAPE] && !escPressed) {
                escPressed = true;
                PauseGame();
            } else if (!this->Keys[GLFW_KEY_ESCAPE]) {
                escPressed = false;
            }
            break;
        }

        case GameState::GAME_MENU:
        {
            // 上下导航
            if (this->Keys[GLFW_KEY_UP] && !upPressed) {
                upPressed = true;
                menuSelection = (menuSelection - 1 + 3) % 3; // 3个菜单选项
            } else if (!this->Keys[GLFW_KEY_UP]) {
                upPressed = false;
            }

            if (this->Keys[GLFW_KEY_DOWN] && !downPressed) {
                downPressed = true;
                menuSelection = (menuSelection + 1) % 3;
            } else if (!this->Keys[GLFW_KEY_DOWN]) {
                downPressed = false;
            }

            // Enter键选择
            if (this->Keys[GLFW_KEY_ENTER] && !enterPressed) {
                enterPressed = true;
                switch (menuSelection) {
                    case 0: // 开始游戏
                        SelectLevel(0); // 默认第一关
                        break;
                    case 1: // 选择关卡
                        setGameState(GameState::LEVEL_SELECT);
                        levelSelection = 0;
                        break;
                    case 2: // 退出游戏
                        m_shouldQuit = true;
                        break;
                }
            } else if (!this->Keys[GLFW_KEY_ENTER]) {
                enterPressed = false;
            }

            // ESC键退出游戏
            if (this->Keys[GLFW_KEY_ESCAPE] && !escPressed) {
                escPressed = true;
                m_shouldQuit = true;
            } else if (!this->Keys[GLFW_KEY_ESCAPE]) {
                escPressed = false;
            }
            break;
        }

        case GameState::LEVEL_SELECT:
        {
            // 方向键导航
            if (this->Keys[GLFW_KEY_LEFT] && !leftPressed) {
                leftPressed = true;
                levelSelection = (levelSelection - 1 + 5) % 5; // 4个关卡 + 返回按钮
            } else if (!this->Keys[GLFW_KEY_LEFT]) {
                leftPressed = false;
            }

            if (this->Keys[GLFW_KEY_RIGHT] && !rightPressed) {
                rightPressed = true;
                levelSelection = (levelSelection + 1) % 5;
            } else if (!this->Keys[GLFW_KEY_RIGHT]) {
                rightPressed = false;
            }

            if (this->Keys[GLFW_KEY_UP] && !upPressed) {
                upPressed = true;
                levelSelection = (levelSelection - 2 + 5) % 5;
            } else if (!this->Keys[GLFW_KEY_UP]) {
                upPressed = false;
            }

            if (this->Keys[GLFW_KEY_DOWN] && !downPressed) {
                downPressed = true;
                levelSelection = (levelSelection + 2) % 5;
            } else if (!this->Keys[GLFW_KEY_DOWN]) {
                downPressed = false;
            }

            // Enter键选择
            if (this->Keys[GLFW_KEY_ENTER] && !enterPressed) {
                enterPressed = true;
                if (levelSelection < 4) {
                    SelectLevel(levelSelection);
                } else {
                    // 返回主菜单
                    setGameState(GameState::GAME_MENU);
                    menuSelection = 1; // 高亮"选择关卡"选项
                }
            } else if (!this->Keys[GLFW_KEY_ENTER]) {
                enterPressed = false;
            }

            // ESC键返回主菜单
            if (this->Keys[GLFW_KEY_ESCAPE] && !escPressed) {
                escPressed = true;
                setGameState(GameState::GAME_MENU);
                menuSelection = 1;
            } else if (!this->Keys[GLFW_KEY_ESCAPE]) {
                escPressed = false;
            }
            break;
        }

        case GameState::PAUSED:
        {
            // 上下导航
            if (this->Keys[GLFW_KEY_UP] && !upPressed) {
                upPressed = true;
                menuSelection = (menuSelection - 1 + 4) % 4; // 4个暂停菜单选项
            } else if (!this->Keys[GLFW_KEY_UP]) {
                upPressed = false;
            }

            if (this->Keys[GLFW_KEY_DOWN] && !downPressed) {
                downPressed = true;
                menuSelection = (menuSelection + 1) % 4;
            } else if (!this->Keys[GLFW_KEY_DOWN]) {
                downPressed = false;
            }

            // Enter键选择
            if (this->Keys[GLFW_KEY_ENTER] && !enterPressed) {
                enterPressed = true;
                switch (menuSelection) {
                    case 0: // 继续游戏
                        ResumeGame();
                        break;
                    case 1: // 重新开始
                        ResetLevel();
                        ResetPlayer();
                        ResumeGame();
                        break;
                    case 2: // 返回主菜单
                        GoToMainMenu();
                        break;
                    case 3: // 退出游戏
                        m_shouldQuit = true;
                        break;
                }
            } else if (!this->Keys[GLFW_KEY_ENTER]) {
                enterPressed = false;
            }

            // ESC键继续游戏
            if (this->Keys[GLFW_KEY_ESCAPE] && !escPressed) {
                escPressed = true;
                ResumeGame();
            } else if (!this->Keys[GLFW_KEY_ESCAPE]) {
                escPressed = false;
            }
            break;
        }

        case GameState::GAME_OVER:
        case GameState::GAME_WIN:
        {
            // 上下导航
            if (this->Keys[GLFW_KEY_UP] && !upPressed) {
                upPressed = true;
                menuSelection = (menuSelection - 1 + 2) % 2; // 2个选项
            } else if (!this->Keys[GLFW_KEY_UP]) {
                upPressed = false;
            }

            if (this->Keys[GLFW_KEY_DOWN] && !downPressed) {
                downPressed = true;
                menuSelection = (menuSelection + 1) % 2;
            } else if (!this->Keys[GLFW_KEY_DOWN]) {
                downPressed = false;
            }

            // Enter键选择
            if (this->Keys[GLFW_KEY_ENTER] && !enterPressed) {
                enterPressed = true;
                if (this->State == GameState::GAME_OVER) {
                    switch (menuSelection) {
                        case 0: // 重新开始
                            ResetLevel();
                            ResetPlayer();
                            StartGame();
                            break;
                        case 1: // 返回主菜单
                            GoToMainMenu();
                            break;
                    }
                } else { // GAME_WIN
                    switch (menuSelection) {
                        case 0: // 下一关卡
                            NextLevel();
                            break;
                        case 1: // 返回主菜单
                            GoToMainMenu();
                            break;
                    }
                }
            } else if (!this->Keys[GLFW_KEY_ENTER]) {
                enterPressed = false;
            }

            // ESC键返回主菜单
            if (this->Keys[GLFW_KEY_ESCAPE] && !escPressed) {
                escPressed = true;
                GoToMainMenu();
            } else if (!this->Keys[GLFW_KEY_ESCAPE]) {
                escPressed = false;
            }
            break;
        }
    }
}

bool Game::CheckCollision(GameObject& one, GameObject& two) {
    auto onePosition = one.position(), oneSize = one.size();
    auto twoPosition = two.position(), twoSize = two.size();

    bool collisionX =   onePosition.x + oneSize.x >= twoPosition.x &&
                        twoPosition.x + twoSize.x >= onePosition.x;
                        
    bool collisionY =   onePosition.y + oneSize.y >= twoPosition.y &&
                        twoPosition.y + twoSize.y >= onePosition.y;

    return collisionX && collisionY;
}

// 圆和AABB碰撞
// 一维情况在X轴上有一个点表示圆心，一个线段范围是[minX, maxX]。那么点到这个范围的最近距离就是三种情况
// + 在线段左边，就是minX，在线段右边就就是maxX，在线段中间就是点自己。
// 升级到二维，就是在X和Y上完全独立的两个一维线段。
// 其实就是将圆心的x，clamp在[rect.left, rect.right]，圆心的y，clamp在[rect.top, rect.bottom]上。
// 这里先减去aabb_half_extends免去了求其他坐标由于减去了，所以限制的范围就变了。
// 也就是将坐标的原点限制在了矩形的中心位置clamp的范围就到了[-aabb_half, aabb_half]
std::tuple<bool, Direction, glm::vec2> Game::CheckCollision(BallObject& one, GameObject& two) {
    glm::vec2 center(one.position() + one.radius());

    glm::vec2 aabb_half_extends(two.size() * 0.5f);
    glm::vec2 aabb_center = two.position() + aabb_half_extends;

    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extends, aabb_half_extends);

    glm::vec2 closest = aabb_center + clamped;

    difference = closest - center;

    if (glm::length(difference) <= one.radius()) {
        return { true, VectorDirection(difference), difference };
    }
    else {
        return { false, Direction::UP, glm::vec2(0.0f) };
    }
}

void Game::DoCollisions() {
    // 检测球和砖块的碰撞
    for (GameObject& box : levels[level].bricks()) {
        if (box.isDestroyed()) {
            continue;
        }

        auto [collisioned, direction, difference_vector] = CheckCollision(*Ball.get(), box);
    
        if (!collisioned) {
            continue;
        }

        if (!box.isSolid()) {
            box.destroy();
        }
  
        auto ballVelocity = Ball->velocity();
        if (direction == Direction::LEFT || direction == Direction::RIGHT) {
            ballVelocity.x = -ballVelocity.x;

            float penetration = Ball->radius() - std::abs(difference_vector.x);
            if (direction == Direction::LEFT) {
                Ball->moveBy(penetration, 0.0f);
            }
            else {
                Ball->moveBy(-penetration, 0.0f);
            }
        }
        else {
            ballVelocity.y = -ballVelocity.y;

            float penetration = Ball->radius() - std::abs(difference_vector.y);
            if(direction == Direction::UP) {
                Ball->moveBy(0.0f, -penetration);
            }
            else {
                Ball->moveBy(0.0f, penetration);
            }
        }

        Ball->setVelocity(ballVelocity);
    }

    // 检测玩家和球的碰撞
    do {
        if (Ball->isStucked()) {
            break;
        }

        auto [collisioned, direction, difference_vector] = CheckCollision(*Ball.get(), *Player.get());

        if (!collisioned) {
            break;
        }

        float centerBoard = Player->position().x + Player->size().x / 2.0f;
        float distance = (Ball->position().x + Ball->radius()) - centerBoard;
        float percentage = distance / (Player->size().x / 2);

        float strength = 2.0f;
        glm::vec2 oldVelocity = Ball->velocity();
        glm::vec2 movementDirection;
        movementDirection.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        movementDirection.y = -1 * abs(oldVelocity.y);

        movementDirection = glm::normalize(movementDirection);

        Ball->setVelocity(glm::vec2(movementDirection * glm::length(oldVelocity)));
    } while(false);

}

void Game::Update(float deltaTime) {
    if (this->State != GameState::GAME_ACTIVE) {
        return; // 只在游戏进行中更新
    }

    Ball->Move(deltaTime, this->Width);

    this->DoCollisions();

    // 更新粒子系统
    ParticleSystem::Update(deltaTime);

    // 检查游戏结束条件
    if (Ball->position().y >= this->Height) {
        this->State = GameState::GAME_OVER;
        menuSelection = 0;
        return;
    }

    // 检查胜利条件
    CheckWinCondition();
}

void Game::Render() {
    switch (this->State) {
        case GameState::GAME_ACTIVE:
        {
            auto background = Engine::ResourceManager::GetTexture("background");
            if (background.has_value()) {
                renderer->DrawSprite(background->get(), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), glm::vec3(1.0f, 1.0f, 1.0f));
            } else {
                APP_ERROR("Game::Render: 没有找到背景图片");
            }

            levels[level].Draw(*renderer.get());

            Player->Draw(*renderer.get());

            Ball->Draw(*renderer.get());

            // 渲染粒子系统
            ParticleSystem::Draw();
            break;
        }

        case GameState::GAME_MENU:
            RenderMainMenu();
            break;

        case GameState::LEVEL_SELECT:
            RenderLevelSelect();
            break;

        case GameState::PAUSED:
            // 先渲染游戏画面，再渲染暂停菜单
            {
                auto background = Engine::ResourceManager::GetTexture("background");
                if (background.has_value()) {
                    renderer->DrawSprite(background->get(), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), glm::vec3(1.0f, 1.0f, 1.0f));
                }

                levels[level].Draw(*renderer.get());
                Player->Draw(*renderer.get());
                Ball->Draw(*renderer.get());
                ParticleSystem::Draw();
            }
            RenderPauseMenu();
            break;

        case GameState::GAME_OVER:
            RenderGameOver();
            break;

        case GameState::GAME_WIN:
            RenderWinScreen();
            break;
    }
}

void Game::setKey(uint index, bool state) {
    this->Keys[index] = state;
}

void Game::setGameState(GameState state) {
    this->State = state;
}

void Game::ResetLevel() {
    if (this->level == 0) {
        this->levels[0].Load("Game/Resources/Levels/one.lvl", this->Width, this->Height * 0.5f);
    }
    else if (this->level == 1) {
        this->levels[1].Load("Game/Resources/Levels/two.lvl", this->Width, this->Height * 0.5f);
    }
    else if (this->level == 2) {
        this->levels[2].Load("Game/Resources/Levels/three.lvl", this->Width, this->Height * 0.5f);
    }
    else if (this->level == 3) {
        this->levels[3].Load("Game/Resources/Levels/four.lvl", this->Width, this->Height * 0.5f);
    }
}

void Game::ResetPlayer() {
    Player->setSize(PLAYER_SIZE);
    Player->setPosition(glm::vec2(this->Width / 2.0 - PLAYER_SIZE.x / 2, this->Height - PLAYER_SIZE.y));
    Ball->Reset(Player->position() + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -(BALL_RADIUS * 2)), INITIAL_BALL_VELOCITY);
}

Game::~Game() {
    
}

static Direction VectorDirection(glm::vec2 target) {
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),  // 上
        glm::vec2(1.0f, 0.0f),  // 右
        glm::vec2(0.0f, -1.0f), // 下
        glm::vec2(-1.0f, 0.0f)  // 左
    };

    float max = 0.0f;
    Direction best_match = Direction::NONE;

    for (uint i = 0; i < 4; i++) {
        float dot_product = glm::dot(glm::normalize(target), compass[i]);

        if (dot_product > max) {
            max = dot_product;
            best_match = static_cast<Direction>(i);
        }
    }

    return best_match;
}

// ==================== 状态转换方法 ====================

void Game::StartGame() {
    this->State = GameState::GAME_ACTIVE;
    menuSelection = 0;
}

void Game::PauseGame() {
    this->State = GameState::PAUSED;
    menuSelection = 0;
}

void Game::ResumeGame() {
    this->State = GameState::GAME_ACTIVE;
}

void Game::GoToMainMenu() {
    this->State = GameState::GAME_MENU;
    menuSelection = 0;
    levelSelection = 0;
}

void Game::SelectLevel(uint levelIndex) {
    if (levelIndex < levels.size()) {
        this->level = levelIndex;
        ResetLevel();
        ResetPlayer();
        StartGame();
    }
}

void Game::NextLevel() {
    if (level + 1 < levels.size()) {
        level++;
        ResetLevel();
        ResetPlayer();
        StartGame();
    } else {
        // 所有关卡完成，返回主菜单
        GoToMainMenu();
    }
}

void Game::CheckGameOver() {
    // 在Update中检查球是否掉落
    // 这个方法在Update中调用
}

void Game::CheckWinCondition() {
    // 检查当前关卡是否所有砖块都被摧毁
    bool levelCompleted = true;
    for (const auto& brick : levels[level].bricks()) {
        if (!brick.isDestroyed() && !brick.isSolid()) {
            levelCompleted = false;
            break;
        }
    }

    if (levelCompleted) {
        this->State = GameState::GAME_WIN;
        menuSelection = 0;
    }
}

// ==================== 菜单渲染方法 ====================

void Game::RenderMainMenu() {
    // 绘制菜单背景
    auto background = Engine::ResourceManager::GetTexture("background");
    if (background.has_value()) {
        renderer->DrawSprite(background->get(), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), glm::vec3(1.0f, 1.0f, 1.0f));
    }

    // 检查文本渲染器是否可用
    if (!textRenderer) {
        APP_ERROR("Game::RenderMainMenu: 文本渲染器未初始化");
        return;
    }

    auto fontOpt = Engine::ResourceManager::GetFont("main");
    if (!fontOpt.has_value()) {
        APP_ERROR("Game::RenderMainMenu: 未找到字体");
        return;
    }

    Font& font = fontOpt->get();

    // 使用SimpleFont示例渲染菜单标题
    if (m_menuTitleFont) {
        std::string title = "BREAKOUT";
        m_menuTitleFont->setText(title);
        float titleWidth = m_menuTitleFont->getTextWidth();
        float titleX = this->Width / 2.0f - titleWidth / 2.0f;
        float titleY = this->Height / 2.0f - 200.0f;
        m_menuTitleFont->draw(glm::vec2(titleX, titleY));
    }

    // 三个菜单选项 (使用SimpleFont局部对象)
    const char* menuItems[] = {"Start Game", "Select Level", "Quit"};
    float startY = this->Height / 2.0f - 100.0f;
    float spacing = 60.0f;

    for (int i = 0; i < 3; i++) {
        // 高亮选中的选项
        glm::vec3 color = (i == menuSelection) ? glm::vec3(1.0f, 0.5f, 0.0f) : glm::vec3(0.8f, 0.8f, 0.8f);

        // 使用SimpleFont局部对象
        SimpleFont menuItemFont(*textRenderer, font, menuItems[i], color, 1.0f);

        // 计算文本宽度以居中
        float textWidth = menuItemFont.getTextWidth();
        float x = this->Width / 2.0f - textWidth / 2.0f;
        float y = startY + i * spacing;

        // 使用SimpleFont的draw方法渲染
        menuItemFont.draw(glm::vec2(x, y));
    }
}

void Game::RenderLevelSelect() {
    // 绘制背景
    auto background = Engine::ResourceManager::GetTexture("background");
    if (background.has_value()) {
        renderer->DrawSprite(background->get(), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), glm::vec3(1.0f, 1.0f, 1.0f));
    }

    // 检查文本渲染器是否可用
    if (!textRenderer) {
        APP_ERROR("Game::RenderLevelSelect: 文本渲染器未初始化");
        return;
    }

    auto fontOpt = Engine::ResourceManager::GetFont("main");
    if (!fontOpt.has_value()) {
        APP_ERROR("Game::RenderLevelSelect: 未找到字体");
        return;
    }

    Font& font = fontOpt->get();

    // 添加关卡选择标题（使用SimpleFont）
    SimpleFont titleFont(*textRenderer, font, "Select Level", glm::vec3(0.9f, 0.9f, 0.3f), 1.5f);
    float titleWidth = titleFont.getTextWidth();
    float titleX = this->Width / 2.0f - titleWidth / 2.0f;
    float titleY = this->Height / 2.0f - 200.0f;
    titleFont.draw(glm::vec2(titleX, titleY));

    // 4个关卡按钮位置
    float startX = this->Width / 2.0f - 200.0f;
    float startY = this->Height / 2.0f - 100.0f;
    float spacing = 100.0f;

    // 关卡编号
    const char* levelNumbers[] = {"1", "2", "3", "4"};

    for (int i = 0; i < 4; i++) {
        int row = i / 2;
        int col = i % 2;

        float x = startX + col * spacing;
        float y = startY + row * spacing;

        glm::vec3 color = (i == levelSelection) ? glm::vec3(1.0f, 0.5f, 0.0f) : glm::vec3(0.8f, 0.8f, 0.8f);

        // 使用SimpleFont局部对象
        SimpleFont levelFont(*textRenderer, font, levelNumbers[i], color, 1.0f);

        // 计算文本居中
        float textWidth = levelFont.getTextWidth();
        float textX = x - textWidth / 2.0f + spacing / 2.0f;
        float textY = y;

        // 使用SimpleFont的draw方法渲染
        levelFont.draw(glm::vec2(textX, textY));
    }

    // 返回按钮（使用SimpleFont）
    float backButtonY = startY + 2 * spacing + 40.0f;
    std::string backText = "Back";
    glm::vec3 backColor = (levelSelection == 4) ? glm::vec3(1.0f, 0.5f, 0.0f) : glm::vec3(0.8f, 0.8f, 0.8f);
    SimpleFont backButtonFont(*textRenderer, font, backText, backColor, 1.0f);
    float backTextWidth = backButtonFont.getTextWidth();
    float backButtonX = this->Width / 2.0f - backTextWidth / 2.0f;

    backButtonFont.draw(glm::vec2(backButtonX, backButtonY));
}

void Game::RenderPauseMenu() {
    // 检查文本渲染器是否可用
    if (!textRenderer) {
        APP_ERROR("Game::RenderPauseMenu: 文本渲染器未初始化");
        return;
    }

    auto fontOpt = Engine::ResourceManager::GetFont("main");
    if (!fontOpt.has_value()) {
        APP_ERROR("Game::RenderPauseMenu: 未找到字体");
        return;
    }

    Font& font = fontOpt->get();

    // 半透明覆盖层（使用黑色块纹理，颜色为深灰色）
    auto buttonTexture = Engine::ResourceManager::GetTexture("block");
    if (buttonTexture.has_value()) {
        renderer->DrawSprite(buttonTexture->get(), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), glm::vec3(0.0f, 0.0f, 0.0f));
    }

    const char* menuItems[] = {"Resume", "Restart", "Main Menu", "Quit"};
    float startY = this->Height / 2.0f - 120.0f;
    float spacing = 60.0f;

    for (int i = 0; i < 4; i++) {
        glm::vec3 color = (i == menuSelection) ? glm::vec3(1.0f, 0.5f, 0.0f) : glm::vec3(0.8f, 0.8f, 0.8f);

        // 计算文本宽度以居中
        std::string text = menuItems[i];
        float textWidth = font.getTextWidth(text, 1.0f);
        float x = this->Width / 2.0f - textWidth / 2.0f;
        float y = startY + i * spacing;

        textRenderer->renderText(font, text, glm::vec2(x, y), 1.0f, color);
    }
}

void Game::RenderGameOver() {
    // 背景
    auto background = Engine::ResourceManager::GetTexture("background");
    if (background.has_value()) {
        renderer->DrawSprite(background->get(), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), glm::vec3(1.0f, 1.0f, 1.0f));
    }

    // 检查文本渲染器是否可用
    if (!textRenderer) {
        APP_ERROR("Game::RenderGameOver: 文本渲染器未初始化");
        return;
    }

    auto fontOpt = Engine::ResourceManager::GetFont("main");
    if (!fontOpt.has_value()) {
        APP_ERROR("Game::RenderGameOver: 未找到字体");
        return;
    }

    Font& font = fontOpt->get();

    // 半透明红色覆盖层
    auto buttonTexture = Engine::ResourceManager::GetTexture("block");
    if (buttonTexture.has_value()) {
        renderer->DrawSprite(buttonTexture->get(), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    // 游戏结束标题
    std::string title = "Game Over";
    float titleWidth = font.getTextWidth(title, 1.5f);
    float titleX = this->Width / 2.0f - titleWidth / 2.0f;
    float titleY = this->Height / 2.0f - 100.0f;
    textRenderer->renderText(font, title, glm::vec2(titleX, titleY), 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));

    // 选项按钮
    const char* options[] = {"Restart", "Main Menu"};
    float startY = this->Height / 2.0f;
    float spacing = 60.0f;

    for (int i = 0; i < 2; i++) {
        glm::vec3 color = (i == menuSelection) ? glm::vec3(1.0f, 0.5f, 0.0f) : glm::vec3(0.8f, 0.8f, 0.8f);
        std::string text = options[i];
        float textWidth = font.getTextWidth(text, 1.0f);
        float x = this->Width / 2.0f - textWidth / 2.0f;
        float y = startY + i * spacing;
        textRenderer->renderText(font, text, glm::vec2(x, y), 1.0f, color);
    }
}

void Game::RenderWinScreen() {
    // 背景
    auto background = Engine::ResourceManager::GetTexture("background");
    if (background.has_value()) {
        renderer->DrawSprite(background->get(), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), glm::vec3(1.0f, 1.0f, 1.0f));
    }

    // 检查文本渲染器是否可用
    if (!textRenderer) {
        APP_ERROR("Game::RenderWinScreen: 文本渲染器未初始化");
        return;
    }

    auto fontOpt = Engine::ResourceManager::GetFont("main");
    if (!fontOpt.has_value()) {
        APP_ERROR("Game::RenderWinScreen: 未找到字体");
        return;
    }

    Font& font = fontOpt->get();

    // 半透明绿色覆盖层
    auto buttonTexture = Engine::ResourceManager::GetTexture("block");
    if (buttonTexture.has_value()) {
        renderer->DrawSprite(buttonTexture->get(), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    // 胜利标题
    std::string title = "You Win!";
    float titleWidth = font.getTextWidth(title, 1.5f);
    float titleX = this->Width / 2.0f - titleWidth / 2.0f;
    float titleY = this->Height / 2.0f - 100.0f;
    textRenderer->renderText(font, title, glm::vec2(titleX, titleY), 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));

    // 选项按钮
    const char* options[] = {"Next Level", "Main Menu"};
    float startY = this->Height / 2.0f;
    float spacing = 60.0f;

    for (int i = 0; i < 2; i++) {
        glm::vec3 color = (i == menuSelection) ? glm::vec3(1.0f, 0.5f, 0.0f) : glm::vec3(0.8f, 0.8f, 0.8f);
        std::string text = options[i];
        float textWidth = font.getTextWidth(text, 1.0f);
        float x = this->Width / 2.0f - textWidth / 2.0f;
        float y = startY + i * spacing;
        textRenderer->renderText(font, text, glm::vec2(x, y), 1.0f, color);
    }
}