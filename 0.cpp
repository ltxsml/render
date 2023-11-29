#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "dependencies/stb_image.h"
#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<fstream>
#include<iostream>
#include<string>
#include<sstream>
#include<cmath>

#define ASSERT(x) if(!(x)) __debug_break();
#define GLcall(x) do{GLclearerror();\
x;\
ASSERT (glLogCall());}\
while(0)


enum shaderTypes{None=-1,vertex=0,fragment=1};

struct shaderProgramSrc{
    std::string vertexSource;
    std::string fragmentSource;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

static void GLclearerror()
{
    while(!glGetError());
}

static void GLCheckError()
{
    while(GLenum error =glGetError()){
        std::cout<<"[OpenGL Error]:"<<error<<"\n";
    }
}

static bool glLogCall()
{
    while(GLenum error =glGetError()){
        std::cout<<"[OpenGL Error]:"<<error<<"\n";
        return false;
    }
    return true;
}

static shaderProgramSrc ParseShader(const std::string &filepath){
    std::ifstream stream(filepath);
    std::string line;
    shaderTypes t=None;
    std::stringstream ss[2];
    while(getline(stream,line)){
        if(line.find("vertex")!=std::string::npos)
        {
            t=vertex;
        }
        else if(line.find("fragment")!=std::string::npos)
        {
            t=fragment;
            
        }
        else{
            ss[(int) t]<<line<<"\n";
        }

    }
    return {ss[0].str(),ss[1].str()};
}

static unsigned CompileShader(const std::string &source,unsigned type)
{
    unsigned int id = glCreateShader(type);
    const char * src = source.c_str();
    glShaderSource(id,1,&src,nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id,GL_COMPILE_STATUS,&result);
    if(!result)
    {
        int length;
        glGetShaderiv(id,GL_INFO_LOG_LENGTH,&length);
        char *message = (char*)alloca(length*sizeof(char));
        glGetShaderInfoLog(id,length,&length,message);
        std::cout<<message<<"\n";
        return 0;
    }
    return id;
}

static unsigned int CreateShader(const std::string &vertexShader,const std::string &fragmentShader){
    unsigned program = glCreateProgram();
    unsigned int vs = CompileShader(vertexShader,GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShader,GL_FRAGMENT_SHADER);
    glAttachShader(program,vs);
    glAttachShader(program,fs);
    glLinkProgram(program);
    

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}



int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    if(glewInit()!=GLEW_OK) std::cout<<"Error";

    
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

    float vertexes[]={
     0.5f,-0.5f,1.0f,0.0f,0.0f, 1.0f,0.0f,
     -0.5f,-0.5f,0.0f,1.0f,0.0f, 0.0f, 0.0f,
     0.5f, 0.5f,0.0f,0.0f,1.0f, 1.0f, 1.0f,
     -0.5f,0.5f,1.0f,0.0f,0.0f, 0.0f, 1.0f
        };

    unsigned int indicies[6]={
        0,1,2,
        1,2,3
    };


    unsigned int buffer;
    unsigned int EBO;
    unsigned int VAO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    
    glGenBuffers(1,&buffer);
    glBindBuffer(GL_ARRAY_BUFFER,buffer);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertexes),vertexes,GL_STATIC_DRAW);

    //创建纹理
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    int width, height, nrChannels;
    unsigned char *data = stbi_load("textures//container.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    //位置属性
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,7*sizeof(float),0);
    // 颜色属性
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2* sizeof(float)));
    glEnableVertexAttribArray(1);
    //纹理属性
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5* sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indicies),indicies,GL_STATIC_DRAW);


    
    shaderProgramSrc source = ParseShader("./res/shaders/green.shader");
    // std::cout<<source.fragmentSource;

    unsigned shader1 = CreateShader(source.vertexSource,source.fragmentSource);


    // source = ParseShader("./res/shaders/blue.shader");
    // unsigned shader2 =CreateShader(source.vertexSource,source.fragmentSource);
    
    // int vertexColorLocation = glGetUniformLocation(shader2, "ourColor");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        /* Render here */
        glClearColor(0.2f,0.3f,0.3f,0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(shader1);

        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        // glDrawArrays(GL_TRIANGLES,0,3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteProgram(shader1);
    // glDeleteProgram(shader2);

    glfwTerminate();
    return 0;
}