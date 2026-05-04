#include <cmath>
#include "camera.h"

Camera::Camera(
    glm::vec3 position,
    glm::vec3 up,
    float yaw,
    float pitch
): front(glm::vec3(0.0f,0.0f,-1.0f)),
   movementSpeed(SPEED),
   mouseSensitivity(SENSITIVITY),
   zoom(ZOOM){
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const{
    return glm::lookAt(position, position+front,up);
}

float Camera::getZoom() const{return zoom;}

void Camera::processKeyboard(CameraMovement direction, float deltaTime){
    float velocity = movementSpeed * deltaTime;
    if(direction == FORWARD)
        position += front * velocity;
    if(direction == BACKWARD)
        position -= front * velocity;
    if(direction == LEFT)
        position -= right * velocity;
    if(direction == RIGHT)
        position += right * velocity;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch){
    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if(constrainPitch){
        if(pitch > 89.0f)
            pitch = 89.0f;
        if(pitch < -89.0f)
            pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset){
    zoom -= yoffset;
    if(zoom < 1.0f)
        zoom = 1.0f;
    if(zoom > 45.0f)
        zoom = 45.0f;
}

void Camera::updateCameraVectors(){
    glm::vec3 frontVec;
    frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontVec.y = sin(glm::radians(pitch));
    frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(frontVec);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}