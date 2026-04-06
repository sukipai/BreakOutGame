#include <IndexBuffer.h>
#include <glad/glad.h>

IndexBuffer::IndexBuffer(const uint* data, uint count) : m_count(count) {
    glGenBuffers(1, &m_rendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), data, GL_STATIC_DRAW);
    m_hasData = true;
}

IndexBuffer::IndexBuffer() : m_rendererID(0), m_count(0), m_hasData(false) {

}

IndexBuffer::IndexBuffer(IndexBuffer&& buffer) : m_rendererID(buffer.m_rendererID), m_count(buffer.m_count), m_hasData(buffer.m_hasData) {
    buffer.m_rendererID = 0;
    buffer.m_count      = 0;
    buffer.m_hasData    = 0;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& buffer) {
    if (m_rendererID != 0) {
        glDeleteBuffers(1, &m_rendererID);
    }

    m_rendererID            = buffer.m_rendererID;
    m_count                 = buffer.m_count;
    m_hasData               = buffer.m_hasData;

    buffer.m_rendererID     = 0;
    buffer.m_count          = 0;
    buffer.m_hasData        = 0;
    
    return *this;
}



IndexBuffer::~IndexBuffer() {
    if (m_rendererID == 0) return;
    glDeleteBuffers(1, &m_rendererID);
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
}

void IndexBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::setBuffer(const uint* data, uint count) {
    if (!m_hasData) {
        glGenBuffers(1, &m_rendererID);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_rendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint), data, GL_STATIC_DRAW);
    m_hasData = true;
}