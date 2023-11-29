#ifndef GLERROR_H
#define GLERROR_H

#include<iostream>
#include<GL/glew.h>

#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCall(x) GLClearError(); \
    x;\
    ASSERT(GLLogCall(#x, __FILE__ , __LINE__))

void GLClearError()
{
    while(glGetError()!=GL_NO_ERROR);
}

bool GLLogCall(const char* function,const char* file,int line)
{
    while(GLenum error = glGetError())
    {
        std::cout<< "GLERROR: ("<< error<<" ): "<< function<<
        "\n "<<file<<": "<<line<<"\n";
        return false;
    }
    return true;
}

#endif