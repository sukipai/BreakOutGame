#ifndef LEARN_OPENGL_VERTEXBUFFER_H
#define LEARN_OPENGL_VERTEXBUFFER_H

using uint = unsigned int;

class VertexBuffer {
private:
    uint m_rendererID;
    bool m_hasData;
public:
    VertexBuffer(const void* data, unsigned int size);
    VertexBuffer();

    VertexBuffer(VertexBuffer&& buffer);
    
    VertexBuffer& operator=(VertexBuffer&& buffer);

    ~VertexBuffer();

    void setBuffer(const void* data, unsigned int size);
    void bind() const;
    void unbind() const;
};


#endif