#include <Yulduz/Yulduz.hpp>

namespace Yulduz {
    void App::Run() {
        Window::SetEventDispatcher(EventDispatcher::GetDefault());
        App app{};

        try {
            app.run();
        } catch (const std::exception &e) {
            YZFATAL("Caught Error: '{}'", e.what());
            app.~App();
        } catch (...) {
            YZFATAL("Caught Unknown Errors!");
            app.~App();
        }
    }

    App::App()
        : m_Window{Window::Settings{
              .Width = 1200,
              .Height = 800,
          }} {
        m_Window.addResizeCallback(&App::resizeCallback, this);
        YZDEBUG("Initializing Yulduz Application");
        m_Context = GraphicsContextBuilder::New()
                        .setBackend(InstanceBackend::Vulkan)
                        //.setBackend(InstanceBackend::DX12)
                        .setPreferredSurfaceFormat(TextureFormat::BGRA8Unorm)
                        .addSurfaceUsage(TextureUsage::CopyDst)
                        .build(m_Window);
        auto &dispatcher = EventDispatcher::GetDefault();
        dispatcher.addCallback<WindowKeyEvent>(&App::keyCallback, this);

        InitImGui(ImGuiSettings{
            .DepthFormat = TextureFormat::Undefined,
            .Context = m_Context,
        });

        ImGuiIO &io = ImGui::GetIO();
        m_Font = io.Fonts->AddFontFromFileTTF(YULDUZ_SOURCE_DIR "/assets/fonts/JetBrainsMonoNerdFont-Medium.ttf", 18.0f);

        m_DeltaTime = 0.0;
        m_Time = 0.0;

        auto shaderFileOptional = FileAsset::FromPath(YULDUZ_SOURCE_DIR "/assets/shaders/frame.wgsl");
        if (!shaderFileOptional) {
            YZFATAL("Failed to open shader file!");
            throw std::runtime_error("Failed to open shader file!");
        }
        auto computeShader = ShaderBuilder::New().buildWGSL(shaderFileOptional.value(), m_Context);

        m_FrameBindGroupLayout = BindGroupLayoutBuilder::New()
                                     .addStorageTexture2D(0, TextureFormat::RGBA8Unorm)
                                     .addUniformBuffer(1)
                                     .build(m_Context);
        auto pipelineLayout = PipelineLayoutBuilder::New()
                                  .setBindGroupLayouts({m_FrameBindGroupLayout})
                                  .build(m_Context);
        m_FramePipeline = ComputePipelineBuilder::New()
                              .setComputeStateReq(ComputeState::New(computeShader))
                              .build(pipelineLayout, m_Context);
        m_Frame = TextureBuilder::New()
                      .setUsage(TextureUsage::StorageBinding | TextureUsage::CopySrc | TextureUsage::CopyDst)
                      .setFormat(TextureFormat::RGBA8Unorm)
                      .empty2d(1, 1, m_Context);
        m_FrameUniform = BufferBuilder::New()
                             .emptyUniform(sizeof(FrameData), m_Context);
        m_FrameBindGroup = BindGroupBuilder::New()
                               .addTexture(0, m_Frame)
                               .addUniformBuffer(1, m_FrameUniform)
                               .build(m_FrameBindGroupLayout, m_Context);
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
        {
            FrameData data{
                .DeltaTime = (float)m_DeltaTime,
                .Time = (float)m_Time,
            };
            m_FrameUniform.write(&data);

            auto [width, height] = m_Frame.getSize2D();

            auto computePass = ComputePassBuilder::New().build(commandEncoder);
            computePass.setPipeline(m_FramePipeline);
            computePass.setBindGroups({m_FrameBindGroup});
            computePass.dispatch({(width + 15) / 16, (height + 15) / 16, 1});
            computePass.finish();
        }
        {
            auto renderPass = RenderPassBuilder::New()
                                  .setColorAttachments({ColorAttachment::New(frame).setClearColor3({0.2f, 0.3f, 0.3f})})
                                  .build(commandEncoder);
            RenderImGuiPass(renderPass);
            renderPass.finish();
        }

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

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");

        m_ViewportSize = ImGui::GetContentRegionAvail();
        resizeFrame();

        ImGui::Image(m_Frame.getDefaultView(), m_ViewportSize, ImVec2{0, 1}, ImVec2{1, 0});
        
        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::PopFont();
    }

    void App::resizeFrame() {
        auto [width, height] = m_ViewportSize;
        auto [prevWidth, prevHeight] = m_Frame.getSize2D();
        if (width == prevWidth && height == prevHeight) return;

        m_Frame.resize2D(width, height);
        m_FrameBindGroup = BindGroupBuilder::New()
                               .addTexture(0, m_Frame)
                               .addUniformBuffer(1, m_FrameUniform)
                               .build(m_FrameBindGroupLayout, m_Context);
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
        m_Context.resize(event.width, event.height);
    }
}  // namespace Yulduz
