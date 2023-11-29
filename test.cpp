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
#include"shader.h"
#include"indexBuffer.h"
#include"vertexBuffer.h"
#include"GLError.h"
#include"texture.h"

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
bool scare =false;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  6.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

void processInput(GLFWwindow* window, Shader& shader);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

//当用户改变窗口的大小的时候，视口也应该被调整。我们可以对窗口注册一个回调函数(Callback Function)，它会在每次窗口大小被调整的时候被调用
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/*  输入控制
glfwGetKey函数，它需要一个窗口以及一个按键作为输入。返回按键是否被按下
检测esc键是否被按下，如果是，关闭窗口
*/



int main()
{   /* Initialize the library */
/*调用glfwInit函数来初始化GLFW，然后我们可以使用glfwWindowHint函数来配置GLFW窗口
*/
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /*把箱子在每个轴都缩放到0.5倍，然后沿z轴旋转90度。GLM希望它的角度是弧度制的(Radian)，所以我们使用glm::radians将角度转化为弧度。
    注意有纹理的那面矩形是在XY平面上的，所以我们需要把它绕着z轴旋转。
    因为我们把这个矩阵传递给了GLM的每个函数，GLM会自动将矩阵相乘，返回的结果是一个包括了多个变换的变换矩阵
    */
    // glm::mat4 trans(1.0f) ;
    // trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));


    //创建窗口对象，glfwCreateWindow函数需要窗口的宽和高作为它的前两个参数。第三个参数表示这个窗口的名称
    {   const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "hello", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glViewport(0, 0, 800, 600);  //glViewport函数前两个参数控制窗口左下角的位置。第三个和第四个参数控制渲染窗口的宽度和高度（像素）
    
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
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
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
//     float vertices[] = {
// //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
//      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // 右上
//      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // 右下
//     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
//     -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // 左上
// };
    unsigned int indices[] = {
        0,1,2,
        0,2,3
    };

    float texCoords[] = {
    0.0f, 0.0f, // 左下角
    1.0f, 0.0f, // 右下角
    0.5f, 1.0f // 上中
};
    float thVertex[] = {
            -1.0f,1.0f,0.0f,0.0f,1.0f,
            -1.0f,-1.0f,0.0f,0.0f,0.0f,
            1.0f,1.0f,0.0f,1.0f,1.0f,
            1.0f,1.0f,0.0f,1.0f,1.0f,
            -1.0f,-1.0f,0.0f,0.0f,0.0f,
            1.0f,-1.0f,0.0f,1.0f,0.0f

    };
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // unsigned int VBO;
    // glGenBuffers(1, &VBO);  //生成缓冲对象
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);  //绑定缓冲区
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    VertexBuffer VBO(vertices,sizeof(vertices));

    
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3* sizeof(float)));
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    unsigned int thVAO;
    GLCall(glGenVertexArrays(1, &thVAO));
    GLCall(glBindVertexArray(thVAO));
    GLCall(VertexBuffer thVBO(thVertex,sizeof(thVertex)));
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)0));
    GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5* sizeof(float), (void*)(3*sizeof(float))));
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glEnableVertexAttribArray(1));
    // unsigned int IBO;
    // glGenBuffers(1,&IBO);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    IndexBuffer IBO(indices,6);
    /*
    glBufferData是一个专门用来把用户定义的数据复制到当前绑定缓冲的函数。它的第一个参数是目标缓冲的类型,
    第二个参数指定传输数据的大小(以字节为单位)；第三个参数是我们希望发送的实际数据,
    第四个参数指定了我们希望显卡如何管理给定的数据。它有三种形式：
        GL_STATIC_DRAW ：数据不会或几乎不会改变。
        GL_DYNAMIC_DRAW：数据会被改变很多。
        GL_STREAM_DRAW ：数据每次绘制时都会改变。
    */
    
    /*第一个参数指定我们要配置的顶点属性。还记得我们在顶点着色器中使用layout(location = 0)定义了position顶点属性的位置值(Location)吗
第二个参数指定顶点属性的大小。顶点属性是一个vec3，它由3个值组成，所以大小是3。
第三个参数指定数据的类型，这里是GL_FLOAT(GLSL中vec*都是由浮点数值组成的)。
下个参数定义我们是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。
第五个参数叫做步长(Stride)，它告诉我们在连续的顶点属性组之间的间隔。*/

    
    // glEnableVertexAttribArray(2);


    //生成纹理
    unsigned int textures[2];
    glGenTextures(2,textures);
    glActiveTexture(GL_TEXTURE0); // 在绑定纹理之前先激活纹理单元
    glBindTexture(GL_TEXTURE_2D,textures[0]);
    //设置环绕方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    //设置边缘颜色
    float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    //设置过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //多级渐远纹理
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    //加载纹理图片
    int width1, height1, nrChannels1;
    int width2, height2, nrChannels2;
    unsigned char *data1 = stbi_load("textures/container.jpg", &width1, &height1, &nrChannels1, 0);
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data2 = stbi_load("textures/awesomeface.png", &width2, &height2, &nrChannels2, 0);
    if (data1)
    {
        /*
    第一个参数指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理
    （任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
    第二个参数为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。
    第三个参数告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。
    第四个和第五个参数设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
    下个参数应该总是被设为0（历史遗留的问题）。
    第七第八个参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。
    最后一个参数是真正的图像数据。
    */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
    glGenerateMipmap(GL_TEXTURE_2D);

    }
    else
    {
    std::cout << "Failed to load texture" << "/n";
    }
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,textures[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    //设置边缘颜色
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    //设置过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    //多级渐远纹理
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data2)
    {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
    glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
    std::cout << "Failed to load texture" << "/n";
    }
    //释放内存
    stbi_image_free(data1);
    stbi_image_free(data2);

    // unsigned int vertexShader;
    // vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // glCompileShader(vertexShader);

    // unsigned int fragmentShader;
    // fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    // glCompileShader(fragmentShader);
    // int  success;
    // char infoLog[512];
    // glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // if(!success)
    // {
    //     glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    //     std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    // }

    // unsigned int shaderProgram;
    // shaderProgram = glCreateProgram();
    // glAttachShader(shaderProgram, vertexShader);
    // glAttachShader(shaderProgram, fragmentShader);
    // glLinkProgram(shaderProgram);

    // glUseProgram(shaderProgram);
    // glDeleteShader(vertexShader);
    // glDeleteShader(fragmentShader);
    Texture lightTexture("res/shaders/th.jpg",GL_TEXTURE2,GL_RGB);
    GLCall(Shader shader("res/shaders/vertex.shader","res/shaders/fragment.shader"));
    GLCall(Shader thshader("res/shaders/th.vs","res/shaders/th.fs"));
    shader.use();
    
    //声明采样器属于哪个纹理，设置uniform变量
    glUniform1i(glGetUniformLocation(shader.get_id(), "ourTexture1"), 0); // 手动设置
    shader.setInt("ourTexture2", 1); // 或者使用着色器类设置
    glEnable(GL_DEPTH_TEST);

    //设置摄像机位置
    // glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    // //摄像机朝向（z轴）
    // glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    // //观察坐标系x轴
    // glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
    // glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    // //y轴
    // glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
    //lookat矩阵
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //渲染循环
    while(!glfwWindowShouldClose(window))
    {   
        processInput(window,shader);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

        // int colorLocation = glGetUniformLocation(shader.get_id(),"ourColor");
        // float timeValue = glfwGetTime();
        // float greenValue = sin(timeValue) / 2.0f + 0.5f;
        // float redValue = sin(timeValue) / 2.0f + 0.5f;
        // glUniform4f(colorLocation,1.0f-redValue,greenValue,0.0f,0.0f);
        if(!scare){
        shader.use();
        if(sin(0.09*glfwGetTime())>0.9f){
                scare = true;
            }
        for(unsigned int i = 0; i < 10; i++)
        {
        glm::mat4 model(1.0f);
        cubePositions[i].y-=0.001f;
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f*(i+1) ; 
        model = glm::rotate(model, float(glfwGetTime())*glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shader.setMat4("model", model);
        // glm::mat4 view(1.0f);
        // // 注意，我们将矩阵向我们要进行移动场景的反方向移动。
        // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        
        glm::mat4 view;
        //glm::LookAt函数需要一个位置、目标和上向量,创建一个观察矩阵
        view = glm::lookAt(cameraPos, cameraPos+cameraFront, cameraUp);
        shader.setMat4("view",view);
        glm::mat4 projection(1.0f);
        projection = glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.0f);
        shader.setMat4("projection",projection);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        }
        else{
            glBindVertexArray(thVAO);
            thshader.use();
            thshader.setInt("th",2);
            GLCall(glDrawArrays(GL_TRIANGLES,0,6));
        }
        
        // glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        // glDrawArrays(GL_TRIANGLES,0,36);
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


void processInput(GLFWwindow *window,Shader& shader)
{   
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window,GLFW_KEY_UP)==GLFW_PRESS&&a<=1.0f){
        a+=speed;
    }
    if(glfwGetKey(window,GLFW_KEY_DOWN)==GLFW_PRESS&&a>=0.0f){
        a-=speed;
    }
    shader.setFloat("kejiandu",a);
    float cameraSpeed = 1.0f*deltaTime; // adjust accordingly
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

    float sensitivity = 0.05f;
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