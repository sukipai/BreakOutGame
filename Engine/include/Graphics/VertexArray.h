#pragma once

#include <VertexBuffer.h>
#include <VertexBufferLayout.h>

using uint = unsigned int;

class VertexArray {
private:
    uint m_rendererID;
public:
    VertexArray();
    ~VertexArray();

    VertexArray(VertexArray&& vertexArray);

    VertexArray& operator=(VertexArray&& vertexArray);

    void bind() const;
    void unbind() const;

    void addBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout);
};
