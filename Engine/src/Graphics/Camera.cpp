#include <Camera.h>
#include <glm/gtc/matrix_transform.hpp>

const float Camera::YAW                     =-90.0f;
const float Camera::PITCH                   = 0.0f;
const float Camera::SPEED                   = 2.5f;
const float Camera::SENSITIVITY             = 0.1f;
const float Camera::ZOOM                    = 45.0f;
const float Camera::SPEEDBOOSTMULTIPLIER    = 2.5f;

Camera::Camera( const glm::vec3 &pos,
                const glm::vec3 &up, float yaw, float pitch, float speed,
                float mouseSensitivity, float zoom)
    : m_pos(pos), m_up(up), m_worldUp(glm::vec3(0.0f, 1.0f, 0.0f)), m_yaw(yaw), m_pitch(pitch),
      m_speed(speed), m_speed_copy(m_speed), m_mouseSensitivity(mouseSensitivity), m_zoom(zoom) {
    
    updateCameraVectors();
}



glm::vec3 Camera::front() const {
    return m_front;
}

glm::vec3 Camera::pos() const {
    return m_pos;
}

glm::vec3 Camera::up() const {
    return m_up;
}

float Camera::zoom() const {
    return m_zoom;
}

float Camera::speed() const {
    return m_speed;
}

void Camera::setSpeed(float speed) {
    m_speed = speed;
    m_speed_copy = m_speed;
}

void Camera::setUp(const glm::vec3& up) {
    m_up = up;
}

glm::mat4 Camera::viewMat() const {
    return glm::lookAt(m_pos, m_pos + m_front, m_up);
    // glm::vec3 target = m_pos + m_front;
    // glm::vec3 front = glm::normalize(m_pos - target);
    // glm::vec3 right = glm::normalize(glm::cross(m_up, front));
    // glm::vec3 up    = glm::normalize(glm::cross(front, right));

    // glm::mat4 lhs(
    //     glm::vec4(right, 0.0f),
    //     glm::vec4(up, 0.0f),
    //     glm::vec4(front, 0.0f),
    //     glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    // );

    // lhs = glm::transpose(lhs);

    // glm::mat4 rhs(
    //     glm::vec4(1.0f, 0.0f, 0.0f, -m_pos.x),
    //     glm::vec4(0.0f, 1.0f, 0.0f, -m_pos.y),
    //     glm::vec4(0.0f, 0.0f, 1.0f, -m_pos.z),
    //     glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
    // );

    // rhs = glm::transpose(rhs);
    
    // return lhs * rhs;
}

glm::mat4 Camera::perspective(float width, float height, float near, float far) const {
    return glm::perspective(glm::radians(m_zoom), width / height, near, far);
}

void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
    float velocity = m_speed * deltaTime;

    if (direction == FORWARD) {
        m_pos += m_front * velocity;
    }
    
    if (direction == BACKWARD) {
        m_pos -= m_front * velocity;
    }

    if (direction == LEFT) {
        m_pos -= m_right * velocity;
    } 

    if (direction == RIGHT) {
        m_pos += m_right * velocity;
    }

    if (direction == UP) {
        m_pos += m_worldUp * velocity;
    }

    if (direction == DOWN) {
        m_pos -= m_worldUp * velocity;
    }

}
        
void Camera::processMouseMove(float xOffset, float yOffset, bool constrainPitch, float pitchStart, float pitchEnd,
                     bool constrainYaw, float yawStart, float yawEnd) 
{
    xOffset *= m_mouseSensitivity;
    yOffset *= m_mouseSensitivity;   

    m_yaw   += xOffset;
    m_pitch += yOffset;

    if (constrainPitch) {
        m_pitch = std::max(pitchStart, m_pitch);
        m_pitch = std::min(pitchEnd, m_pitch);
    }

    if (constrainYaw) {
        m_yaw = std::max(yawStart, m_yaw);
        m_yaw = std::min(yawEnd, m_yaw);
    }

    updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset) {
    m_zoom -= yOffset;
   
    if (m_zoom < 1.0f) {
        m_zoom = 1.0f;
    }

    if (m_zoom > 90.0f) {
        m_zoom = 90.0f;
    }
} 

void Camera::processSpeedBoost(bool isBoosting) {
    if (isBoosting) {
        m_speed = m_speed_copy * SPEEDBOOSTMULTIPLIER;
    }
    else {
        m_speed = m_speed_copy;
    }
}

void Camera::updateCameraVectors() {
    glm::vec3 front;

    float pitch_rad  = glm::radians(m_pitch);
    float yaw_rad    = glm::radians(m_yaw);

    front.x = std::cos(pitch_rad) * std::cos(yaw_rad);
    front.y = std::sin(pitch_rad);
    front.z = std::cos(pitch_rad) * std::sin(yaw_rad);
    m_front = glm::normalize(front);

    m_right = glm::normalize(glm::cross(front, m_worldUp));
    m_up    = glm::normalize(glm::cross(m_right, front));
}