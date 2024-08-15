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
        YZDEBUG("Initializing Yulduz Application");
        m_Context = GraphicsContextBuilder::New()
                        .setBackend(InstanceBackend::Vulkan)
                        //.setBackend(InstanceBackend::DX12)
                        .setPreferredSurfaceFormat(TextureFormat::RGBA8Unorm)
                        .addSurfaceUsage(TextureUsage::CopyDst)
                        .build(m_Window);
        auto &dispatcher = EventDispatcher::GetDefault();
        dispatcher.addCallback<WindowKeyEvent>(&App::keyCallback, this);
        dispatcher.addCallback<WindowResizeEvent>(&App::resizeCallback, this);

        m_DeltaTime = 0.0;
        m_Time = 0.0;

        auto shaderFileOptional = FileAsset::FromPath(YULDUZ_SOURCE_DIR "/assets/shaders/frame.wgsl");
        if (!shaderFileOptional) {
            YZFATAL("Failed to open shader file!");
            throw std::runtime_error("Failed to open shader file!");
        }
        auto computeShader = ShaderBuilder::New().buildWGSL(shaderFileOptional.value(), m_Context);

        auto [width, height] = m_Window.getSize();

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
                      .empty2d(width, height, m_Context);
        m_FrameUniform = BufferBuilder::New()
                             .emptyUniform(sizeof(FrameData), m_Context);
        m_FrameBindGroup = BindGroupBuilder::New()
                               .addTexture(0, m_Frame)
                               .addUniformBuffer(1, m_FrameUniform)
                               .build(m_FrameBindGroupLayout, m_Context);
    }

    App::~App() {
        YZDEBUG("Releasing Yulduz Application");
    }

    void App::run() {
        static Milliseconds::Timer timer;
        EventDispatcher &dispatcher = EventDispatcher::GetDefault();

        while (m_Window.isRunning()) {
            timer.start();

            Window::PollEvents();
            dispatcher.dispatch();

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
            renderPass.finish();
        }

        commandEncoder.copyTextureToTexture(ImageCopyTexture::New(m_Frame), ImageCopyTexture::New(frame));

        m_Context.sumbitCommands({commandEncoder.finish()});
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
        m_Frame.resize2D(event.width, event.height);
        m_FrameBindGroup = BindGroupBuilder::New()
                               .addTexture(0, m_Frame)
                               .addUniformBuffer(1, m_FrameUniform)
                               .build(m_FrameBindGroupLayout, m_Context);
    }
}  // namespace Yulduz
