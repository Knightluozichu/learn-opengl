#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum CameraMovement{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera{
public:
    // 核心属性
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float movementSpeed;
    float mouseSensitivity;
    float zoom;
    
    float getZoom() const;
    glm::mat4 getViewMatrix() const;
    void processKeyboard(CameraMovement direction, float deltaTime);
    void processMouseMovement(float xoffset,float yoffset,bool constrainPitch = true);
    void processMouseScroll(float yoffset);
    
    Camera(
        glm::vec3 position = glm::vec3(0.0f,0.0f,0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH
    );

private:
    void updateCameraVectors();
};
#endif