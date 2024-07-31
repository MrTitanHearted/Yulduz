#pragma once
#include <Yulduz/Engine.hpp>
#include <Yulduz/Renderer.hpp>

namespace Yulduz {
    class App {
       public:
        static void Run();

       private:
        Timer m_Timer;
        EventDispatcher m_EventDispatcher;
        AssetManager m_AssetManager;
        std::shared_ptr<Window> m_Window;
        std::shared_ptr<RenderContext> m_Context;
        std::shared_ptr<Framebuffer> m_Depthbuffer;

        Renderer m_Renderer;
        ImVec2 m_Viewport;
        float m_LastRenderTime;

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
