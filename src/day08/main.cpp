#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Common/shader.h"
#include "Common/camera.h"

using namespace std;

void processInput(GLFWwindow *window, Camera &camera, float deltaTime);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

const unsigned int SCR_WIDTH  = 800;
const unsigned int SCR_HEIGHT = 600;

static bool firstMouse = true;
static float lastX = SCR_WIDTH  / 2.0f;
static float lastY = SCR_HEIGHT / 2.0f;

bool cursorDisabled = true;
static bool tabWasPressed = false;

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (!cursorDisabled) return;
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }
    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    Camera *camera = (Camera *)glfwGetWindowUserPointer(window);
    if (camera)
        camera->processMouseMovement(xoffset, yoffset);
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(
        SCR_WIDTH, SCR_HEIGHT, "Day 08: Colors", nullptr, nullptr);
    if (NULL == window)
    {
        cout << "GLFW window creation failed!" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "glad initialization failed!" << endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // 两个 shader 程序
    Shader lightingShader("shaders/day08/lighting.vs", "shaders/day08/lighting.fs");
    Shader lightCubeShader("shaders/day08/light_cube.vs", "shaders/day08/light_cube.fs");

    // --- 顶点数据：只有位置（3D），本轮还没有法线 ---
    float vertices[] = {
        // Back face (z = -0.5)
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        // Front face (z = +0.5)
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        // Left face (x = -0.5)
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        // Right face (x = +0.5)
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        // Bottom face (y = -0.5)
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,
        // Top face (y = +0.5)
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
    };

    // 光源位置
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    unsigned int VBO, cubeVAO, lightVAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &cubeVAO);
    glGenVertexArrays(1, &lightVAO);

    // 1) 上传顶点数据（一次）
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 2) 配置 cubeVAO：被照射物体
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 3) 配置 lightVAO：光源（共享同一个 VBO）
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
    glfwSetWindowUserPointer(window, &camera);
    float lastTime = 0.0f;
    float deltaTime = 0.0f;

    // 颜色
    glm::vec3 objectColor(1.0f, 0.5f, 0.31f);  // 珊瑚色
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);    // 白光

    while (!glfwWindowShouldClose(window))
    {
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        processInput(window, camera, deltaTime);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view       = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.getZoom()),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // ---- 1) 画被照射的立方体 ----
        lightingShader.use();
        lightingShader.setVec3("objectColor", objectColor.x, objectColor.y, objectColor.z);
        lightingShader.setVec3("lightColor",  lightColor.x,  lightColor.y,  lightColor.z);
        lightingShader.setMat4("view",        glm::value_ptr(view));
        lightingShader.setMat4("projection",  glm::value_ptr(projection));

        glm::mat4 model(1.0f);
        lightingShader.setMat4("model", glm::value_ptr(model));
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // ---- 2) 画光源立方体 ----
        lightCubeShader.use();
        lightCubeShader.setMat4("view",       glm::value_ptr(view));
        lightCubeShader.setMat4("projection", glm::value_ptr(projection));

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightCubeShader.setMat4("model", glm::value_ptr(model));
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window, Camera &camera, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, deltaTime);

    int tabState = glfwGetKey(window, GLFW_KEY_TAB);
    if (tabState == GLFW_PRESS && !tabWasPressed)
    {
        if (cursorDisabled)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cursorDisabled = false;
        }
        else
        {
            firstMouse = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            cursorDisabled = true;
        }
    }
    tabWasPressed = (tabState == GLFW_PRESS);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
