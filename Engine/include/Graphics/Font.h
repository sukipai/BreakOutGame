#ifndef __ZSH_ENGINE_FONT_H
#define __ZSH_ENGINE_FONT_H

#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <Texture.h>

struct Character {
    glm::ivec2 size;       // Size of glyph
    glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
    unsigned int advance;  // Horizontal offset to next glyph (in 1/64 pixels)
    glm::vec2 texCoords;   // Texture coordinates in atlas (normalized)
    glm::vec2 texSize;     // Texture size in atlas (normalized)
};

class Font {
private:
    std::unordered_map<char, Character> m_characters;
    Texture m_atlasTexture;
    unsigned int m_fontSize;
    std::string m_fontPath;

public:
    Font(const std::string& fontPath, unsigned int fontSize = 48);

    const Character& getCharacter(char c) const;
    Texture& getAtlasTexture() { return m_atlasTexture; }
    unsigned int getFontSize() const { return m_fontSize; }

    // Calculate width of a text string in pixels at given scale
    float getTextWidth(const std::string& text, float scale = 1.0f) const;

    // Get line height (approximate)
    float getLineHeight(float scale = 1.0f) const { return m_fontSize * scale; }
};

#endif // __ZSH_ENGINE_FONT_H