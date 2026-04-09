#include <Graphics/Font.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glad/glad.h>
#include <Core/log.h>
#include <iostream>

Font::Font(const std::string& fontPath, unsigned int fontSize)
    : m_fontPath(fontPath), m_fontSize(fontSize) {

    // Initialize FreeType
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        ENGINE_CORE_ERROR("Failed to initialize FreeType library");
        return;
    }

    // Load font face
    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        ENGINE_CORE_ERROR("Failed to load font: {}", fontPath);
        FT_Done_FreeType(ft);
        return;
    }

    // Set pixel size
    FT_Set_Pixel_Sizes(face, 0, fontSize);

    // Disable byte-alignment restriction for texture loading
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Determine atlas size - we'll pack ASCII characters 32-126
    const int charCount = 126 - 32 + 1;
    const int padding = 2;
    int atlasWidth = 0;
    int atlasHeight = 0;

    // First pass: calculate required atlas size
    for (unsigned char c = 32; c <= 126; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            ENGINE_CORE_WARN("Failed to load glyph for character: {}", c);
            continue;
        }

        atlasWidth += face->glyph->bitmap.width + padding;
        atlasHeight = std::max(atlasHeight, (int)face->glyph->bitmap.rows);
    }

    // Create texture atlas
    std::vector<unsigned char> atlasData(atlasWidth * atlasHeight, 0);
    int xOffset = 0;

    // Second pass: render glyphs to atlas and store character info
    for (unsigned char c = 32; c <= 126; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            continue;
        }

        FT_GlyphSlot glyph = face->glyph;
        int width = glyph->bitmap.width;
        int height = glyph->bitmap.rows;
        int pitch = std::abs(glyph->bitmap.pitch); // pitch may differ from width due to row alignment

        // Copy glyph bitmap to atlas
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                unsigned char pixel = glyph->bitmap.buffer[y * pitch + x];
                atlasData[(y * atlasWidth) + (x + xOffset)] = pixel;
            }
        }

        // Store character information
        Character character = {
            .size = glm::ivec2(width, height),
            .bearing = glm::ivec2(glyph->bitmap_left, glyph->bitmap_top),
            .advance = static_cast<unsigned int>(glyph->advance.x),
            .texCoords = glm::vec2((float)xOffset / (float)atlasWidth, 0.0f),
            .texSize = glm::vec2((float)width / (float)atlasWidth, (float)height / (float)atlasHeight)
        };

        m_characters[c] = character;
        xOffset += width + padding;
    }

    // Create OpenGL texture for atlas using Texture class
    m_atlasTexture.setTextureFromData(atlasData.data(), atlasWidth, atlasHeight, 1);

    // Debug: check if atlas contains any non-zero pixels
    int nonZeroCount = 0;
    for (size_t i = 0; i < atlasData.size(); i++) {
        if (atlasData[i] != 0) nonZeroCount++;
    }


    // Debug: print first few characters info
    for (unsigned char c = 32; c <= 38; c++) {
        auto it = m_characters.find(c);
        if (it != m_characters.end()) {
            const Character& ch = it->second;

        }
    }

    // Cleanup FreeType
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

const Character& Font::getCharacter(char c) const {
    static Character defaultChar = {
        .size = glm::ivec2(0, 0),
        .bearing = glm::ivec2(0, 0),
        .advance = 0,
        .texCoords = glm::vec2(0.0f),
        .texSize = glm::vec2(0.0f)
    };

    if (c < 32 || c > 126) {
        // Return space character for unsupported chars
        auto it = m_characters.find(' ');
        return it != m_characters.end() ? it->second : defaultChar;
    }

    auto it = m_characters.find(c);
    if (it != m_characters.end()) {
        return it->second;
    }

    return defaultChar;
}

float Font::getTextWidth(const std::string& text, float scale) const {
    float width = 0.0f;

    for (char c : text) {
        const Character& ch = getCharacter(c);
        width += (ch.advance >> 6) * scale; // advance is in 1/64 pixels
    }

    return width;
}