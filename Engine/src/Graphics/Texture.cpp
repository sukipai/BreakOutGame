#include <Texture.h>
#include <glad/glad.h>
#include <stb_image.h>
#include <print>

Texture::Texture(const std::string& path, bool isRGBA, bool flip) : m_filePath(path) {
    glGenTextures(1, &m_rendererID);
    glBindTexture(GL_TEXTURE_2D, m_rendererID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // 既然生成了 Mipmap，推荐使用这个过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // 修复了 GL_TEXTURE 的 typo

    stbi_set_flip_vertically_on_load(flip);

    m_imageData = stbi_load(m_filePath.c_str(), &m_width, &m_height, &m_channels, 0);

    if (!m_imageData) {
        std::println("Error: Texture could not read {}.", m_filePath);
        m_hasImage = false;
        return; 
    }

    // 1. 动态判断正确的通道格式，防止内存越界读出乱码
    GLenum srcChannelFormat = GL_RGB;
    if (m_channels == 1) {
        srcChannelFormat = GL_RED;
    } else if (m_channels == 3) {
        srcChannelFormat = GL_RGB;
    } else if (m_channels == 4) {
        srcChannelFormat = GL_RGBA;
    }

    // 2. 解除 OpenGL 默认的 4 字节对齐限制，防止非 2 次幂或奇数宽度的 RGB 图片花屏错位
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 3. 上传纹理数据
    glTexImage2D(GL_TEXTURE_2D, 0, srcChannelFormat, m_width, m_height, 0, srcChannelFormat, GL_UNSIGNED_BYTE, m_imageData);
    glGenerateMipmap(GL_TEXTURE_2D);

    m_hasImage = true;

    stbi_image_free(m_imageData);
    m_imageData = nullptr;
}
Texture::Texture() :    m_rendererID(0), m_filePath(), m_width(0), m_height(0), m_channels(0),
                        m_imageData(nullptr), m_hasImage(false) {
}

Texture::Texture(Texture&& texture) {
    m_rendererID            = texture.m_rendererID;
    m_filePath              = texture.m_filePath;
    m_width                 = texture.m_width;
    m_height                = texture.m_height;
    m_channels              = texture.m_channels;
    m_imageData             = texture.m_imageData;
    m_hasImage              = texture.m_hasImage;

    texture.m_rendererID    = 0;
    texture.m_imageData     = nullptr;
    texture.m_hasImage      = false;
}

Texture& Texture::operator=(Texture&& texture) {
    if (m_rendererID != 0) {
        glDeleteTextures(1, &m_rendererID);
    }

    m_rendererID            = texture.m_rendererID;
    m_filePath              = texture.m_filePath;
    m_width                 = texture.m_width;
    m_height                = texture.m_height;
    m_channels              = texture.m_channels;
    m_imageData             = texture.m_imageData;
    m_hasImage              = texture.m_hasImage;

    texture.m_rendererID    = 0;
    texture.m_imageData     = nullptr;
    texture.m_hasImage      = false;

    return *this;
}


void Texture::bind(uint slot) const{
    if (!m_hasImage) {
        std::println("Warning: There is not an image in Texture");
    }
    
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_rendererID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setTexture(const std::string& path, bool isRGBA, bool flip) {
    stbi_set_flip_vertically_on_load(flip);
    int width, height, channels;
    m_imageData = stbi_load(path.c_str(), &width, &height, &channels, 0);
    
    if (!m_imageData) {
        std::println("Error: Failed to load new texture from {}.", path);
        return;
    }

    if (m_rendererID == 0) {
        glGenTextures(1, &m_rendererID);
        glBindTexture(GL_TEXTURE_2D, m_rendererID);
    
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }

    glBindTexture(GL_TEXTURE_2D, m_rendererID);

    m_width = width;
    m_height = height;
    m_channels = channels;
    m_filePath = path;

    // 1. 动态判断正确的通道格式，防止内存越界读出乱码
    GLenum srcChannelFormat = GL_RGB;
    if (m_channels == 1) {
        srcChannelFormat = GL_RED;
    } else if (m_channels == 3) {
        srcChannelFormat = GL_RGB;
    } else if (m_channels == 4) {
        srcChannelFormat = GL_RGBA;
    }

    // 2. 解除 OpenGL 默认的 4 字节对齐限制，防止非 2 次幂或奇数宽度的 RGB 图片花屏错位
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    auto interFormat = srcChannelFormat;

    glTexImage2D(GL_TEXTURE_2D, 0, interFormat, m_width, m_height, 0, srcChannelFormat, GL_UNSIGNED_BYTE, m_imageData);
    glGenerateMipmap(GL_TEXTURE_2D);


    stbi_image_free(m_imageData);

    m_hasImage = true;
    m_imageData = nullptr;
}

Texture::~Texture() {
    if (m_rendererID == 0) return;
    glDeleteTextures(1, &m_rendererID);
}