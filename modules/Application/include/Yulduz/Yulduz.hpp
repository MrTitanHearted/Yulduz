#pragma once
#include <Yulduz/Engine.hpp>
#include <Yulduz/Raytracer.hpp>

namespace Yulduz {
    class App {
       public:
        static void Run();

       private:
        double m_DeltaTime;
        double m_EventDispatchTime;
        double m_ImGuiTime;
        double m_RenderTime;
        double m_ComputePassTime;
        double m_ImGuiPassTime;
        double m_CommandSubmissionTime;

        double m_Time;
        CameraManager m_Camera;

        AssetManager m_AssetManager;
        std::shared_ptr<EventDispatcher> m_EventDispatcher;
        std::shared_ptr<Window> m_Window;
        std::shared_ptr<RenderContext> m_Context;
        std::shared_ptr<Framebuffer> m_Depthbuffer;
        std::shared_ptr<RayTracer> m_RayTracer;

        Scene m_Scene;

        ImVec2 m_Viewport;

        ImFont *m_Font;

       private:
        App();
        ~App();

        void run();
        void renderFrame(const std::shared_ptr<Framebuffer> &frame);
        void renderImGui();
        void moveCamera();

        void keyCallback(const WindowKeyEvent &event);
        void resizeCallback(const WindowResizeEvent &event);
        void mouseMoveCallback(const WindowMouseMoveEvent &event);
        void mouseScrollCallback(const WindowMouseScrollEvent &event);
    };
}  // namespace Yulduz
