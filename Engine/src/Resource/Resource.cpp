#include <Resource.h>


namespace Engine {
    
std::unordered_map<std::string, Shader> ResourceManager::Shaders;
std::unordered_map<std::string, Texture> ResourceManager::Textures;
std::unordered_map<std::string, Font> ResourceManager::Fonts;

std::optional<std::reference_wrapper<Shader>> 
ResourceManager::LoadShader(const std::string& name, 
                            const std::string& vShaderFile, 
                            const std::string& fShaderFile, 
                            const std::string& gShaderFile) 
{
    auto [it, inserted] = Shaders.try_emplace(name, vShaderFile, fShaderFile, gShaderFile);

    if (!inserted) {
        return std::nullopt;
    }

    return std::ref(it->second);
}

std::optional<std::reference_wrapper<Shader>> ResourceManager::GetShader(const std::string& name) {
    auto it = Shaders.find(name);

    if (it == Shaders.end()) {
        return std::nullopt;
    }

    return std::ref(it->second);
}

std::optional<std::reference_wrapper<Texture>> 
ResourceManager::LoadTexture(   const std::string& name, 
                                const std::string& file, 
                                bool alpha) 
{
    auto [it, inserted] = Textures.try_emplace(name, file, alpha, true);

    if (!inserted) {
        return std::nullopt;
    }

    return std::ref(it->second);
}

std::optional<std::reference_wrapper<Texture>> ResourceManager::GetTexture(const std::string& name) {
    auto it = Textures.find(name);

    if (it == Textures.end()) {
        return std::nullopt;
    }

    return std::ref(it->second);
}

std::optional<std::reference_wrapper<Font>>
ResourceManager::LoadFont(const std::string& name,
                          const std::string& fontFile,
                          unsigned int fontSize)
{
    auto [it, inserted] = Fonts.try_emplace(name, fontFile, fontSize);

    if (!inserted) {
        return std::nullopt;
    }

    return std::ref(it->second);
}

std::optional<std::reference_wrapper<Font>> ResourceManager::GetFont(const std::string& name) {
    auto it = Fonts.find(name);

    if (it == Fonts.end()) {
        return std::nullopt;
    }

    return std::ref(it->second);
}

void ResourceManager::Clear() {
    Shaders.clear();
    Textures.clear();
    Fonts.clear();
}

} // namespace Engine