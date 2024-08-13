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

       private:
        App();
        ~App();

        void run();
        void renderFrame(const Texture &frame);
        void renderImGui();

        void keyCallback(const WindowKeyEvent &event);
        void resizeCallback(const WindowResizeEvent &event);
    };
}  // namespace Yulduz