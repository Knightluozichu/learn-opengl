
#include <cmath>
#include "camera.h"

// ========== 构造函数 ==========
Camera::Camera(
    glm::vec3 position,
    glm::vec3 up,
    float yaw,
    float pitch
): front(glm::vec3(0.0f, 0.0f, -1.0f)),
   movementSpeed(SPEED),
   mouseSensitivity(SENSITIVITY),
   zoom(ZOOM){
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    updateCameraVectors(); // 根据 yaw/pitch 计算 front, right, up 向量
}

// ========== 对外接口：核心能力 ==========

// 获取 View 矩阵（用于传给 shader）
// 内部使用 glm::lookAt ，参数是当前 position 和 position+front
glm::mat4 Camera::getViewMatrix() const{
    return glm::lookAt(position, position + front, up);
}

// 获取投影矩阵（方便统一管理，可选）
float Camera::getZoom() const{return zoom;}

// ========== 对外接口：输入处理 ==========

// 处理键盘输入 （WASD）
// direction：FORWARD, BACKWARD, LEFT, RIGHT
// deltaTime：帧时间，用于计算实际移动距离（速度*时间）
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

// 处理鼠标移动（改变视角）
// xoffset，yoffset：鼠标相对于上帧的位移（像素）
// constrainPitch：是否限制 pitch 在[-89, 89] 之间
void Camera::processMouseMovement(float xoffset, float yoffset,bool constrainPitch){
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
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);

    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}