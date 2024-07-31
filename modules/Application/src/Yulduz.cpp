#include <Yulduz/Yulduz.hpp>
#include <random>

namespace Random {
    std::uint32_t Uint32() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
#undef max
        static std::uniform_int_distribution<std::uint32_t> distr(0, std::numeric_limits<std::uint32_t>::max());
        return distr(gen);
    }
}  // namespace Random

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
    }

    App::~App() {
        ShutdownImGui();
    }

    void App::run() {
        while (m_Window->isRunning()) {
            m_Timer.start();

            Window::PollEvents();
            m_EventDispatcher.dispatch();

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

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");

        m_Viewport = ImGui::GetContentRegionAvail();

        if (m_Framebuffer)
            ImGui::Image(m_Framebuffer->getView(), ImVec2{static_cast<float>(m_Framebuffer->getWidth()), static_cast<float>(m_Framebuffer->getHeight())});

        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopFont();
    }

    void App::updateFramedata() {
        if (m_Viewport.x == 0 || m_Viewport.y == 0) return;

        static Milliseconds::Timer timer;
        timer.start();

        if (!m_Framebuffer || m_Viewport.x != m_Framebuffer->getWidth() || m_Viewport.y != m_Framebuffer->getHeight()) {
            m_Framebuffer = TextureBuilder::New()
                                .setLabel("ImGui Framebuffer")
                                .addTextureUsage(TextureUsage::CopySrc)
                                .setFormat(TextureFormat::RGBA8UnormSrgb)
                                .emptyFramebuffer(m_Viewport.x, m_Viewport.y, m_Context);
            m_Framedata.resize(m_Framebuffer->getWidth() * m_Framebuffer->getHeight() * m_Framebuffer->getFormatSize() / sizeof(std::uint32_t));
        }

        for (std::size_t i = 0; i < m_Framedata.size(); i++) {
            m_Framedata[i] = Random::Uint32() | 0xFF000000;
        }

        ImageCopyTexture::New(m_Framebuffer).write(m_Framedata.data(), m_Context);

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
