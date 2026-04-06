#pragma once

#include <vector>
#include <glad/glad.h>

using uint = unsigned int;
using ubyte = unsigned char;

struct VertexBufferElement {
    uint type;
    uint count;
    ubyte normalized;

    static uint GetSizeOfType(uint type) {
        switch (type) {
            case GL_FLOAT: return 4;
            case GL_UNSIGNED_INT: return 4;
            case GL_UNSIGNED_BYTE: return 1;
        }
        return 0;
    }
};

class VertexBufferLayout {
private:
    std::vector<VertexBufferElement> m_elements;
    uint m_stride;
public:
    VertexBufferLayout() : m_stride(0) { }
    ~VertexBufferLayout() = default;

    template<typename T>
    void push(uint count);

    const std::vector<VertexBufferElement>& getElements() const;
    uint getStride() const;
};