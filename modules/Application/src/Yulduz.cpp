#include <Yulduz/Yulduz.hpp>

namespace Yulduz {
    void App::Run() {
        App app;
        app.run();
    }

    App::App()
        : m_Camera{45.0f, 0.1f, 100.0f} {
        m_Window = Window::New(Window::Settings{
            .Title = "Yulduz Cherno Raytracing",
            .Width = 1200,
            .Height = 800,
            .EventDispatcher = m_EventDispatcher,
        });
        auto [width, height] = m_Window->getSize();
        m_Context = RenderContextBuilder::New()
                        .addSurfaceUsage(TextureUsage::CopySrc)
                        .build(m_Window);

        m_Context->registerCallbacks(m_EventDispatcher);
        m_EventDispatcher.addCallback<WindowKeyEvent>(std::bind(&App::keyCallback, this, std::placeholders::_1));
        m_EventDispatcher.addCallback<WindowResizeEvent>(std::bind(&App::resizeCallback, this, std::placeholders::_1));
        RenderContext::SetupWGPULogging(WebGPULogLevel::Error);
        InitImGui(m_Context);
        ImGuiIO &io = ImGui::GetIO();
        m_Font = io.Fonts->AddFontFromFileTTF(YULDUZ_SOURCE_DIR "/assets/fonts/JetBrainsMonoNerdFont-Medium.ttf", 18.0f);
        m_Depthbuffer = TextureBuilder::New()
                            .setLabel("ImGui Depthbuffer")
                            .setFormat(TextureFormat::Depth32Float)
                            .emptyFramebuffer(width, height, m_Context);

        m_LastRenderTime = 0.0f;
        m_Renderer.setRenderContext(m_Context);
        m_Scene.Spheres.emplace_back(Sphere{
            .Position = glm::vec3{0.0f}, 
            .Radius = 0.5f, 
            .Albedo = glm::vec3{1.0f, 0.0f, 1.0f},
        });
        m_Scene.Spheres.emplace_back(Sphere{
            .Position = glm::vec3{1.0f,0.0f,-5.0f}, 
            .Radius = 1.5f, 
            .Albedo = glm::vec3{0.2f, 0.3f, 1.0f},
        });
    }

    App::~App() {
        ShutdownImGui();
    }

    void App::run() {
        while (m_Window->isRunning()) {
            m_Timer.start();

            Window::PollEvents();
            m_EventDispatcher.dispatch();

            m_Camera.OnUpdate(m_Timer.getElapsedSeconds() * 1000.0f, m_Window);

            ImGuiFrame(std::bind(&App::renderImGui, this));

            m_Context->renderFrameOnSurface(std::bind(&App::renderFrame, this, std::placeholders::_1));

            m_Timer.stop();
        }
    }

    void App::renderFrame(const std::shared_ptr<Framebuffer> &frame) {
        std::vector<std::shared_ptr<CommandBuffer>> commands;
        std::shared_ptr<CommandEncoder> encoder = CommandEncoderBuilder::New().build(m_Context);
        std::shared_ptr<RenderPass> renderPass =
            RenderPassBuilder::New()
                .setLabel("Yulduz ImGui Render Pass")
                .addColorAttachment(ColorAttachment::New(frame))
                .setDepthStencilAttachment(DepthStencilAttachment::New(m_Depthbuffer))
                .build(encoder);
        RenderImGui(renderPass);
        renderPass->finish();

        commands.emplace_back(encoder->finish());

        m_Context->submitCommands(commands);
    }

    void App::renderImGui() {
        updateFramedata();
        ImGuiIO &io = ImGui::GetIO();
        ImGui::DockSpaceOverViewport();
        ImGui::PushFont(m_Font);

        ImGui::Begin("Settings");
        ImGui::Text("Last render: %.3fms", m_LastRenderTime);
        if (ImGui::Button("Render"))
            updateFramedata();
        ImGui::End();

        ImGui::Begin("Scene");
        for (std::size_t i = 0; i < m_Scene.Spheres.size(); i++) {
            ImGui::PushID(i);

            Sphere& sphere = m_Scene.Spheres[i];
            ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
            ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
            ImGui::ColorEdit3("Albedo", glm::value_ptr(sphere.Albedo));

            ImGui::Separator();

            ImGui::PopID();
        }
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");

        m_Viewport = ImGui::GetContentRegionAvail();

        std::shared_ptr<Framebuffer> image = m_Renderer.getFinalImage();
        if (image)
            ImGui::Image(image->getView(), ImVec2{static_cast<float>(image->getWidth()), static_cast<float>(image->getHeight())}, ImVec2{0, 1}, ImVec2{1, 0});

        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopFont();
    }

    void App::updateFramedata() {
        if (m_Viewport.x == 0 || m_Viewport.y == 0) return;

        static Milliseconds::Timer timer;
        timer.start();

        m_Renderer.resize(m_Viewport.x, m_Viewport.y);
        m_Camera.OnResize(m_Viewport.x, m_Viewport.y);
        m_Renderer.render(m_Scene, m_Camera);

        timer.stop();

        m_LastRenderTime = timer.getElapsedSeconds();
    }

    void App::keyCallback(const WindowKeyEvent &event) {
        if (event.action != KeyAction::PRESS) return;
        if (event.key == KeyCode::ESCAPE) m_Window->close();
        if (event.key == KeyCode::R) m_Context->printWGPUReport();
    }

    void App::resizeCallback(const WindowResizeEvent &event) {
        m_Depthbuffer = TextureBuilder::New()
                            .setLabel("Yulduz Context Depth Buffer")
                            .setFormat(TextureFormat::Depth32Float)
                            .emptyFramebuffer(event.width, event.height, m_Context);
    }
}  // namespace Yulduz
