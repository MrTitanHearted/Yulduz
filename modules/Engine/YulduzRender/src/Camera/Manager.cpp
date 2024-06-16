#include <YulduzRender/Camera/Manager.hpp>

namespace Yulduz {
    CameraManager::CameraManager(const std::string &label, float aspectRatio)
        : m_Camera(label) {
        m_FirstMouse = true;
        m_Far = 100.0f;
        m_Near = 0.01f;
        m_AspectRatio = aspectRatio;
    }

    void CameraManager::move(CameraMovement direction, float dt) {
        m_Camera.move(direction, dt);
    }

    void CameraManager::moveMouse(float x, float y, bool constrainPitch, float constrainValue) {
        if (m_FirstMouse) {
            m_LastX = x;
            m_LastY = y;

            m_FirstMouse = false;
        }

        float xOffset = x - m_LastX;
        float yOffset = m_LastY - y;
        m_LastX = x;
        m_LastY = y;

        m_Camera.moveMouse(xOffset, yOffset, constrainPitch, constrainValue);
    }

    void CameraManager::scrollMouse(float yOffset, float zoomMax, float zoomMin) {
        m_Camera.scrollMouse(yOffset, zoomMax, zoomMin);
    }

    void CameraManager::setPosition(glm::vec3 position) {
        m_Camera.setPosition(position);
    }

    void CameraManager::setFront(glm::vec3 front) {
        m_Camera.setFront(front);
    }

    void CameraManager::setWorldUp(glm::vec3 worldUp) {
        m_Camera.setWorldUp(worldUp);
    }

    void CameraManager::setSpeed(float speed) {
        m_Camera.setSpeed(speed);
    }

    void CameraManager::setSensitivity(float sensitivity) {
        m_Camera.setSensitivity(sensitivity);
    }

    void CameraManager::setZoom(float zoom) {
        m_Camera.setZoom(zoom);
    }

    void CameraManager::setFirstMouse(bool firstMouse) {
        m_FirstMouse = firstMouse;
    }

    void CameraManager::setLastX(float lastX) {
        m_LastX = lastX;
    }

    void CameraManager::setLastY(float lastY) {
        m_LastY = lastY;
    }

    void CameraManager::setFar(float zFar) {
        m_Far = zFar;
    }

    void CameraManager::setNear(float zNear) {
        m_Near = zNear;
    }

    void CameraManager::setAspectRatio(float aspectRatio) {
        m_AspectRatio = aspectRatio;
    }

    CameraManager &CameraManager::withPosition(glm::vec3 position) {
        m_Camera.setPosition(position);
        return *this;
    }

    CameraManager &CameraManager::withFront(glm::vec3 front) {
        m_Camera.setFront(front);
        return *this;
    }

    CameraManager &CameraManager::withWorldUp(glm::vec3 worldUp) {
        m_Camera.setWorldUp(worldUp);
        return *this;
    }

    CameraManager &CameraManager::withSpeed(float speed) {
        m_Camera.setSpeed(speed);
        return *this;
    }

    CameraManager &CameraManager::withSensitivity(float sensitivity) {
        m_Camera.setSensitivity(sensitivity);
        return *this;
    }

    CameraManager &CameraManager::withZoom(float zoom) {
        m_Camera.setZoom(zoom);
        return *this;
    }

    CameraManager &CameraManager::withFirstMouse(bool firstMouse) {
        m_FirstMouse = firstMouse;
        return *this;
    }

    CameraManager &CameraManager::withLastX(float lastX) {
        m_LastX = lastX;
        return *this;
    }

    CameraManager &CameraManager::withLastY(float lastY) {
        m_LastY = lastY;
        return *this;
    }

    CameraManager &CameraManager::withFar(float zFar) {
        m_Far = zFar;
        return *this;
    }

    CameraManager &CameraManager::withNear(float zNear) {
        m_Near = zNear;
        return *this;
    }

    CameraManager &CameraManager::withAspectRatio(float aspectRatio) {
        m_AspectRatio = aspectRatio;
        return *this;
    }

    std::string CameraManager::getLabel() const {
        return m_Camera.getLabel();
    }

    glm::mat4 CameraManager::getView() const {
        return m_Camera.getView();
    }

    glm::mat4 CameraManager::getProjection() const {
        return m_Camera.getProjection(m_AspectRatio, m_Near, m_Far);
    }

    glm::vec3 CameraManager::getPosition() const {
        return m_Camera.getPosition();
    }

    glm::vec3 CameraManager::getFront() const {
        return m_Camera.getFront();
    }

    glm::vec3 CameraManager::getRight() const {
        return m_Camera.getRight();
    }

    glm::vec3 CameraManager::getUp() const {
        return m_Camera.getUp();
    }

    glm::vec3 CameraManager::getWorldUp() const {
        return m_Camera.getWorldUp();
    }

    float CameraManager::getYaw() const {
        return m_Camera.getYaw();
    }

    float CameraManager::getPitch() const {
        return m_Camera.getPitch();
    }

    float CameraManager::getSpeed() const {
        return m_Camera.getSpeed();
    }

    float CameraManager::getSensitivity() const {
        return m_Camera.getSensitivity();
    }

    float CameraManager::getZoom() const {
        return m_Camera.getZoom();
    }

    bool CameraManager::getFirstMouse() const {
        return m_FirstMouse;
    }

    float CameraManager::getLastX() const {
        return m_LastX;
    }

    float CameraManager::getLastY() const {
        return m_LastY;
    }

    float CameraManager::getFar() const {
        return m_Far;
    }

    float CameraManager::getNear() const {
        return m_Near;
    }

    float CameraManager::getAspectRatio() const {
        return m_AspectRatio;
    }
}  // namespace Yulduz
