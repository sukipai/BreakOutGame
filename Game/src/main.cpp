#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <print>
#include <chrono>
#include <Game.h>
#include <Core/log.h>

const unsigned int WINDOW_WIDTH     = 800;
const unsigned int WINDOW_HEIGHT    = 600;
double deltaTime;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

Game game(WINDOW_WIDTH, WINDOW_HEIGHT);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

int main() {
    Engine::Log::Init();
    if (!glfwInit()) {
        std::println("GLFW init failed");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow((int)WINDOW_WIDTH, (int)WINDOW_HEIGHT, "Hello OpenGL", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::println("GLAD init failed\n");
        return -1;
    }

    glfwSetKeyCallback(window, key_callback);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ENGINE_CORE_INFO(std::format("OpenGL version: {}", (char *)glGetString(GL_VERSION)));


    ENGINE_CORE_INFO("引擎日志系统初始化成功! ");

    game.Init();
    game.setGameState(GameState::GAME_ACTIVE);

    APP_INFO("Breakout 游戏开始");

    auto lastFrameTime = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
        auto currentFrameTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> duration = currentFrameTime - lastFrameTime;
        deltaTime = duration.count();
        lastFrameTime = currentFrameTime;

        glfwPollEvents();

        game.ProcessInput(deltaTime);
        game.Update(deltaTime);

        // Render here
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        game.Render();

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

double calcFPS(GLFWwindow* window) {
    static auto lastTime = glfwGetTime();
    static int nbFrames = 0;
    static float fps = 0;
    auto currentTime = glfwGetTime();

    nbFrames++;
    if (currentTime - lastTime >= 1.0f) {
        fps = nbFrames / (currentTime - lastTime);
        nbFrames = 0;
        lastTime = currentTime;
    }
    return fps;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    
    if (key >= 0 && key <= 1024) {
        if (action == GLFW_PRESS) {
            game.setKey(key, true);
        }
        else if (action == GLFW_RELEASE) {
            game.setKey(key, false);
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}