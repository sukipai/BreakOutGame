#ifndef __ZSH_ENGINE_RESOURCE_H
#define __ZSH_ENGINE_RESOURCE_H

#include <unordered_map>
#include <string>
#include <glad/glad.h>

#include <Texture.h>
#include <Shader.h>
#include <Graphics/Font.h>

namespace Engine {

class ResourceManager {
private:
    static std::unordered_map<std::string, Shader> Shaders;
    static std::unordered_map<std::string, Texture> Textures;
    static std::unordered_map<std::string, Font> Fonts;

public:
    static std::optional<std::reference_wrapper<Shader>> LoadShader(const std::string& name, const std::string& vShaderFile, const std::string& fShaderFile, const std::string& gShaderFile = "");

    static std::optional<std::reference_wrapper<Shader>> GetShader(const std::string& name);

    static std::optional<std::reference_wrapper<Texture>>  LoadTexture(const std::string& name, const std::string& file, bool alpha = true);

    static std::optional<std::reference_wrapper<Texture>> GetTexture(const std::string& name);

    // Font management
    static std::optional<std::reference_wrapper<Font>> LoadFont(
        const std::string& name,
        const std::string& fontFile,
        unsigned int fontSize = 48);

    static std::optional<std::reference_wrapper<Font>> GetFont(const std::string& name);

    static void Clear();

private:
    ResourceManager() { }
}; 

}

#endif