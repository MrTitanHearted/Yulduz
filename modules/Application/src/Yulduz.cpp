#include <Yulduz/Yulduz.hpp>

namespace Yulduz {
    void App::Run() {
        App app;
        app.run();
    }

    App::App() {
        m_Window = Window::New(Window::Settings{
            .Title = "Yulduz Cherno Raytracing",
            .EventDispatcher = m_EventDispatcher,
        });
        auto [width, height] = m_Window->getSize();
        m_Context =
            RenderContextBuilder::New()
                .addSurfaceUsage(TextureUsage::CopySrc)
                .build(m_Window);
        m_ContextDepthBuffer =
            TextureBuilder::New()
                .setLabel("Yulduz Context Depth Buffer")
                .setFormat(TextureFormat::Depth32Float)
                .emptyFramebuffer(width, height, m_Context);
        m_Context->registerCallbacks(m_EventDispatcher);
        m_EventDispatcher.addCallback<WindowKeyEvent>(std::bind(&App::keyCallback, this, std::placeholders::_1));
        m_EventDispatcher.addCallback<WindowResizeEvent>(std::bind(&App::resizeCallback, this, std::placeholders::_1));
        RenderContext::SetupWGPULogging(WebGPULogLevel::Error);
        InitImGui(m_Context);
        ImGuiIO &io = ImGui::GetIO();
        m_Font = io.Fonts->AddFontFromFileTTF("assets/fonts/JetBrainsMonoNerdFont-Medium.ttf", 18.0f);

        m_Depthbuffer = TextureBuilder::New()
                            .setLabel("Yulduz Main Depth Buffer")
                            .setFormat(TextureFormat::Depth32Float)
                            .emptyFramebuffer(width, height, m_Context);
        m_Framebuffer = TextureBuilder::New()
                            .setLabel("ImGui Framebuffer")
                            .addTextureUsage(TextureUsage::CopySrc)
                            .setFormat(m_Context->getSurfaceFormat())
                            .emptyFramebuffer(width, height, m_Context);
        m_Framedata.resize(width * height * m_Framebuffer->getFormatSize() / sizeof(std::uint32_t));
    }

    App::~App() {
        ShutdownImGui();
    }

    void App::run() {
        while (m_Window->isRunning()) {
            m_Timer.start();

            Window::PollEvents();
            m_EventDispatcher.dispatch();

            updateFramedata();

            if (m_Render) ImageCopyTexture::New(m_Framebuffer).write(m_Framedata.data(), m_Context);

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
                .setDepthStencilAttachment(DepthStencilAttachment::New(m_ContextDepthBuffer))
                .build(encoder);
        RenderImGui(renderPass);
        renderPass->finish();

        commands.emplace_back(encoder->finish());

        m_Context->submitCommands(commands);
    }

    void App::renderImGui() {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::DockSpaceOverViewport();
        ImGui::PushFont(m_Font);

        ImGui::ShowDemoWindow();

        ImGui::Begin("Settings");
        m_Render = ImGui::Button("Render", ImVec2{100, 20});
        ImGui::End();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        if (ImGui::Begin("Viewport")) {
            ImGui::Image(m_Framebuffer->getView(), ImGui::GetContentRegionAvail());
        } else {
            m_IsInGameWindow = false;
        }
        ImGui::PopStyleVar();
        ImGui::PopFont();
        ImGui::End();
    }

    void App::updateFramedata() {
        for (std::size_t i = 0; i < m_Framedata.size(); i++) {
            m_Framedata[i] = 0xFFFF00FF;
        }
    }

    void App::keyCallback(const WindowKeyEvent &event) {
        if (event.action != KeyAction::PRESS) return;
        if (event.key == KeyCode::ESCAPE) m_Window->close();
        if (event.key == KeyCode::R) m_Context->printWGPUReport();
    }

    void App::resizeCallback(const WindowResizeEvent &event) {
        m_ContextDepthBuffer =
            TextureBuilder::New()
                .setLabel("Yulduz Context Depth Buffer")
                .setFormat(TextureFormat::Depth32Float)
                .emptyFramebuffer(event.width, event.height, m_Context);
        m_Depthbuffer = TextureBuilder::New()
                            .setLabel("Yulduz Main Depth Buffer")
                            .setFormat(TextureFormat::Depth32Float)
                            .emptyFramebuffer(event.width, event.height, m_Context);
        m_Framebuffer = TextureBuilder::New()
                            .setLabel("ImGui Framebuffer")
                            .addTextureUsage(TextureUsage::CopySrc)
                            .setFormat(m_Context->getSurfaceFormat())
                            .emptyFramebuffer(event.width, event.height, m_Context);
        m_Framedata.resize(event.width * event.height * m_Framebuffer->getFormatSize() / sizeof(std::uint32_t));
    }
}  // namespace Yulduz
