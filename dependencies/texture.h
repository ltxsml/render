#ifndef TEXTURE
#define TEXTURE
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glew.h>
#include"GLError.h"

#include<iostream>

class Texture{
    public:
    Texture(const std::string &texturePath,unsigned int textureUnit);
    Texture(const std::string &texturePath,unsigned int textureUnit,unsigned int pictureKind);
    Texture(Texture &) = delete;
    unsigned get_id() const {return m_id;}
    void bind(unsigned int textureUnit);
    void unbind();

    ~Texture() { glDeleteTextures(1, &m_id);};
    private:
    unsigned int m_id;
    int m_Height;
    int m_Width;
    int m_BPP;
    unsigned char *m_Data ;

};

Texture::Texture(const std::string &texturePath,unsigned int textureUnit,unsigned int pictureKind){
    glGenTextures(1,&m_id);
    glActiveTexture(GL_TEXTURE0+textureUnit); // 在绑定纹理之前先激活纹理单元
    glBindTexture(GL_TEXTURE_2D,m_id);
    //设置环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    //设置边缘颜色
    // float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    // glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    //设置纹素像素匹配方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //多级渐远纹理
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    const char * tPath = texturePath.c_str();
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(tPath, &width, &height, &nrChannels, 0);
    if (data)
    {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, pictureKind, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
    std::cout << "Texture failed to load at path: " << texturePath << std::endl;
    }
    
    stbi_image_free(data);
}


Texture::Texture(const std::string &texturePath,unsigned int textureUnit)
    :m_id(0),m_Width(0),m_Height(0),m_BPP(0),m_Data(nullptr)
{
    glGenTextures(1,&m_id);
    glActiveTexture(GL_TEXTURE0+textureUnit); // 在绑定纹理之前先激活纹理单元
    //加载纹理
    const char * tPath = texturePath.c_str();
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(tPath, &m_Width, &m_Height, &m_BPP, 0);
    if (data)
    {
        GLenum format;
        if (m_BPP == 1)
            format = GL_RED;
        else if (m_BPP == 3)
            format = GL_RGB;
        else if (m_BPP == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, m_id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);//第二个0是设置边框
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D,0);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << texturePath << std::endl;
        stbi_image_free(data);
    }

}

void Texture::bind(unsigned int textureUnit)
{
    glActiveTexture(GL_TEXTURE0+textureUnit);
    glBindTexture(GL_TEXTURE_2D,m_id);
}

void Texture::unbind(){
    glBindTexture(GL_TEXTURE_2D,0);
}
#endif