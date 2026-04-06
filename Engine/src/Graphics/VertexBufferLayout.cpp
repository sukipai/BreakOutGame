#include <VertexBufferLayout.h>
#include <glad/glad.h>

const std::vector<VertexBufferElement>& VertexBufferLayout::getElements() const {
    return m_elements;
}

uint VertexBufferLayout::getStride() const {
    return m_stride;
}

template<>
void VertexBufferLayout::push<float>(uint count) {
    m_elements.push_back({GL_FLOAT,  count, GL_FALSE } );
    m_stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
}

template<>
void VertexBufferLayout::push<uint>(uint count) {
    m_elements.push_back({GL_UNSIGNED_INT,  count, GL_FALSE } );
    m_stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
}

template<>
void VertexBufferLayout::push<ubyte>(uint count) {
    m_elements.push_back({GL_UNSIGNED_BYTE,  count, GL_FALSE } );
    m_stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
}