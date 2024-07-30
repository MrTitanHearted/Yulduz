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
        std::shared_ptr<Framebuffer> m_ContextDepthBuffer;

        std::shared_ptr<Framebuffer> m_Depthbuffer;
        std::shared_ptr<Framebuffer> m_Framebuffer;
        std::vector<std::uint32_t> m_Framedata;

        ImFont *m_Font;

        bool m_Render;

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
