cmake_minimum_required(VERSION 3.14)
project(breakout LANGUAGES C CXX)

# 让 CMake 优先在 Homebrew 的目录里找依赖，防止 Miniconda 污染
set(CMAKE_PREFIX_PATH "/opt/homebrew" ${CMAKE_PREFIX_PATH})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_ROOT /Users/sukipai/Code/CPPLibraries/glfw)
set(GLFW_INCLUDE_DIR ${GLFW_ROOT}/include)
set(GLFW_LIBRARY ${GLFW_ROOT}/lib-arm64/libglfw3.a)
set(GLAD_ROOT /Users/sukipai/Code/CPPLibraries/glad)
set(STB_IMAGE /Users/sukipai/Code/CPPLibraries/stbImage)
set(GLM /Users/sukipai/Code/CPPLibraries/glm)
set(ENGINE_DIR ${CMAKE_SOURCE_DIR}/Engine)
set(FREETYPE_ROOT /Users/sukipai/Code/CPPLibraries/freeType)
set(FREETYPE_INCLUDE_DIR ${FREETYPE_ROOT}/include/freetype2)
set(FREETYPE_LIBRARY ${FREETYPE_ROOT}/lib/libfreetype.a)

find_package(assimp REQUIRED)
find_package(spdlog REQUIRED)

# 【关键修改 1】：告诉 spdlog 不要去外部（比如 conda）找 fmt 的动态库
set(SPDLOG_FMT_EXTERNAL OFF)

add_executable(breakout
    Game/src/main.cpp
    Game/src/Game.cpp
    Game/src/GameObject.cpp
    Game/src/GameLevel.cpp
    Game/src/BallObject.cpp
    Game/src/PowerUp.cpp
    Engine/src/Graphics/Shader.cpp
    Engine/src/Graphics/VertexBuffer.cpp
    Engine/src/Graphics/IndexBuffer.cpp
    Engine/src/Graphics/VertexArray.cpp
    Engine/src/Graphics/VertexBufferLayout.cpp
    Engine/src/Graphics/stb_image.cpp
    Engine/src/Graphics/Texture.cpp
    Engine/src/Graphics/Camera.cpp
    Engine/src/Graphics/Mesh.cpp
    Engine/src/Graphics/Model.cpp
    Engine/src/Resource/Resource.cpp
    Engine/src/Graphics/SpriteRenderer.cpp
    Engine/src/Core/log.cpp
    Engine/src/Graphics/ParticleSystem.cpp
    Engine/src/Graphics/Font.cpp
    Engine/src/Graphics/TextRenderer.cpp
    Engine/src/Graphics/PostProcessor.cpp
    ${GLAD_ROOT}/src/glad.c
)

target_include_directories(breakout
    PRIVATE
        ${GLFW_INCLUDE_DIR}
        ${ENGINE_DIR}/include/Graphics
        ${ENGINE_DIR}/include/Resource
        ${ENGINE_DIR}/include
        ${CMAKE_SOURCE_DIR}/Game/include
        ${GLAD_ROOT}/include
        ${STB_IMAGE}/include
        ${GLM}
        ${FREETYPE_INCLUDE_DIR}
)

target_link_libraries(breakout
    PRIVATE
        ${GLFW_LIBRARY}
        ${FREETYPE_LIBRARY}
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
        assimp::assimp
        # 【关键修改 2】：链接 spdlog_header_only，彻底抛弃 .dylib 文件
        spdlog::spdlog_header_only
        bz2
        z
)