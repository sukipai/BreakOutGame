#include <Game.h>
#include <Resource.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Core/log.h>
#include <GameLevel.h>
#include <GameObject.h>

static const std::string SHADER_RESOURCES_PATH = "Game/Resources/Shaders/";
static const std::string TEXTURE_RESOURCES_PATH = "Game/Resources/Textures/";

Game::Game(uint width, uint height) : State(GameState::GAME_ACTIVE), Width(width), Height(height), levels(), level(0){

}

void Game::Init() {
    auto spriteShaderOpt = Engine::ResourceManager::LoadShader("sprite", SHADER_RESOURCES_PATH + "sprite.vert", SHADER_RESOURCES_PATH + "sprite.frag");
    Engine::ResourceManager::LoadTexture("face", TEXTURE_RESOURCES_PATH + "awesomeface.png", true);

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), 
        static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);

    if (!spriteShaderOpt.has_value()) {
        ENGINE_CORE_ERROR("无法加载着色器 sprite");
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
    level = 1;
}

void Game::ProcessInput(float deltaTime) {

}

void Game::Update(float deltaTime) {

}

void Game::Render() {
    if (this->State == GameState::GAME_ACTIVE) {
        auto background = Engine::ResourceManager::GetTexture("background");
        if (background.has_value()) {
            renderer->DrawSprite(background->get(), glm::vec2(0.0f), glm::vec2(this->Width, this->Height), glm::vec3(0.0f));
        } else {
            APP_ERROR("Game::Render: 没有找到背景图片");
        }

        levels[level].Draw(*renderer.get());
    }
}

void Game::setKey(uint index, bool state) {
    this->Keys[index] = state;
}

void Game::setGameState(GameState state) {
    this->State = state;
}

Game::~Game() {
    
}