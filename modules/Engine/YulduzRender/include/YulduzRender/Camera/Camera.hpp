#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Enums.hpp>

namespace Yulduz {
    class Camera {
       public:
        static const float YAW;
        static const float PITCH;
        static const float SPEED;
        static const float SENSITIVITY;
        static const float ZOOM;
        static const glm::vec3 WORLD_UP;

       public:
        Camera(const std::string &label = "Yulduz Camera");
        ~Camera();

        void move(CameraMovement direction, float dt);
        void moveMouse(float xOffset, float yOffset, bool constrainPitch, float constrainValue);
        void scrollMouse(float yOffset, float zoomMax, float zoomMin);

        void setPosition(const glm::vec3 &position);
        void setFront(const glm::vec3 &front);
        void setWorldUp(const glm::vec3 &worldUp);
        void setSpeed(float speed);
        void setSensitivity(float sensitivity);
        void setZoom(float zoom);

        std::string getLabel() const;

        glm::mat4 getProjection(float aspectRatio, float zNear, float zFar) const;
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

       private:
        std::string m_Label;

        glm::vec3 m_Position;
        glm::vec3 m_Front;
        glm::vec3 m_Right;
        glm::vec3 m_Up;
        glm::vec3 m_WorldUp;

        float m_Yaw;
        float m_Pitch;

        float m_MovementSpeed;
        float m_MouseSensitivity;
        float m_Zoom;

        void updateCameraVectors();
    };
}  // namespace Yulduz
