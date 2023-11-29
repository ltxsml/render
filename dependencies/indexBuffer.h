#ifndef ELEMENTBUFFER_H
#define ELEMENTBUFFER_H

#include <GL/glew.h>

class IndexBuffer{
    public:
    IndexBuffer(const unsigned int* data,unsigned int count);
    IndexBuffer(IndexBuffer &) = delete;
    ~IndexBuffer(){ glDeleteBuffers(1,&m_ID);}
    void bind()const{   glBindBuffer(GL_ARRAY_BUFFER, m_ID); }
    void unbind()const{ glBindBuffer(GL_ARRAY_BUFFER, 0);}
    unsigned int get_count () const {return m_count;}
    private:
    unsigned m_ID;
    unsigned m_count;
};


// IndexBuffer::IndexBuffer(const unsigned int* data){
//     glGenBuffers(1, &m_ID);  //生成缓冲对象
//     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);  //绑定缓冲区
//     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);
//     m_count = sizeof(data)/sizeof(unsigned);
// }

IndexBuffer::IndexBuffer(const unsigned int* data,unsigned int count){
    glGenBuffers(1, &m_ID);  //生成缓冲对象
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID);  //绑定缓冲区
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*sizeof(unsigned), data, GL_STATIC_DRAW);
    m_count=count;
}


#endif