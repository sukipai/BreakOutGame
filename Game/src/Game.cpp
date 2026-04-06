#include <Game.h>
#include <Resource.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Core/log.h>

static const std::string SHADER_RESOURCES_PATH = "Game/Resources/Shaders/";
static const std::string TEXTURE_RESOURCES_PATH = "Game/Resources/Textures/";

Game::Game(uint width, uint height) : State(GameState::GAME_ACTIVE), Width(width), Height(height) {

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
}

void Game::ProcessInput(float deltaTime) {

}

void Game::Update(float deltaTime) {

}

void Game::Render() {
    auto faceTextureOpt = Engine::ResourceManager::GetTexture("face");
    if (!faceTextureOpt.has_value()) {
        ENGINE_CORE_ERROR("无法加载纹理 face");
        exit(0);
    }
    
    renderer->DrawSprite(faceTextureOpt->get(), 
                        glm::vec2(200, 200), glm::vec2(300, 400), glm::vec3(0.0f, 1.0f, 0.0f), 45.0f);
}

void Game::setKey(uint index, bool state) {
    this->Keys[index] = state;
}

void Game::setGameState(GameState state) {
    this->State = state;
}

Game::~Game() {
    
}