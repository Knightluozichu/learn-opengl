// 复习 day02
// day03 GLSL 着色器语言

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
using namespace std;

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// const char *vertexShaderSource = R"(
// #version 330 core
// layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec3 aColor;
// out vec3 ourColor;
// void main()
// {
//     gl_Position = vec4(aPos, 1.0f);
//     ourColor = aColor;
// })";

// const char *fragmentShaderSourceOrange = R"(
// #version 330 core
// uniform vec3 uW;
// out vec4 fragColor;
// in vec3 ourColor;
// void main()
// {
//     vec3 c1 = ourColor;
//     vec3 c2 = vec3(ourColor.b, ourColor.r, ourColor.g);
//     vec3 c3 = vec3(ourColor.g, ourColor.b, ourColor.r); 
//     vec3 mixed = uW.x*c1 + uW.y*c2 + uW.z*c3;
//     fragColor = vec4(mixed, 1.0);
// }
// )";



int main()
{
    // 初始化及配置 glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    #if __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    
    // 创建窗口
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH,SCR_HEIGHT,"shader practicle",
    nullptr, nullptr);
    if(NULL == window)
    {
        cout << "创建窗口失败！" <<endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

    // 初始化 glad，加载所有 OPENGL 函数指针
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "glad 初始化失败！" << endl;
        return -1;
    }

    cout << "opengl 版本：" << glGetString(GL_VERSION) << endl;
    cout << "glsl 版本：" << glGetString(GL_SHADING_LANGUAGE_VERSION) <<endl;

    // 工程化约定：构建后 CMake 会把 src/day03/shaders/ 复制到 build/shaders/day03/
    // 所以运行时从 build 目录启动，可用下面的相对路径读取
    Shader ourShader("shaders/day03/vertex.glsl", "shaders/day03/fragment.glsl");

    // // 创建顶点着色器
    // unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertexShader,1,&vertexShaderSource,nullptr);
    // glCompileShader(vertexShader);
    // int success;
    // char infoLog[512];
    // glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // if(!success)
    // {
    //     glGetShaderInfoLog(vertexShader,512, nullptr,infoLog);
    //     cout << "顶点着色器编译失败：" << infoLog << endl;
    //     return -1;
    // }

    // // 创建片段着色器
    // unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragmentShader,1,&fragmentShaderSourceOrange,nullptr);
    // glCompileShader(fragmentShader);
    // glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    // if(!success)
    // {
    //     glGetShaderInfoLog(fragmentShader,512,nullptr,infoLog);
    //     cout << "片段着色器编译失败：" << infoLog << endl;
    //     return -1;
    // }

    // // 创建着色器程序
    // unsigned int shaderProgram = glCreateProgram();
    // glAttachShader(shaderProgram,vertexShader);
    // glAttachShader(shaderProgram, fragmentShader);
    // glLinkProgram(shaderProgram);
    // glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    // if(!success)
    // {
    //     glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
    //     cout << "着色器程序链接失败：" << infoLog << endl;
    //     return -1;
    // }

    // // 删除不再需要的着色器
    // glDeleteShader(vertexShader);
    // glDeleteShader(fragmentShader);

    // 设置顶点数据
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,    1.0f,0.0f,0.0f,
        0.5f, -0.5f, 0.0f,     0.0f,1.0f,0.0f,
        0.0f, 0.5f, 0.0f,      0.0f,0.0f,1.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
    };
    unsigned int VBO,VAO,EBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);
    glGenBuffers(1,&EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
    
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f,0.3f,0.3f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glUseProgram(shaderProgram);
        ourShader.use();
        float t = (float)glfwGetTime() * 3.0f; // 3倍速度（你想快就乘更大）

        float a = 0.5f + 0.5f * sinf(t);
        float b = 0.5f + 0.5f * sinf(t + 2.09439510239f); // 2π/3
        float c = 0.5f + 0.5f * sinf(t + 4.18879020479f); // 4π/3
    
        float s = a + b + c;
        a /= s; b /= s; c /= s;

        // int uWloc = glGetUniformLocation(shaderProgram, "uW");
        // glUniform3f(uWloc, a,b,c);

        ourShader.setVec3("uW", a,b,c);
        ourShader.setFloat("vMoveX", 0.5f);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_INT,0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&VAO);
    glDeleteBuffers(1,&VBO);
    glDeleteBuffers(1,&EBO);
    // glDeleteProgram(shaderProgram);
    glfwTerminate();
    
    return 0;
}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0,0,width, height);
}