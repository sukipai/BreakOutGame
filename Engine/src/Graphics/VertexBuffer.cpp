#include <VertexBuffer.h>
#include <glad/glad.h>

VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
    glGenBuffers(1, &m_rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    m_hasData = true;
}

VertexBuffer::VertexBuffer() : m_rendererID(0), m_hasData(false) {

}

VertexBuffer::VertexBuffer(VertexBuffer&& buffer) {
    this->m_hasData = buffer.m_hasData;
    this->m_rendererID = buffer.m_rendererID;

    buffer.m_rendererID = 0;
    buffer.m_hasData = false;
}
    
VertexBuffer& VertexBuffer::operator=(VertexBuffer&& buffer) {
    if (this->m_rendererID != 0) {
        glDeleteBuffers(1, &m_rendererID);
    }

    this->m_hasData = buffer.m_hasData;
    this->m_rendererID = buffer.m_rendererID;

    buffer.m_rendererID = 0;
    buffer.m_hasData = false;

    return *this;
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::setBuffer(const void* data, unsigned int size) {
    if (!m_hasData) {
        glGenBuffers(1, &m_rendererID);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    m_hasData = true;
}

VertexBuffer::~VertexBuffer() {
    if (m_rendererID == 0) return;
    glDeleteBuffers(1, &m_rendererID);
}