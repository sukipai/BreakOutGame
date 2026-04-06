#pragma once

#include <glm/fwd.hpp>
#include <glm/glm.hpp>

class Camera {
private:
    static const float YAW;
    static const float PITCH;
    static const float SPEED;
    static const float SENSITIVITY;
    static const float ZOOM;
    static const float SPEEDBOOSTMULTIPLIER;

public:
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

public:
    Camera(
            const glm::vec3&    pos                 = glm::vec3(0.0f, 0.0f, 3.0f), 
            const glm::vec3&    up                  = glm::vec3(0.0f, 1.0f, 0.0f), 
            float               yaw                 = YAW,
            float               pitch               = PITCH,
            float               speed               = SPEED,
            float               mouseSensitivity    = SENSITIVITY,
            float               zoom                = ZOOM
        );

    void        processKeyboard(Camera_Movement direction, float deltaTime);
        
    void        processMouseMove(float xOffset, float yOffset, bool constrainPitch = true, float pitchStart = -89.0f, float pitchEnd = 89.0f,
                     bool constrainYaw = false, float yawStart = -89.0f, float yawEnd = 89.0f);

    void        processMouseScroll(float yOffset);
    
    void        processSpeedBoost(bool isBoosting);

    glm::vec3   front()                                                         const;
    glm::vec3   pos()                                                           const;
    glm::vec3   up()                                                            const;
    glm::mat4   viewMat()                                                       const;
    glm::mat4   perspective(float width, float height, float near, float far)   const;
    float       zoom()                                                          const;
    float       speed()                                                         const;

    void        setSpeed(float speed = SPEED);
    void        setUp(const glm::vec3& up);
private:
    void updateCameraVectors();

private:
    glm::vec3       m_front;                // 相机的指向的方向
    glm::vec3       m_pos;                  // 相机的位置
    glm::vec3       m_up;                   // 相机的向上方向
    glm::vec3       m_right;                // 相机的右边
    glm::vec3       m_worldUp;              // 世界的y轴

    float           m_yaw;
    float           m_pitch;

    float           m_speed;                // 相机移动速度
    float           m_speed_copy;           // 速度的副本用于加速后恢复
    float           m_mouseSensitivity;     // 鼠标灵敏度
    float           m_zoom;                 // 视野
};