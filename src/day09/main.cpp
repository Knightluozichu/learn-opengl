#include <cstddef>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Common/shader.h"
#include "Common/camera.h"
#include "glm/detail/type_vec.hpp"

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

void mouse_callback(GLFWwindow *window, double xpos, double ypos){
    if(!cursorDisabled) return;
    if(firstMouse)
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
    if(camera)
    {
        camera->processMouseMovement(xoffset, yoffset);
    }
};

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(
        SCR_WIDTH,SCR_HEIGHT,"Day 09: Phone Light Model",nullptr,nullptr);
    
    if(NULL == window)
    {
        cout << "GLFW window creation failed!" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
    glfwSetCursorPosCallback(window,mouse_callback);
    glfwSetInputMode(window,GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << " glad initialization failed!" << endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    Shader lightingShader("shaders/day09/lighting.vs","shaders/day09/lighting.fs");
    Shader lightCubeShader("shaders/day09/light_cube.vs","shaders/day09/light_cube.fs");

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

    glm::vec3 lightPos(1.2f,1.0f,2.0f);

    unsigned int VBO, cubeVAO, lightVAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &cubeVAO);
    glGenVertexArrays(1, &lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices,GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    Camera camera(glm::vec3(0.0f,0.0f,3.0f));
    glfwSetWindowUserPointer(window, &camera);
    float lastTime = 0.0f;
    float deltaTime = 0.0f;

    glm::vec3 objectcolor(1.0f,0.5f,0.31f);
    glm::vec3 lightColor(1.0f,1.0f,1.0f);
    float amibientStrength = 0.2f;

    while(!glfwWindowShouldClose(window))
    {
        float currentTime = (float)glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        processInput(window,camera,deltaTime);

        glClearColor(0.1f,0.1f,0.1f,0.1f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.getZoom()),
            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f,100.0f);
        
        lightingShader.use();
        lightingShader.setVec3("lightPos", lightPos.x, lightPos.y, lightPos.z);
        // lightingShader.setVec3("viewPos", , float y, float z)
        lightingShader.setFloat("amibientStrength", amibientStrength);
        lightingShader.setVec3("objectColor",objectcolor.x,objectcolor.y,objectcolor.z);
        lightingShader.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
        lightingShader.setMat4("view", glm::value_ptr(view));
        lightingShader.setMat4("projection", glm::value_ptr(projection));
        glm::mat4 model(1.0f);
        lightingShader.setMat4("model", glm::value_ptr(model));
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES,0,72);

        lightCubeShader.use();
        lightCubeShader.setMat4("view", glm::value_ptr(view));
        lightCubeShader.setMat4("projection", glm::value_ptr(projection));
        // float t = (float)glfwGetTime() * 0.5f;
        // float x = 0.5f + 0.5f * sinf(t);
        // float y = 0.5f + 0.5f * sinf(2.0f + t);
        // float z = 0.5f + 0.5f * sinf(4.0f + t);
        // lightColor = glm::vec3(x,y,z);
        // lightCubeShader.setVec3("sinTime", lightColor.x, lightColor.y, lightColor.z);
        lightCubeShader.setVec3("lightColor", lightColor.x, lightColor.y, lightColor.z);
        // lightCubeShader.setFloat("amibientStrength", amibientStrength);
        model = glm::mat4(1.0f);
        model = glm::rotate(model,(float)glfwGetTime(), glm::vec3(0.0f,1.0f,0.0f));
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));

        lightCubeShader.setMat4("model", glm::value_ptr(model));
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES,0,72);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1,&cubeVAO);
    glDeleteVertexArrays(1,&lightVAO);
    glDeleteBuffers(1,&VBO);
    return 0;
};

void processInput(GLFWwindow *window, Camera &camera, float deltaTime)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }

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