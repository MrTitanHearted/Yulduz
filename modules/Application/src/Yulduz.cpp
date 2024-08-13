#include <Yulduz/Yulduz.hpp>

namespace Yulduz {
    void App::Run() {
        // Logger::SetLogLevel(LogLevel::Info);
        Window::SetEventDispatcher(EventDispatcher::GetDefault());
        App app{};
        app.run();
    }

    App::App()
        : m_Window{Window::Settings{}} {
        YZDEBUG("Initializing Yulduz Application");
        m_Context = GraphicsContextBuilder::New()
                        .setBackend(InstanceBackend::Vulkan)
                        .setPreferredSurfaceFormat(TextureFormat::BGRA8Unorm)
                        .build(m_Window);
        InitImGui(ImGuiContext{
            .DepthFormat = TextureFormat::Undefined,
            .Context = m_Context,
        });
        auto &dispatcher = EventDispatcher::GetDefault();
        dispatcher.addCallback<WindowKeyEvent>(&App::keyCallback, this);
        dispatcher.addCallback<WindowResizeEvent>(&App::resizeCallback, this);

        m_DeltaTime = 0.0;
        m_Time = 0.0;

        ImGuiIO &io = ImGui::GetIO();
        m_Font = io.Fonts->AddFontFromFileTTF(YULDUZ_SOURCE_DIR "/assets/fonts/JetBrainsMonoNerdFont-Medium.ttf", 18.0f);
    }

    App::~App() {
        YZDEBUG("Releasing Yulduz Application");
        ShutdownImGui();
    }

    void App::run() {
        static Milliseconds::Timer timer;
        EventDispatcher &dispatcher = EventDispatcher::GetDefault();

        while (m_Window.isRunning()) {
            timer.start();

            Window::PollEvents();
            dispatcher.dispatch();

            ImGuiFrame(&App::renderImGui, this);

            m_Context.renderFrame(&App::renderFrame, this);

            timer.stop();
            m_DeltaTime = timer.getElapsed();
            m_Time += m_DeltaTime;
        }
    }

    void App::renderFrame(const Texture &frame) {
        auto commandEncoder = CommandEncoderBuilder::New().build(m_Context);
        auto renderPass = RenderPassBuilder::New()
                              .setColorAttachments({ColorAttachment::New(frame)
                                                        .setClearColor3({0.2f, 0.3f, 0.3f})})
                              .build(commandEncoder);
        RenderImGuiPass(renderPass);
        renderPass.finish();

        m_Context.sumbitCommands({commandEncoder.finish()});
    }

    void App::renderImGui() {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::DockSpaceOverViewport();
        ImGui::PushFont(m_Font);

        ImGui::Begin("Hey");
        ImGui::Text("Hello, World!");
        ImGui::Text("FPS: %.3f", 1000.0 / m_DeltaTime);
        ImGui::Text("Time: %.6f", m_Time);
        ImGui::Text("Delta Time: %.6f", m_DeltaTime);
        ImGui::Text("Backend Type: %s", GetBackendTypeName(m_Context.getBackendType()));
        ImGui::End();

        ImGui::Begin("Present Mode");
        ImGui::Text("Current Present Mode: '%s'", GetPresentModeName(m_Context.getPresentMode()));
        ImGui::Separator();
        ImGui::Text("Available Present Modes");
        SurfaceCapabilities caps = m_Context.getSurfaceCapabilities();
        static PresentMode selected = m_Context.getPresentMode();
        for (const PresentMode &mode : caps.PresentModes) {
            if (ImGui::Selectable(GetPresentModeName(mode), selected == mode) && m_Context.getPresentMode() != mode) {
                selected = mode;
                m_Context.setPresentMode(mode);
            }
        }
        ImGui::End();

        ImGui::PopFont();
    }

    void App::keyCallback(const WindowKeyEvent &event) {
        if (event.action != KeyAction::Press) return;
        if (event.key == KeyCode::Escape) m_Window.close();
        if (event.key == KeyCode::F) {
            if (m_Window.isFullscreen())
                m_Window.makeWindowed();
            else
                m_Window.makeFullscreen();
            auto [width, height] = m_Window.getSize();
            m_Context.resize(width, height);
        }
    }

    void App::resizeCallback(const WindowResizeEvent &event) {
        ImGui_ImplWGPU_InvalidateDeviceObjects();
        m_Context.resize(event.width, event.height);
        ImGui_ImplWGPU_CreateDeviceObjects();
    }
}  // namespace Yulduz
