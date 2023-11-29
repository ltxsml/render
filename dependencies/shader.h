#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include"GLError.h"


#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include<unordered_map>


class Shader
{
public:
    // 构造器读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath);
    ~Shader(){ glDeleteProgram(ID);}
    // 使用/激活程序
    void use(){glUseProgram(ID);}
    // uniform工具函数
    void setBool(const std::string &name, bool value) ;  
    void setInt(const std::string &name, int value) ;   
    void setFloat(const std::string &name, float value) ;
    void setVec3(const std::string &name, float a,float b,float c);
    void setVec3(const std::string &name,const glm::vec3& v);
    void setMat3(const std::string &name,const glm::mat3 &matrix);
    void setMat4(const std::string &name,const glm::mat4 &matrix) ;
    unsigned int get_id()const {return ID;}
    private:
    typedef std::unordered_map<std::string ,  int> cache;

    // 程序ID
    unsigned int ID;
    cache m_uniformLocationCache;
    std::string getCode(const char* Codepath);
    unsigned compileShader(const char* shaderCode,unsigned shaderType);
    unsigned createShader(const char* vertexCode,const char* fragmentCode);
    int getUniformLocation(const std::string &name);
};


std::string Shader::getCode(const char* codePath){
    std::ifstream file;
    std::stringstream codeStream;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try{
        file.open(codePath);
        
        // 读取文件的缓冲内容到数据流中
        codeStream << file.rdbuf();  
        // 关闭文件处理器
        file.close();
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << "\n";
    }
    return codeStream.str();
}

unsigned Shader::compileShader(const char* shaderCode,unsigned shaderType){
    GLCall(unsigned int id = glCreateShader(shaderType));
    GLCall(glShaderSource(id,1,&shaderCode,NULL));
    GLCall(glCompileShader(id));

    int result;
    glGetShaderiv(id,GL_COMPILE_STATUS,&result);
    if(!result)
    {
        int length;
        glGetShaderiv(id,GL_INFO_LOG_LENGTH,&length);
        char *message = (char*)alloca(length*sizeof(char));
        glGetShaderInfoLog(id,length,&length,message);
        if(shaderType==GL_VERTEX_SHADER)
        std::cout<<"ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"<<message<<"\n";
        if(shaderType==GL_FRAGMENT_SHADER)
        std::cout<<"ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"<<message<<"\n";
        return 0;
    }
    return id;
}

unsigned Shader::createShader(const char* vertexCode,const char* fragmentCode){
    unsigned id = glCreateProgram();
    unsigned vertexShader = compileShader(vertexCode,GL_VERTEX_SHADER);
    unsigned fragmentShader = compileShader(fragmentCode,GL_FRAGMENT_SHADER);
    glAttachShader(id,vertexShader);
    glAttachShader(id,fragmentShader);
    glLinkProgram(id);
    int result;
    glGetProgramiv(ID, GL_LINK_STATUS, &result);
    if(!result)
    {   char infoLog[512];
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
    }
    GLCall(glDeleteShader(vertexShader));
    GLCall(glDeleteShader(fragmentShader));
    return id;
}

Shader::Shader(const char* vertexPath, const char* fragmentPath){
    
    std::string vShaderCode = getCode(vertexPath);
    std::string fShaderCode = getCode(fragmentPath);
    const char* vscode = vShaderCode.c_str();
    const char* fscode = fShaderCode.c_str();
    ID = createShader(vscode,fscode);
}

int Shader::getUniformLocation(const std::string &name) 
{
    if(m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
        return m_uniformLocationCache[name];
    int location =  glGetUniformLocation(ID,name.c_str());
    if(location == -1)
        std::cout<<"Warning: uniform "<<name<<" dosen't exsit\n";
    m_uniformLocationCache[name] = location;
    return location;
}

void Shader::setBool(const std::string &name, bool value) 
{   
    glUniform1i(getUniformLocation(name), (int)value); 
}
void Shader::setInt(const std::string &name, int value) 
{ 
    glUniform1i(getUniformLocation(name), value); 
}
void Shader::setFloat(const std::string &name, float value) 
{ 
    glUniform1f(getUniformLocation(name), value); 
} 

void Shader::setVec3(const std::string &name, float a,float b,float c){
    glUniform3f(getUniformLocation(name),a,b,c);
}

void Shader::setVec3(const std::string &name,const glm::vec3& v){
    glUniform3f(getUniformLocation(name),v[0],v[1],v[2]);
}

void Shader::setMat3(const std::string &name,const glm::mat3 &matrix)
{
    int uniformLoc = getUniformLocation(name);
    glUniformMatrix3fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setMat4(const std::string &name,const glm::mat4 &matrix) 
{
    int uniformLoc = getUniformLocation(name);
    glUniformMatrix4fv(uniformLoc, 1, GL_FALSE, glm::value_ptr(matrix));
}

#endif