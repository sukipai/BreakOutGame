#ifndef LEARN_OPENGL_INDEXBUFFER_H
#define LEARN_OPENGL_INDEXBUFFER_H

using uint = unsigned int;

class IndexBuffer {
private:
    uint m_rendererID;
    uint m_count;
    bool m_hasData;

public:
    IndexBuffer(const uint* data, uint count);

    IndexBuffer();

    IndexBuffer(IndexBuffer&& buffer);

    IndexBuffer& operator=(IndexBuffer&& buffer);

    ~IndexBuffer();

    void setBuffer(const uint* data, uint count);
    void bind() const;
    void unbind() const;

    inline uint getCount() const { return m_count; }
};


#endif