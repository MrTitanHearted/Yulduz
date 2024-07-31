#pragma once
#include <Yulduz/Engine.hpp>
#include <Yulduz/Renderer.hpp>
#include <Yulduz/RayTracedCamera.hpp>
#include <Yulduz/Scene.hpp>

namespace Yulduz {
    class App {
       public:
        static void Run();

       private:
        double m_DeltaTime;
        double m_EventDispatchTime;
        double m_CameraTime;
        double m_ImGuiTime;
        double m_RenderTime;
        double m_RayTracingTime;

        EventDispatcher m_EventDispatcher;
        AssetManager m_AssetManager;
        std::shared_ptr<Window> m_Window;
        std::shared_ptr<RenderContext> m_Context;
        std::shared_ptr<Framebuffer> m_Depthbuffer;

        Renderer m_Renderer;
        RayTracedCamera m_Camera;
        Scene m_Scene;
        ImVec2 m_Viewport;

        ImFont *m_Font;

       private:
        App();
        ~App();

        void run();
        void renderFrame(const std::shared_ptr<Framebuffer> &frame);
        void renderImGui();

        void updateFramedata();

        void keyCallback(const WindowKeyEvent &event);
        void resizeCallback(const WindowResizeEvent &event);
    };
}  // namespace Yulduz
