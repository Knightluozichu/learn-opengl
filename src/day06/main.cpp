#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "texture.h"

using namespace std;

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                          "Day 06: Coordinate Systems", nullptr, nullptr);
    if (NULL == window)
    {
        cout << "GLFW window creation failed!" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 初始化 glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "glad initialization failed!" << endl;
        return -1;
    }

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    Shader ourShader("shaders/day06/vertex.glsl", "shaders/day06/fragment.glsl");
    Texture texture1("assets/day06/awesomeface.png");

    // 立方体顶点 (36个顶点, 6个面 * 2个三角形 * 3个顶点)
    // 包含 3D 坐标 (x,y,z) 和 纹理坐标 (x,y)
    // EBO 优化：24个顶点 (6面 * 4点) + 36个索引
    float vertices[] = {
        // Back face (z = -0.5)
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // 0 Bottom-Left
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // 1 Bottom-Right
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 2 Top-Right
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 3 Top-Left
        // Front face (z = +0.5)
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 4 Bottom-Left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 5 Bottom-Right
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // 6 Top-Right
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // 7 Top-Left
        // Left face (x = -0.5)
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 8 Top-Right
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 9 Top-Left
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 10 Bottom-Left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 11 Bottom-Right
        // Right face (x = +0.5)
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 12 Top-Left
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 13 Top-Right
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 14 Bottom-Right
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 15 Bottom-Left
        // Bottom face (y = -0.5)
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // 16 Top-Right
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // 17 Top-Left
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // 18 Bottom-Left
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // 19 Bottom-Right
        // Top face (y = +0.5)
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // 20 Top-Left
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // 21 Top-Right
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // 22 Bottom-Right
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // 23 Bottom-Left
    };

    unsigned int indices[] = {
        0, 1, 2, 2, 3, 0,       // Back
        4, 5, 6, 6, 7, 4,       // Front
        8, 9, 10, 10, 11, 8,    // Left
        12, 13, 14, 14, 15, 12, // Right
        16, 17, 18, 18, 19, 16, // Bottom
        20, 21, 22, 22, 23, 20  // Top
    };

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 绑定 EBO (必须在 bind VAO 之后)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 属性0: 坐标 (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // 属性1: 纹理 (vec2)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    ourShader.use();
    // 告诉 shader 采样器属于纹理单元 0
    ourShader.setInt("texture1", 0);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // 清除颜色缓冲 AND 深度缓冲
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1.ID);
        ourShader.use();

        // 创建 MVP 矩阵
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);

        // 1. Model: 绕 (1, 0.3, 0.5) 轴随时间旋转
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // 2. View: 摄像机向后移 (即场景向前移 -3)
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // 3. Projection: 透视投影, FOV 45度
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // 传给 Shader
        ourShader.setMat4("model", glm::value_ptr(model));
        ourShader.setMat4("view", glm::value_ptr(view));
        ourShader.setMat4("projection", glm::value_ptr(projection));

        // 绘制立方体
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
