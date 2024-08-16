#pragma once

#include <Yulduz/Core.hpp>
#include <Yulduz/Window.hpp>
#include <Yulduz/Graphics.hpp>

namespace Yulduz {
    class App {
       public:
        static void Run();

       private:
        Window m_Window;
        GraphicsContext m_Context;

        double m_DeltaTime;
        double m_Time;

        ImFont *m_Font;
        ImVec2 m_ViewportSize;

        CameraManager m_Camera;

        Texture m_Frame;
        RenderBundle m_TriangleRenderBundle;
        UniformBuffer m_CameraUniformBuffer;
        UniformBuffer m_ModelUniformBuffer;

        float m_CameraMultiplier;

       private:
        struct CameraData {
            glm::mat4 Projection;
            glm::mat4 View;
        };

       private:
        App();
        ~App();

        void run();
        void renderFrame(const Texture &frame);

        void renderImGui();
        void resizeFrame();

        void moveCamera();

        void keyCallback(const WindowKeyEvent &event);
        void resizeCallback(const WindowResizeEvent &event);
        void mouseMoveCallback(const WindowMouseMoveEvent &event);
        void mouseScrollCallback(const WindowMouseScrollEvent &event);
    };
}  // namespace Yulduz