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

float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f; // ��һ֡��ʱ��

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

//���û��ı䴰�ڵĴ�С��ʱ���ӿ�ҲӦ�ñ����������ǿ��ԶԴ���ע��һ���ص�����(Callback Function)��������ÿ�δ��ڴ�С��������ʱ�򱻵���
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

/*  �������
glfwGetKey����������Ҫһ�������Լ�һ��������Ϊ���롣���ذ����Ƿ񱻰���
���esc���Ƿ񱻰��£�����ǣ��رմ���
*/



int main()
{   /* Initialize the library */
/*����glfwInit��������ʼ��GLFW��Ȼ�����ǿ���ʹ��glfwWindowHint����������GLFW����
*/
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    /*��������ÿ���ᶼ���ŵ�0.5����Ȼ����z����ת90�ȡ�GLMϣ�����ĽǶ��ǻ����Ƶ�(Radian)����������ʹ��glm::radians���Ƕ�ת��Ϊ���ȡ�
    ע��������������������XYƽ���ϵģ�����������Ҫ��������z����ת��
    ��Ϊ���ǰ�������󴫵ݸ���GLM��ÿ��������GLM���Զ���������ˣ����صĽ����һ�������˶���任�ı任����
    */
    // glm::mat4 trans(1.0f) ;
    // trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    // trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));


    //�������ڶ���glfwCreateWindow������Ҫ���ڵĿ�͸���Ϊ����ǰ����������������������ʾ������ڵ�����
    {   const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "hello", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glViewport(0, 0, 800, 600);  //glViewport����ǰ�����������ƴ������½ǵ�λ�á��������͵��ĸ�����������Ⱦ���ڵĿ�Ⱥ͸߶ȣ����أ�
    
    glfwMakeContextCurrent(window);
    //��ʼ��glew
    if(glewInit()!=GLEW_OK) {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    //ע��ص��������ڴ�������֮����Ⱦѭ����ʼ��֮ǰע����Щ�ص�����
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
// //     ---- λ�� ----       ---- ��ɫ ----     - �������� -
//      0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // ����
//      0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // ����
//     -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // ����
//     -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // ����
// };
    unsigned int indices[] = {
        0,1,2,
        0,2,3
    };

    float texCoords[] = {
    0.0f, 0.0f, // ���½�
    1.0f, 0.0f, // ���½�
    0.5f, 1.0f // ����
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
    // glGenBuffers(1, &VBO);  //���ɻ������
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);  //�󶨻�����
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
    glBufferData��һ��ר���������û���������ݸ��Ƶ���ǰ�󶨻���ĺ��������ĵ�һ��������Ŀ�껺�������,
    �ڶ�������ָ���������ݵĴ�С(���ֽ�Ϊ��λ)������������������ϣ�����͵�ʵ������,
    ���ĸ�����ָ��������ϣ���Կ���ι�����������ݡ�����������ʽ��
        GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
        GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
        GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣
    */
    
    /*��һ������ָ������Ҫ���õĶ������ԡ����ǵ������ڶ�����ɫ����ʹ��layout(location = 0)������position�������Ե�λ��ֵ(Location)��
�ڶ�������ָ���������ԵĴ�С������������һ��vec3������3��ֵ��ɣ����Դ�С��3��
����������ָ�����ݵ����ͣ�������GL_FLOAT(GLSL��vec*�����ɸ�����ֵ��ɵ�)��
�¸��������������Ƿ�ϣ�����ݱ���׼��(Normalize)�������������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮�䡣
�����������������(Stride)�������������������Ķ���������֮��ļ����*/

    
    // glEnableVertexAttribArray(2);


    //��������
    unsigned int textures[2];
    glGenTextures(2,textures);
    glActiveTexture(GL_TEXTURE0); // �ڰ�����֮ǰ�ȼ�������Ԫ
    glBindTexture(GL_TEXTURE_2D,textures[0]);
    //���û��Ʒ�ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    //���ñ�Ե��ɫ
    float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    //���ù��˷�ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //�༶��Զ����
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    //��������ͼƬ
    int width1, height1, nrChannels1;
    int width2, height2, nrChannels2;
    unsigned char *data1 = stbi_load("textures/container.jpg", &width1, &height1, &nrChannels1, 0);
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data2 = stbi_load("textures/awesomeface.png", &width2, &height2, &nrChannels2, 0);
    if (data1)
    {
        /*
    ��һ������ָ��������Ŀ��(Target)������ΪGL_TEXTURE_2D��ζ�Ż������뵱ǰ�󶨵����������ͬһ��Ŀ���ϵ�����
    ���κΰ󶨵�GL_TEXTURE_1D��GL_TEXTURE_3D���������ܵ�Ӱ�죩��
    �ڶ�������Ϊ����ָ���༶��Զ����ļ��������ϣ�������ֶ�����ÿ���༶��Զ����ļ���Ļ�������������0��Ҳ���ǻ�������
    ��������������OpenGL����ϣ����������Ϊ���ָ�ʽ�����ǵ�ͼ��ֻ��RGBֵ���������Ҳ��������ΪRGBֵ��
    ���ĸ��͵���������������յ�����Ŀ�Ⱥ͸߶ȡ�����֮ǰ����ͼ���ʱ�򴢴������ǣ���������ʹ�ö�Ӧ�ı�����
    �¸�����Ӧ�����Ǳ���Ϊ0����ʷ���������⣩��
    ���ߵڰ˸�����������Դͼ�ĸ�ʽ���������͡�����ʹ��RGBֵ�������ͼ�񣬲������Ǵ���Ϊchar(byte)���飬���ǽ��ᴫ���Ӧֵ��
    ���һ��������������ͼ�����ݡ�
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
    //���ñ�Ե��ɫ
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    //���ù��˷�ʽ
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    //�༶��Զ����
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
    //�ͷ��ڴ�
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
    
    //���������������ĸ���������uniform����
    glUniform1i(glGetUniformLocation(shader.get_id(), "ourTexture1"), 0); // �ֶ�����
    shader.setInt("ourTexture2", 1); // ����ʹ����ɫ��������
    glEnable(GL_DEPTH_TEST);

    //���������λ��
    // glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    // //���������z�ᣩ
    // glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
    // //�۲�����ϵx��
    // glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f); 
    // glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
    // //y��
    // glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
    //lookat����
    
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    //��Ⱦѭ��
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
        // // ע�⣬���ǽ�����������Ҫ�����ƶ������ķ������ƶ���
        // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        
        glm::mat4 view;
        //glm::LookAt������Ҫһ��λ�á�Ŀ���������,����һ���۲����
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
        // ��鲢�����¼�����������
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