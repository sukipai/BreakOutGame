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

static const std::string SHADER_RESOURCES_PATH = "Game/Resources/Shaders/";
static const std::string TEXTURE_RESOURCES_PATH = "Game/Resources/Textures/";

static Direction VectorDirection(glm::vec2 target);


Game::Game(uint width, uint height) : State(GameState::GAME_ACTIVE), Width(width), Height(height), levels(), level(0){

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
}

void Game::ProcessInput(float deltaTime) {
    if (this->State == GameState::GAME_ACTIVE) {
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
    Ball->Move(deltaTime, this->Width);

    this->DoCollisions();

    // 更新粒子系统
    ParticleSystem::Update(deltaTime);

    if (Ball->position().y >= this->Height) {
        this->ResetLevel();
        this->ResetPlayer();
    }
}

void Game::Render() {
    if (this->State == GameState::GAME_ACTIVE) {
        auto background = Engine::ResourceManager::GetTexture("background");
        if (background.has_value()) {
            renderer->DrawSprite(background->get(), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), glm::vec3(1.0f));
        } else {
            APP_ERROR("Game::Render: 没有找到背景图片");
        }

        levels[level].Draw(*renderer.get());

        Player->Draw(*renderer.get());

        Ball->Draw(*renderer.get());

        // 渲染粒子系统
        ParticleSystem::Draw();
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