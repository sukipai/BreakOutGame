#ifndef __ZSH_GAME_SIMPLEFONT_H
#define __ZSH_GAME_SIMPLEFONT_H

#include <string>
#include <glm/glm.hpp>
#include <TextRenderer.h>
#include <Font.h>

// 简单的文本渲染包装器类
// 提供类似于用户请求的接口：
//   SimpleFont font("文本", glm::vec3(颜色), 大小);
//   font.draw(位置);
//   font.setText("新文本");
//   std::string text = font.getText();
class SimpleFont {
private:
    TextRenderer& m_textRenderer;
    Font& m_font;
    std::string m_text;
    glm::vec3 m_color;
    float m_scale;
    

public:
    // 构造函数
    SimpleFont(TextRenderer& textRenderer, Font& font,
               const std::string& text = "",
               const glm::vec3& color = glm::vec3(1.0f),
               float scale = 1.0f)
        : m_textRenderer(textRenderer), m_font(font),
          m_text(text), m_color(color), m_scale(scale) {}

    // 渲染文本到指定位置
    void draw(const glm::vec2& position) const {
        m_textRenderer.renderText(m_font, m_text, position, m_scale, m_color);
    }

    // 渲染文本到指定位置（重载，使用不同的缩放）
    void draw(const glm::vec2& position, float scale) const {
        m_textRenderer.renderText(m_font, m_text, position, scale, m_color);
    }

    // 渲染文本到指定位置（重载，使用不同的颜色和缩放）
    void draw(const glm::vec2& position, const glm::vec3& color, float scale) const {
        m_textRenderer.renderText(m_font, m_text, position, scale, color);
    }

    // 设置文本内容
    void setText(const std::string& text) {
        m_text = text;
    }

    // 获取文本内容
    const std::string& getText() const {
        return m_text;
    }

    // 设置颜色
    void setColor(const glm::vec3& color) {
        m_color = color;
    }

    // 获取颜色
    const glm::vec3& getColor() const {
        return m_color;
    }

    // 设置缩放
    void setScale(float scale) {
        m_scale = scale;
    }

    // 获取缩放
    float getScale() const {
        return m_scale;
    }

    // 获取文本宽度（像素）
    float getTextWidth() const {
        return m_font.getTextWidth(m_text, m_scale);
    }

    // 获取文本宽度（指定缩放）
    float getTextWidth(float scale) const {
        return m_font.getTextWidth(m_text, scale);
    }

    // 获取字体引用
    Font& getFont() { return m_font; }
    const Font& getFont() const { return m_font; }

    // 获取文本渲染器引用
    TextRenderer& getTextRenderer() { return m_textRenderer; }
    const TextRenderer& getTextRenderer() const { return m_textRenderer; }
};

#endif // __ZSH_GAME_SIMPLEFONT_H