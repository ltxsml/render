#define GLEW_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include<GL/glew.h>
#include <GLFW/glfw3.h>
#include<iostream>
#include<string>
#include<cmath>
#include<glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include"GLError.h"
#include"shader.h"
#include"indexBuffer.h"
#include"vertexBuffer.h"
#include"texture.h"   //抽象很少


float a = 0.7f;
float speed = 0.0001f;

float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f; // 上一帧的时间

constexpr float screenWidth = 800;
constexpr float screenHeight = 600;
float lastX = screenWidth/2, lastY = screenHeight/2;
float yaw = -90.0f , pitch = 0.0f ;
float fov = 30.0f;
bool firstMouse = true ;
bool scare = false;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  7.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


//当用户改变窗口的大小的时候，视口也应该被调整。我们可以对窗口注册一个回调函数(Callback Function)，它会在每次窗口大小被调整的时候被调用
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}



int main()
{   
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

// 设置窗口大小为屏幕的大小

    {GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glViewport(0, 0, mode->width,mode->height );  //glViewport函数前两个参数控制窗口左下角的位置。第三个和第四个参数控制渲染窗口的宽度和高度（像素）
    
    glfwMakeContextCurrent(window);
    //初始化glew
    if(glewInit()!=GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    //注册回调函数，在创建窗口之后，渲染循环初始化之前注册这些回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window,scroll_callback);


float vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};
    float thVertex[] = {
        -1.0f,1.0f,0.0f,0.0f,1.0f,
        -1.0f,-1.0f,0.0f,0.0f,0.0f,
        1.0f,1.0f,0.0f,1.0f,1.0f,
        1.0f,1.0f,0.0f,1.0f,1.0f,
        -1.0f,-1.0f,0.0f,0.0f,0.0f,
        1.0f,-1.0f,0.0f,1.0f,0.0f

};

    glm::vec3 cubePositions[] = {
  glm::vec3( 0.0f,  0.0f,  0.0f), 
  glm::vec3( 2.0f,  5.0f, -15.0f), 
  glm::vec3(-1.5f, -2.2f, -2.5f),  
  glm::vec3(-3.8f, -2.0f, -12.3f),  
  glm::vec3( 2.4f, -0.4f, -3.5f),  
  glm::vec3(-1.7f,  3.0f, -7.5f),  
  glm::vec3( 1.3f, -2.0f, -2.5f),  
  glm::vec3( 1.5f,  2.0f, -2.5f), 
  glm::vec3( 1.5f,  0.2f, -1.5f), 
  glm::vec3(-1.3f,  1.0f, -1.5f)  
};
    glm::vec3 lightPosition = glm::vec3( 1.2f, 1.0f, 2.0f);

    Texture diffTexture("textures/container2.png",GL_TEXTURE0,GL_RGBA);
    Texture specTexture("textures/container2_specular.png",GL_TEXTURE1,GL_RGBA);
    Texture emiTexture("textures/matrix.jpg",GL_TEXTURE3,GL_RGB);
    Texture lightTexture("textures/th.jpg",GL_TEXTURE2,GL_RGB);

    unsigned int objectVAO;
    GLCall(glGenVertexArrays(1, &objectVAO));
    GLCall(glBindVertexArray(objectVAO));

    GLCall(VertexBuffer VBO(vertices,sizeof(vertices)));
    

    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)0));
    GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)(3*sizeof(float))));
    GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)(6*sizeof(float))));
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glEnableVertexAttribArray(1));
    GLCall(glEnableVertexAttribArray(2));
    GLCall(Shader OBshader("res/shaders/objectVertex.shader","res/shaders/objectFragment.shader"));
    GLCall(Shader lightshader("res/shaders/lightVertex.shader","res/shaders/lightFrag.shader"));
    GLCall(Shader thshader("res/shaders/th.vs","res/shaders/th.fs"));
    GLCall(OBshader.use());
    OBshader.setVec3("objectColor",1.0f, 0.5f, 0.31f);
    OBshader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);

    unsigned int lightVAO;
    GLCall(glGenVertexArrays(1, &lightVAO));
    GLCall(glBindVertexArray(lightVAO));
    VBO.bind();
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
    GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)(3*sizeof(float))));
    GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8* sizeof(float), (void*)(6*sizeof(float))));
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glEnableVertexAttribArray(2));

    unsigned int thVAO;
    GLCall(glGenVertexArrays(1, &thVAO));
    GLCall(glBindVertexArray(thVAO));
    GLCall(VertexBuffer thVBO(thVertex,sizeof(thVertex)));
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)0));
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3*sizeof(float))));
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glEnableVertexAttribArray(1));
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    

    //渲染循环
    while(!glfwWindowShouldClose(window))
    {   
        processInput(window);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        // if(!scare){
        //     if(sin(0.08*glfwGetTime())>0.9f){
        //         scare = true;
        //     }
            OBshader.use();
            OBshader.setInt("material.diffuse",0);  //设置采样器纹理属于哪个纹理单元
            OBshader.setInt("material.specular",1);
            lightPosition.x = sin(glfwGetTime());
            lightPosition.z = 5*cos(glfwGetTime());
            OBshader.setVec3("light.lightPos",lightPosition);
            OBshader.setFloat("light.constant",  1.0f);
            OBshader.setFloat("light.linear",    0.045f);
            OBshader.setFloat("light.quadratic", 0.0075f);
            glm::vec3 lightColor;
            lightColor.x = 0.99f+sin(0.7*glfwGetTime());
            lightColor.y = 0.99f+sin(0.9*glfwGetTime());
            lightColor.z = 0.99f+sin(1.5*glfwGetTime());

            glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // 降低影响
            glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // 很低的影响
            OBshader.setFloat("matrixlight", (1.0+sin(glfwGetTime()))/2+0.5);
            OBshader.setVec3("light.ambient", ambientColor);
            OBshader.setVec3("light.diffuse", diffuseColor);
            OBshader.setVec3("light.ambient",  0.2f, 0.2f, 0.2f);
            OBshader.setVec3("light.diffuse",  0.5f, 0.5f, 0.5f); // 将光照调暗了一些以搭配场景
            OBshader.setVec3("light.specular", 1.0f, 1.0f, 1.0f); 
            OBshader.setVec3("material.ambient",  1.0f, 0.5f, 0.31f);
            OBshader.setVec3("material.diffuse",  1.0f, 0.5f, 0.31f);
            OBshader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
            OBshader.setFloat("material.shininess", 32.0f);
            for(int i=0;i<10;++i){
                glm::mat4 model(1.0f);
                model = glm::translate(model,cubePositions[i]);
                float angle = 20.0f*(i+1) ; 
                model = glm::rotate(model, float(glfwGetTime())*glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                glm::mat3 faxianjuzhen = glm::transpose(glm::inverse(model));
                OBshader.setMat3("faxianjuzhen",faxianjuzhen); 
                OBshader.setMat4("model", model);
                OBshader.setVec3("camera.viewPos",cameraPos);
                OBshader.setFloat("camera.cutOff",cos(glm::radians(4.5f)));
                OBshader.setFloat("camera.outerCutoff",cos(glm::radians(5.5f)));
                OBshader.setVec3("camera.direction",cameraFront);
                glm::mat4 view;
                //glm::LookAt函数需要一个位置、目标和上向量,创建一个观察矩阵
                view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
                OBshader.setMat4("view",view);
                glm::mat4 projection(1.0f);
                projection = glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.0f);
                OBshader.setMat4("projection",projection);
                glBindVertexArray(objectVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
            }
            lightshader.use();
            glm::mat4 model(1.0f);
            model = glm::translate(model,lightPosition);
            model = glm::scale(model, glm::vec3(0.9f));
            glm::mat4 view;
            //glm::LookAt函数需要一个位置、目标和上向量,创建一个观察矩阵
            view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
            glm::mat4 projection(1.0f);
            projection = glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.0f);
            lightshader.setMat4("model",model);
            lightshader.setMat4("view",view);
            lightshader.setMat4("projection",projection);
            lightshader.setVec3("lightColor",lightColor);
            lightshader.setFloat("matrixmove", (glfwGetTime()));
            lightshader.setInt("ourTexture",3);
            glBindVertexArray(lightVAO);
            
            glDrawArrays(GL_TRIANGLES,0,36);
        // }
        // else{
        //     thshader.use();
        //     thshader.setInt("th",2);
        //     glBindVertexArray(thVAO);
        //     GLCall(glDrawArrays(GL_TRIANGLES,0,6));
        // }
        // 检查并调用事件，交换缓冲
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        glfwPollEvents();    
        glfwSwapBuffers(window);
    }
    }
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow *window)
{   
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window,GLFW_KEY_UP)==GLFW_PRESS&&a<=1.0f){
        a+=speed;
    }
    if(glfwGetKey(window,GLFW_KEY_DOWN)==GLFW_PRESS&&a>=0.0f){
        a-=speed;
    }
    float cameraSpeed = 2.0f*deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    if(firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.03f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw   += xoffset;
    pitch += yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if(fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if(fov <= 1.0f)
        fov = 1.0f;
    if(fov >= 45.0f)
        fov = 45.0f;
}