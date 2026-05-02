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

// 默认摄像机参数
const float YAW = -90.0f; 
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera{
public:
    // ========== 核心属性 ==========

    // 位置向量 - 摄像机在世界中的位置
    glm::vec3 position;
    
    // 朝向向量 - 摄像机看向哪（归一化）
    glm::vec3 front;

    // 上向量 - 摄像机的上方向（由 front 和 right 叉乘得到）
    glm::vec3 up;

    // 右向量 - 摄像机的右方向（由 front 和世界 up 叉乘得到）
    glm::vec3 right;

    // 世界坐标系的上方向（通常是（0,1,0））
    glm::vec3 worldUp;

    // ========== 欧拉角 (Euler Angles) ==========

    // Yaw（偏航角） - 绕世界 y 轴旋转的角度
    // 0 = 朝向 +X ， 90 = 朝向 +Z，-90 = 朝向 -Z等
    float yaw;

    // Pitch（俯仰角） - 绕世界 x 轴旋转的角度
    // 0 = 水平， += 向上看， -= 向下看
    float pitch;

    // ========== 可配置参数 ==========

    // 移动速度
    float movementSpeed;

    // 鼠标灵敏度
    float mouseSensitivity;
    
    // 缩放（FOV 角度，用于投影矩阵，非 view）
    float zoom;

    float getZoom() const;

    glm::mat4 getViewMatrix() const;

    void processKeyboard(CameraMovement direction, float deltaTime);

    void processMouseMovement(float xoffset, float yoffset,bool constrainPitch = true);

    void processMouseScroll(float yoffset);
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH
    );

private:
    void updateCameraVectors();   
};
#endif