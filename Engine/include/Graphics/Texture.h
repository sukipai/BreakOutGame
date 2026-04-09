#pragma once

#include <string>

class Texture {
    using uint = unsigned int;
    using ubyte = unsigned char;
private:
    uint m_rendererID;
    std::string m_filePath;
    int m_width, m_height, m_channels;
    ubyte* m_imageData;
    bool m_hasImage;
public:
    Texture(const std::string& path, bool isRGBA = false, bool flip = true);
    Texture();

    Texture(Texture&& texutre);
    Texture(Texture& texture)=delete;

    Texture& operator=(Texture&& texture);
    Texture& operator=(Texture& texture)=delete;

    Texture(const Texture& texture) = delete;
    Texture& operator=(const Texture& texture) = delete;

    ~Texture();

    void bind(uint slot) const;
    void unbind() const;
    void setTexture(const std::string& path, bool isRGBA = false, bool flip = true);
    void setTextureFromData(unsigned char* data, int width, int height, int channels = 1);

    inline int width() const { return m_width; }
    inline int height() const { return m_height; }
    inline bool has_image() const { return m_hasImage; } 
};