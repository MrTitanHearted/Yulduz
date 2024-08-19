#include <Yulduz/Yulduz.hpp>

namespace Yulduz {
    class Vertex {
       public:
        Vertex(glm::vec3 position, glm::vec3 uv) : Position{position}, UV{uv} {}
        Vertex(float x, float y, float z, float u, float v) : Position{x, y, z}, UV{u, v} {}

        inline static Vertex New(glm::vec3 position, glm::vec3 uv) { return Vertex{position, uv}; }
        inline static Vertex New(float x, float y, float z, float u, float v) { return Vertex{x, y, z, u, v}; }

       public:
        glm::vec3 Position;
        glm::vec2 UV;

        static VertexLayout g_VertexLayout;
    };

    VertexLayout Vertex::g_VertexLayout = VertexLayout::New(sizeof(Vertex))
                                              .setAttributes({
                                                  VertexLayout::Attribute{.Format = VertexFormat::Float32x3, .Offset = offsetof(Vertex, Position), .ShaderLocation = 0},
                                                  VertexLayout::Attribute{.Format = VertexFormat::Float32x2, .Offset = offsetof(Vertex, UV), .ShaderLocation = 1},
                                              });

    void App::Run() {
        Window::SetEventObserver(EventObserver::GetDefault());
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
                        // .setBackend(InstanceBackend::DX12)
                        .setPreferredSurfaceFormat(TextureFormat::BGRA8Unorm)
                        .addSurfaceUsage(TextureUsage::CopyDst)
                        .build(m_Window);
        auto &observer = EventObserver::GetDefault();
        observer.addCallback<WindowResizeEvent>(&App::resizeCallback, this);
        observer.addCallback<WindowKeyEvent>(&App::keyCallback, this);
        observer.addCallback<WindowMouseMoveEvent>(&App::mouseMoveCallback, this);
        observer.addCallback<WindowMouseScrollEvent>(&App::mouseScrollCallback, this);

        InitImGui(ImGuiSettings{
            .DepthFormat = TextureFormat::Undefined,
            .Context = m_Context,
        });

        ImGuiIO &io = ImGui::GetIO();
        m_Font = io.Fonts->AddFontFromFileTTF(YULDUZ_SOURCE_DIR "/assets/fonts/JetBrainsMonoNerdFont-Medium.ttf", 18.0f);

        m_DeltaTime = 0.0;
        m_Time = 0.0;

        auto shaderFileOptional = FileAsset::FromPath(YULDUZ_SOURCE_DIR "/assets/shaders/triangle.wgsl");
        if (!shaderFileOptional) {
            YZFATAL("Failed to open shader file!");
            throw std::runtime_error("Failed to open shader file!");
        }
        auto textureOptional = TextureAsset::FromPath(YULDUZ_SOURCE_DIR "/assets/textures/awesomeface.png");
        if (!textureOptional) {
            YZFATAL("Failed to open texture file!");
            throw std::runtime_error("Failed to open texture file!");
        }

        auto triangleShader = ShaderBuilder::New().buildWGSL(shaderFileOptional.value(), m_Context);
        auto sampler = SamplerBuilder::New().build(m_Context);
        auto texture = TextureBuilder::New().build(textureOptional.value(), m_Context);

        m_Frame = TextureBuilder::New()
                      .setUsage(TextureUsage::RenderAttachment | TextureUsage::CopySrc | TextureUsage::CopyDst)
                      .setFormat(m_Context.getSurfaceFormat())
                      .empty2d(1, 1, m_Context);

        auto cameraBindGroupLayout = BindGroupLayoutBuilder::New()
                                         .addUniformBuffer(0)
                                         .addUniformBuffer(1)
                                         .build(m_Context);
        auto triangleBindGroupLayout = BindGroupLayoutBuilder::New()
                                           .addSampler(0)
                                           .addTexture2D(1)
                                           .build(m_Context);
        auto trianglePipelineLayout = PipelineLayoutBuilder::New()
                                          .setBindGroupLayouts({cameraBindGroupLayout, triangleBindGroupLayout})
                                          .build(m_Context);
        auto trianglePipeline = RenderPipelineBuilder::New()
                                    .setFragmentStateReq(FragmentState::New(triangleShader).addColorTargetState(ColorTargetState::New(m_Frame.getFormat())))
                                    .setVertexStateReq(VertexState::New(triangleShader).addVertexLayout(Vertex::g_VertexLayout))
                                    .build(trianglePipelineLayout, m_Context);

        m_CameraUniformBuffer = BufferBuilder::New().emptyUniform(sizeof(CameraData), m_Context);
        m_ModelUniformBuffer = BufferBuilder::New().emptyUniform(sizeof(glm::mat4), m_Context);

        auto cameraBindGroup = BindGroupBuilder::New()
                                   .addBuffer(0, m_ModelUniformBuffer)
                                   .addBuffer(1, m_CameraUniformBuffer)
                                   .build(cameraBindGroupLayout, m_Context);
        auto triangleBindGroup = BindGroupBuilder::New()
                                     .addSampler(0, sampler)
                                     .addTexture(1, texture)
                                     .build(triangleBindGroupLayout, m_Context);
        auto triangleVertexBuffer =
            BufferBuilder::New().buildVertex<Vertex>(
                {
                    Vertex::New(-1.0, -1.0, 0.0, 0.0, 0.0),
                    Vertex::New(0.0, 1.0, 0.0, 0.5, 1.0),
                    Vertex::New(1.0, -1.0, 0.0, 1.0, 0.0),
                },
                m_Context);
        auto triangleIndexBuffer =
            BufferBuilder::New().buildIndex(std::vector<std::uint32_t>{0, 1, 2}, m_Context);

        auto triangleRenderBundleEncoder = RenderBundleEncoderBuilder::New()
                                               .addColorFormat(m_Frame.getFormat())
                                               .build(m_Context);
        triangleRenderBundleEncoder.setPipeline(trianglePipeline);
        triangleRenderBundleEncoder.setBindGroups({cameraBindGroup, triangleBindGroup});
        triangleRenderBundleEncoder.setVertexBuffer(0, triangleVertexBuffer);
        triangleRenderBundleEncoder.drawIndexed(triangleIndexBuffer);
        m_TriangleRenderBundle = triangleRenderBundleEncoder.finish();

        m_CameraMultiplier = 20.0f;
    }

    App::~App() {
        YZDEBUG("Releasing Yulduz Application");
        ShutdownImGui();
    }

    void App::run() {
        static Milliseconds::Timer timer;

        while (m_Window.isRunning()) {
            timer.start();

            Window::PollEvents();

            moveCamera();

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
            glm::mat4 modelData{1.0f};
            CameraData cameraData{
                .Projection = m_Camera.getProjection(),
                .View = m_Camera.getView(),
            };

            m_ModelUniformBuffer.write(&modelData);
            m_CameraUniformBuffer.write(&cameraData);

            auto renderPass = RenderPassBuilder::New()
                                  .setLabel("Triangle Pass")
                                  .setColorAttachments({ColorAttachment::New(m_Frame).setClearColor3({0.2f, 0.3f, 0.3f})})
                                  .build(commandEncoder);
            renderPass.executeBundle(m_TriangleRenderBundle);
            renderPass.finish();
        }

        {
            auto renderPass = RenderPassBuilder::New()
                                  .setLabel("ImGui Pass")
                                  .setColorAttachments({ColorAttachment::New(frame)})
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

        ImGui::Begin("Settings");
        ImGui::Text("Camera Multiplier when pressed Shift key:");
        ImGui::SameLine();
        ImGui::DragFloat("##DragFloat", &m_CameraMultiplier, 0.1f, 1.0f, 100.0f);
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

        ImGui::Image(m_Frame.getDefaultView(), m_ViewportSize);

        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::PopFont();
    }

    void App::resizeFrame() {
        auto [width, height] = m_ViewportSize;
        auto [prevWidth, prevHeight] = m_Frame.getSize2D();
        if (width == prevWidth && height == prevHeight) return;

        m_Frame.resize2D(width, height);
        m_Camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    }

    void App::moveCamera() {
        if (!m_Window.isMouseButtonDown(MouseButton::Right)) {
            if (m_Window.getCursorMode() != CursorMode::Normal)
                m_Window.setCursorMode(CursorMode::Normal);
            m_Camera.setFirstMouse(true);
            return;
        }

        if (m_Window.getCursorMode() != CursorMode::Disabled)
            m_Window.setCursorMode(CursorMode::Disabled);

        if (m_Window.isKeyDown(KeyCode::W))
            m_Camera.move(CameraMovement::WorldForward, m_DeltaTime);
        if (m_Window.isKeyDown(KeyCode::S))
            m_Camera.move(CameraMovement::WorldBackward, m_DeltaTime);
        if (m_Window.isKeyDown(KeyCode::D))
            m_Camera.move(CameraMovement::Right, m_DeltaTime);
        if (m_Window.isKeyDown(KeyCode::A))
            m_Camera.move(CameraMovement::Left, m_DeltaTime);
        if (m_Window.isKeyDown(KeyCode::E))
            m_Camera.move(CameraMovement::WorldUp, m_DeltaTime);
        if (m_Window.isKeyDown(KeyCode::Q))
            m_Camera.move(CameraMovement::WorldDown, m_DeltaTime);
    }

    void App::keyCallback(const WindowKeyEvent &event) {
        if (event.action == KeyAction::Press) {
            if (event.key == KeyCode::Escape) m_Window.close();
            if (event.key == KeyCode::F) {
                if (m_Window.isFullscreen())
                    m_Window.makeWindowed();
                else
                    m_Window.makeFullscreen();
                auto [width, height] = m_Window.getSize();
                m_Context.resize(width, height);
            }
            if (event.key == KeyCode::LeftShift || event.key == KeyCode::RightShift)
                m_Camera.setSpeed(m_Camera.getSpeed() * m_CameraMultiplier);
        } else if (event.action == KeyAction::Release) {
            if (event.key == KeyCode::LeftShift || event.key == KeyCode::RightShift)
                m_Camera.setSpeed(m_Camera.getSpeed() / m_CameraMultiplier);
        }
    }

    void App::resizeCallback(const WindowResizeEvent &event) {
        m_Context.resize(event.width, event.height);
    }

    void App::mouseMoveCallback(const WindowMouseMoveEvent &event) {
        if (m_Window.isMouseButtonDown(MouseButton::Right)) {
            m_Window.setCursorMode(CursorMode::Disabled);
            m_Camera.moveMouse(event.x, event.y);
        } else {
            if (m_Window.getCursorMode() != CursorMode::Normal)
                m_Window.setCursorMode(CursorMode::Normal);
            m_Camera.setFirstMouse(true);
        }
    }

    void App::mouseScrollCallback(const WindowMouseScrollEvent &event) {
        if (!m_Window.isMouseButtonDown(MouseButton::Right)) return;
        m_Camera.scrollMouse(event.yOffset);
    }
}  // namespace Yulduz
