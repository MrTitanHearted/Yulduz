#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Enums.hpp>
#include <YulduzRender/Camera/Camera.hpp>

namespace Yulduz {
    class CameraManager {
       public:
        CameraManager(const std::string &label = "Yulduz Camera", float aspectRatio = 1.0f);
        ~CameraManager() = default;

        void move(CameraMovement direction, float dt);
        void moveMouse(float x, float y, bool constrainPitch = true, float constrainValue = 90.0f);
        void scrollMouse(float yOffset, float zoomMax = 45.0f, float zoomMin = 1.0f);

        void setPosition(glm::vec3 position);
        void setFront(glm::vec3 front);
        void setWorldUp(glm::vec3 worldUp);
        void setSpeed(float speed);
        void setSensitivity(float sensitivity);
        void setZoom(float zoom);
        void setFirstMouse(bool firstMouse);
        void setLastX(float lastX);
        void setLastY(float lastY);
        void setFar(float zFar);
        void setNear(float zNear);
        void setAspectRatio(float aspectRatio);

        CameraManager &withPosition(glm::vec3 position);
        CameraManager &withFront(glm::vec3 front);
        CameraManager &withWorldUp(glm::vec3 worldUp);
        CameraManager &withSpeed(float speed);
        CameraManager &withSensitivity(float sensitivity);
        CameraManager &withZoom(float zoom);
        CameraManager &withFirstMouse(bool firstMouse);
        CameraManager &withLastX(float lastX);
        CameraManager &withLastY(float lastY);
        CameraManager &withFar(float zFar);
        CameraManager &withNear(float zNear);
        CameraManager &withAspectRatio(float aspectRatio);

        std::string getLabel() const;
        glm::mat4 getProjection() const;
        glm::mat4 getView() const;

        glm::vec3 getPosition() const;
        glm::vec3 getFront() const;
        glm::vec3 getRight() const;
        glm::vec3 getUp() const;
        glm::vec3 getWorldUp() const;
        float getYaw() const;
        float getPitch() const;
        float getSpeed() const;
        float getSensitivity() const;
        float getZoom() const;
        
        bool getFirstMouse() const;
        float getLastX() const;
        float getLastY() const;
        float getFar() const;
        float getNear() const;
        float getAspectRatio() const;

       private:
        Camera m_Camera;

        bool m_FirstMouse;

        float m_LastX;
        float m_LastY;
        float m_Far;
        float m_Near;
        float m_AspectRatio;
    };
}  // namespace Yulduz
