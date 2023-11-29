#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H

#include <GL/glew.h>

class VertexBuffer{
    public:
    // VertexBuffer(const void* data);
    VertexBuffer(const void* data,unsigned int size);
    VertexBuffer(VertexBuffer&) = delete;
    ~VertexBuffer(){ glDeleteBuffers(1,&m_id);}
    unsigned int get_id()const {return m_id;};
    void bind()const{  GLCall( glBindBuffer(GL_ARRAY_BUFFER, m_id)); }
    void unbind()const{ glBindBuffer(GL_ARRAY_BUFFER, 0);}
    private:
    unsigned m_id;
};


// VertexBuffer::VertexBuffer(const void* data):VertexBuffer(data,sizeof(data)){};
VertexBuffer::VertexBuffer(const void* data,unsigned int size){
    glGenBuffers(1, &m_id);  //生成缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, m_id);  //绑定缓冲区
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}


#endif