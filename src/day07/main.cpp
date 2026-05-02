#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <random>

#include "Common/shader.h"
#include "Common/texture.h"
#include "Common/camera.h"

using namespace std;

void processInput(GLFWwindow *window, Camera &camera, float deltaTime);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

static bool firstMouse = true;
static float lastX = SCR_WIDTH / 2.0f;
static float lastY = SCR_HEIGHT / 2.0f;

bool cursorDisabled = true;
static bool tabWasPressed = false;

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
  if (!cursorDisabled)
    return;
  if (firstMouse)
  {
    lastX = (float)xpos;
    lastY = (float)ypos;
    firstMouse = false;
  }
  float xoffset = (float)xpos - lastX;
  float yoffset = lastY - (float)ypos; // 屏幕 y 向上，OpenGL y 向下
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
      SCR_WIDTH, SCR_HEIGHT, "Day 07: Camera Systems", nullptr, nullptr);
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

  Shader ourShader("shaders/day07/vertex.glsl", "shaders/day07/fragment.glsl");
  Texture texture1("assets/day07/clangd.png");

  float vertices[] = {
      // Back face (z = -0.5)
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, // 0 Bottom-Left
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f,  // 1 Bottom-Right
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // 2 Top-Right
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,  // 3 Top-Left
      // Front face (z = +0.5)
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, // 4 Bottom-Left
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,  // 5 Bottom-Right
      0.5f, 0.5f, 0.5f, 1.0f, 1.0f,   // 6 Top-Right
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,  // 7 Top-Left
      // Left face (x = -0.5)
      -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // 8 Top-Right
      -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // 9 Top-Left
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // 10 Bottom-Left
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // 11 Bottom-Right
                                       // Right face (x = +0.5)
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,    // 12 Top-Left
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,   // 13 Top-Right
      0.5f, -0.5f, -0.5f, 0.0f, 1.0f,  // 14 Bottom-Right
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f,   // 15 Bottom-Left
      // Bottom face (y = -0.5)
      -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, // 16 Top-Right
      0.5f, -0.5f, -0.5f, 1.0f, 1.0f,  // 17 Top-Left
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f,   // 18 Bottom-Left
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,  // 19 Bottom-Right
      // Top face (y = +0.5)
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, // 20 Top-Left
      0.5f, 0.5f, -0.5f, 1.0f, 1.0f,  // 21 Top-Right
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f,   // 22 Bottom-Right
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f   // 23 Bottom-Left
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
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // 属性0: 坐标 (vec3)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  // 属性1: 纹理 (vec2)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void *)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  ourShader.use();
  // 告诉 shader 采样器属于纹理单元 0
  ourShader.setInt("texture1", 0);

  std::vector<glm::vec3> cubePositions;
  cubePositions.reserve(100);

  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_real_distribution<float> distXZ(-10.0f, 10.0f);
  std::uniform_real_distribution<float> distY(-10.0f, 10.0f);

  for (int i = 0; i < 100; ++i)
  {
    float x = distXZ(rng);
    float y = distY(rng);
    float z = distXZ(rng);
    cubePositions.emplace_back(x, y, z);
  }

  Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
  glfwSetWindowUserPointer(window, &camera);
  float lastTime = 0.0f;
  float deltaTime = 0.0f;

  while (!glfwWindowShouldClose(window))
  {

    float currentTime = (float)glfwGetTime();
    deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    processInput(window, camera, deltaTime);

    // 清除颜色缓冲 AND 深度缓冲
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1.ID);
    ourShader.use();

    // 创建 MVP 矩阵
    // glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    // 1. Model: 绕 (1, 0.3, 0.5) 轴随时间旋转
    // model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f),
    //                     glm::vec3(0.5f, 1.0f, 0.0f));

    // 2. View: 摄像机向后移 (即场景向前移 -3)
    // view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    view = camera.getViewMatrix();

    // 3. Projection: 透视投影, FOV 45度
    // projection =
    //     glm::perspective(glm::radians(45.0f),
    //                      (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    projection = glm::perspective(glm::radians(camera.getZoom()),
                                  (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    // 传给 Shader
    // ourShader.setMat4("model", glm::value_ptr(model));
    ourShader.setMat4("view", glm::value_ptr(view));
    ourShader.setMat4("projection", glm::value_ptr(projection));

    // 绘制立方体
    glBindVertexArray(VAO);
    // glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

    // 对 100 个位置循环：每个都有自己的 model
    for (size_t i = 0; i < cubePositions.size(); ++i)
    {
      glm::mat4 model = glm::mat4(1.0f);

      // 先平移到该立方体的位置
      model = glm::translate(model, cubePositions[i]);

      // 再做一点旋转，让每个转得不太一样
      float angle = (float)glfwGetTime() * glm::radians(30.0f) + (float)i;
      model = glm::rotate(model, angle, glm::vec3(0.5f, 1.0f, 0.0f));

      ourShader.setMat4("model", glm::value_ptr(model));
      glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glfwTerminate();
  return 0;
}

// void processInput(GLFWwindow *window) {
//   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//     glfwSetWindowShouldClose(window, true);
//   }
// }

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
    // 这里才真正切换
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
