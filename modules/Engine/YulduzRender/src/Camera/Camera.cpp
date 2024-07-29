#include <YulduzRender/Camera/Camera.hpp>

namespace Yulduz {
    const float Camera::YAW = -90.0f;
    const float Camera::PITCH = 0.0f;
    const float Camera::SPEED = 500.0f;
    const float Camera::SENSITIVITY = 0.1f;
    const float Camera::ZOOM = 45.0f;
    const glm::vec3 Camera::WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);

    Camera::Camera(const std::string &label) {
        YZDEBUG("Initializing Camera: '{}'", label);

        m_Label = label;

        m_Position = glm::vec3(0.0f);
        m_WorldUp = WORLD_UP;
        m_Yaw = YAW;
        m_Pitch = PITCH;
        m_MovementSpeed = SPEED;
        m_MouseSensitivity = SENSITIVITY;
        m_Zoom = ZOOM;

        updateCameraVectors();

        setFront(glm::vec3(0.0f, 0.0f, -1.0f));
    }

    Camera::~Camera() {
        YZDEBUG("Releasing Camera: '{}'", m_Label);
    }

    void Camera::move(CameraMovement direction, float dt) {
        float speed = m_MovementSpeed * dt;
        if (static_cast<int>(direction & CameraMovement::Forward))
            m_Position += m_Front * speed;
        if (static_cast<int>(direction & CameraMovement::Backward))
            m_Position -= m_Front * speed;
        if (static_cast<int>(direction & CameraMovement::Up))
            m_Position += m_Up * speed;
        if (static_cast<int>(direction & CameraMovement::Down))
            m_Position -= m_Up * speed;
        if (static_cast<int>(direction & CameraMovement::WorldForward))
            m_Position += glm::normalize(glm::vec3(m_Front.x, 0.0f, m_Front.z)) * speed;
        if (static_cast<int>(direction & CameraMovement::WorldBackward))
            m_Position -= glm::normalize(glm::vec3(m_Front.x, 0.0f, m_Front.z)) * speed;
        if (static_cast<int>(direction & CameraMovement::WorldUp))
            m_Position += m_WorldUp * speed;
        if (static_cast<int>(direction & CameraMovement::WorldDown))
            m_Position -= m_WorldUp * speed;
        if (static_cast<int>(direction & CameraMovement::Right))
            m_Position += m_Right * speed;
        if (static_cast<int>(direction & CameraMovement::Left))
            m_Position -= m_Right * speed;
    }

    void Camera::moveMouse(float xOffset, float yOffset, bool constrainPitch, float constrainValue) {
        xOffset *= m_MouseSensitivity;
        yOffset *= m_MouseSensitivity;

        m_Yaw += xOffset;

        if (constrainPitch) {
            if (m_Pitch + yOffset > constrainValue)
                m_Pitch = constrainValue;
            else if (m_Pitch + yOffset < -constrainValue)
                m_Pitch = -constrainValue;
            else
                m_Pitch += yOffset;
        } else {
            m_Pitch += yOffset;
        }

        updateCameraVectors();
    }

    void Camera::scrollMouse(float yOffset, float zoomMax, float zoomMin) {
        m_Zoom -= yOffset;
        if (m_Zoom > zoomMax)
            m_Zoom = zoomMax;
        if (m_Zoom < zoomMin)
            m_Zoom = zoomMin;
    }

    void Camera::setPosition(const glm::vec3 &position) {
        m_Position = position;
    }

    void Camera::setFront(const glm::vec3 &front) {
        m_Front = glm::normalize(front);
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

    void Camera::setWorldUp(const glm::vec3 &worldUp) {
        m_WorldUp = glm::normalize(worldUp);
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }

    void Camera::setSpeed(float speed) {
        m_MovementSpeed = speed;
    }

    void Camera::setSensitivity(float sensitivity) {
        m_MouseSensitivity = sensitivity;
    }

    void Camera::setZoom(float zoom) {
        m_Zoom = zoom;
    }

    std::string Camera::getLabel() const {
        return m_Label;
    }

    glm::mat4 Camera::getProjection(float aspectRatio, float zNear, float zFar) const {
        return glm::perspective(glm::radians(m_Zoom), aspectRatio, zNear, zFar);
    }

    glm::mat4 Camera::getView() const {
        return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
    }

    glm::vec3 Camera::getPosition() const {
        return m_Position;
    }

    glm::vec3 Camera::getFront() const {
        return m_Front;
    }

    glm::vec3 Camera::getRight() const {
        return m_Right;
    }

    glm::vec3 Camera::getUp() const {
        return m_Up;
    }

    glm::vec3 Camera::getWorldUp() const {
        return m_WorldUp;
    }

    float Camera::getYaw() const {
        return m_Yaw;
    }

    float Camera::getPitch() const {
        return m_Pitch;
    }

    float Camera::getSpeed() const {
        return m_MovementSpeed;
    }

    float Camera::getSensitivity() const {
        return m_MouseSensitivity;
    }

    float Camera::getZoom() const {
        return m_Zoom;
    }

    void Camera::updateCameraVectors() {
        glm::vec3 front;
        front.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
        front.y = glm::sin(glm::radians(m_Pitch));
        front.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));

        m_Front = glm::normalize(front);
        m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
        m_Up = glm::normalize(glm::cross(m_Right, m_Front));
    }
}  // namespace Yulduz
