#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main()
{
    // 初始化及配置 g lfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建窗口
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT,
                                          "shader practicle day 04", nullptr, nullptr);
    if (NULL == window)
    {
        cout << "GLFW window creation failed!" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 初始化 glad，加载所有 OPENGL 函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "glad initialization failed!" << endl;
        return -1;
    }

    cout << "opengl version: " << glGetString(GL_VERSION) << endl;
    cout << "glsl version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    // 工程化约定：构建后 CMake 会把 src/day05/shaders/ 复制到 build/shaders/day05/
    // 所以运行时从 build 目录启动，可用下面的相对路径读取
    Shader ourShader("shaders/day05/vertex.glsl", "shaders/day05/fragment.glsl");
    Texture texture1("assets/day05/trinket_box.jpg");
    Texture texture2("assets/day05/awesomeface.png");
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, texture1.ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    // 设置顶点数据
    float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // 左下角 0
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 右下角 1
        -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,  // 顶部 2
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f    // 右上角 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        1, 2, 3};

    // 这份 vertices / indices 本身就能画出 2 个三角形（6 个索引），所以一套 VAO/VBO/EBO 就够了
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 告诉 fragment shader：ourTexture 这个 sampler2D 从纹理单元 0 采样
    ourShader.use();
    ourShader.setInt("ourTexture", 0);
    ourShader.setInt("ourTexture2", 1);

    // 下面是“重复创建第二套 VAO/VBO/EBO”的旧写法，保留注释做对照（现在不需要了）
    /*
    unsigned int VBOs[2], VAOs[2], EBOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    glGenBuffers(2, EBOs);
    // ... (同样的配置重复两遍) ...
    */

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    
        ourShader.use();
        texture1.bind(0);
        texture2.bind(1);
    
        float currentTime = (float)glfwGetTime();
        float angle = currentTime;  // 当前旋转角度（弧度）
    
        // ========== 顺序 A：先旋转再平移 ==========
        glm::mat4 transA(1.0f);
        transA = glm::rotate(transA, angle, glm::vec3(0.0f, 0.0f, 1.0f));      // 先：绕原点旋转
        transA = glm::translate(transA, glm::vec3(0.5f, -0.5f, 0.0f));         // 后：平移
    
        // 使用顺序A的变换（这样你才能看到旋转效果）
        ourShader.setMat4("transform", glm::value_ptr(transA));
    
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
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