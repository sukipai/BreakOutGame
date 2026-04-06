#include "VertexBufferLayout.h"
#include <VertexArray.h>
#include <glad/glad.h>
#include <print>

VertexArray::VertexArray() {
    glGenVertexArrays(1, &m_rendererID);
}

VertexArray::VertexArray(VertexArray&& vertexArray) {
    this->m_rendererID = vertexArray.m_rendererID;
    vertexArray.m_rendererID = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& vertexArray) {
    if (this->m_rendererID != 0) {
        glDeleteVertexArrays(1, &this->m_rendererID);
    }

    this->m_rendererID = vertexArray.m_rendererID;
    vertexArray.m_rendererID = 0;
    
    return *this;
}

VertexArray::~VertexArray() {
    if (m_rendererID == 0) return;
    glDeleteVertexArrays(1, &m_rendererID);
}

void VertexArray::bind() const {
    glBindVertexArray(m_rendererID);
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}

void VertexArray::addBuffer(const VertexBuffer& buffer, const VertexBufferLayout& layout) {
    bind();
    buffer.bind();

    const auto & elements = layout.getElements();
    uint offset = 0;

    for (int i = 0; i < elements.size(); i++) {
        auto & element = elements[i];

        glEnableVertexAttribArray(i);
        glVertexAttribPointer(
            i,
            element.count,
            element.type,
            element.normalized,
            layout.getStride(),
            (void *)(uintptr_t)offset
        );
        offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
    }
}