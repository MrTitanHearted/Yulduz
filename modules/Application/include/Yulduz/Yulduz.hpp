#pragma once
#include <Yulduz/Engine.hpp>

namespace Yulduz {
    class App {
       public:
        static void Run();

       private:
        bool m_IsInGameWindow;

        Timer m_Timer;
        EventDispatcher m_EventDispatcher;
        AssetManager m_AssetManager;
        std::shared_ptr<Window> m_Window;
        std::shared_ptr<RenderContext> m_Context;

        ImFont *m_Font;

        std::shared_ptr<RenderBundle> m_RenderBundle;

        std::shared_ptr<Framebuffer> m_Depthbuffer;
        std::shared_ptr<Framebuffer> m_Framebuffer;

        CameraManager m_Camera;

        std::shared_ptr<UniformBuffer> m_ProjectionUniform;
        std::shared_ptr<UniformBuffer> m_ViewUniform;

       private:
        App();
        ~App();

        void run();
        void renderFrame(const std::shared_ptr<Framebuffer> &frame);
        void renderImGui();

        void keyCallback(const WindowKeyEvent &event);
        void mouseButtonCallback(const WindowMouseButtonEvent &event);
        void resizeCallback(const WindowResizeEvent &event);
        void mouseMoveCallback(const WindowMouseMoveEvent &event);
        void mouseScrollCallback(const WindowMouseScrollEvent &event);
    };
}  // namespace Yulduz
