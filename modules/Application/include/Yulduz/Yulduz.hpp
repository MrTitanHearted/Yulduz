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

        BindGroupLayout m_FrameBindGroupLayout;
        ComputePipeline m_FramePipeline;
        Texture m_Frame;
        UniformBuffer m_FrameUniform;
        BindGroup m_FrameBindGroup;

       private:
        struct FrameData {
            float DeltaTime;
            float Time;
        };

       private:
        App();
        ~App();

        void run();
        void renderFrame(const Texture &frame);
        
        void renderImGui();
        void resizeFrame();

        void keyCallback(const WindowKeyEvent &event);
        void resizeCallback(const WindowResizeEvent &event);
    };
}  // namespace Yulduz