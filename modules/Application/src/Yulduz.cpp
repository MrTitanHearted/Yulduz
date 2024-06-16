#include <Yulduz/Yulduz.hpp>

namespace Yulduz {
    class Vertex {
       public:
        Vertex(float x, float y, float z, float u, float v) {
            m_Position = glm::vec3(x, y, z);
            m_UV = glm::vec2(u, v);
        }

        static VertexLayout GetLayout() {
            return VertexLayout(sizeof(Vertex))
                .addAttribute(VertexFormat::Float32x3, offsetof(Vertex, m_Position), 0)
                .addAttribute(VertexFormat::Float32x2, offsetof(Vertex, m_UV), 1);
        }

       private:
        glm::vec3 m_Position;
        glm::vec2 m_UV;
    };

    void App::Run() {
        App app;
        app.run();
    }

    App::App() {
        m_Window = Window::New(Window::Settings{
            .eventDispatcher = m_EventDispatcher,
        });
        m_Context = RenderContextBuilder::New()
                        .setBackend(RenderContextBackend::Vulkan)
                        .addSurfaceUsage(TextureUsage::CopySrc)
                        .build(m_Window);
        m_Context->registerCallbacks(m_EventDispatcher);
        m_EventDispatcher.addCallback<WindowKeyEvent>(std::bind(&App::keyCallback, this, std::placeholders::_1));
        m_EventDispatcher.addCallback<WindowMouseButtonEvent>(std::bind(&App::mouseButtonCallback, this, std::placeholders::_1));
        m_EventDispatcher.addCallback<WindowResizeEvent>(std::bind(&App::resizeCallback, this, std::placeholders::_1));
        m_EventDispatcher.addCallback<WindowMouseMoveEvent>(std::bind(&App::mouseMoveCallback, this, std::placeholders::_1));
        m_EventDispatcher.addCallback<WindowMouseScrollEvent>(std::bind(&App::mouseScrollCallback, this, std::placeholders::_1));
        RenderContext::SetupWGPULogging(WebGPULogLevel::Info);
        InitImGui(m_Context);

        ImGuiIO &io = ImGui::GetIO();
        m_Font = io.Fonts->AddFontFromFileTTF("assets/fonts/JetBrainsMonoNerdFont-Medium.ttf", 18.0f);

        auto shaderAssetOption = FileAsset::FromPath("assets/shaders/triangle.wgsl");
        if (!shaderAssetOption) {
            YZFATAL("Failed to load shader!");
            throw std::runtime_error("Failed to load shader!");
        }
        auto textureAssetOption = TextureAsset::FromPath("assets/textures/awesomeface.png");
        if (!textureAssetOption) {
            YZFATAL("Failed to load texture!");
            throw std::runtime_error("Failed to load texture!");
        }

        std::vector<Vertex> vertices{
            Vertex(-0.5f, -0.5f, 0.0f, 0.0f, 0.0f),
            Vertex(0.0f, 0.5f, 0.0f, 0.5f, 1.0f),
            Vertex(0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
        };
        std::vector<uint32_t> indices{0, 1, 2};
        glm::mat4 model(1.0f);
        auto modelUniform =
            UniformBufferBuilder::New()
                .setLabel("Model Uniform")
                .build(&model, sizeof(glm::mat4), m_Context);
        m_ProjectionUniform =
            UniformBufferBuilder::New()
                .setLabel("Camera Projection Uniform")
                .empty(sizeof(glm::mat4), m_Context);
        m_ViewUniform =
            UniformBufferBuilder::New()
                .setLabel("Camera View Uniform")
                .empty(sizeof(glm::mat4), m_Context);

        auto cameraBindGroupLayout =
            BindGroupLayoutBuilder::New()
                .addUniformBuffer(0)
                .addUniformBuffer(1)
                .addUniformBuffer(2)
                .build(m_Context);
        auto bindGroupLayout =
            BindGroupLayoutBuilder::New()
                .addSampler(0)
                .addTexture2D(1)
                .build(m_Context);
        auto pipelineLayout = PipelineLayoutBuilder::New()
                                  .addBindGroupLayout(cameraBindGroupLayout)
                                  .addBindGroupLayout(bindGroupLayout)
                                  .build(m_Context);
        auto shader = ShaderBuilder::New().buildWGSL(shaderAssetOption.value(), m_Context);
        auto sampler = SamplerBuilder::New().build(m_Context);
        auto texture = TextureBuilder::New().build(textureAssetOption.value(), m_Context);
        auto cameraBindGroup =
            BindGroupBuilder::New()
                .addUniformBuffer(0, modelUniform)
                .addUniformBuffer(1, m_ProjectionUniform)
                .addUniformBuffer(2, m_ViewUniform)
                .build(cameraBindGroupLayout, m_Context);
        auto bindGroup =
            BindGroupBuilder::New()
                .addSampler(0, sampler)
                .addTexture(1, texture)
                .build(bindGroupLayout, m_Context);
        auto vertexBuffer = VertexBufferBuilder::New().build(vertices, m_Context);
        auto indexBuffer = IndexBufferBuilder::New().build(indices, m_Context);
        auto renderPipeline = RenderPipelineBuilder::New()
                                  .setVertexState(VertexState(shader).addVertexLayout(Vertex::GetLayout()))
                                  .setFragmentState(FragmentState(shader).addColorTargetState(ColorTargetState(m_Context->getSurfaceFormat())))
                                  .setDepthStencilState(DepthStencilState())
                                  .build(pipelineLayout, m_Context);
        auto renderBundleEncoder = RenderBundleEncoderBuilder::New()
                                       .addColorFormat(m_Context->getSurfaceFormat())
                                       .setDepthStencilFormat(m_Context->getDepthTexture()->getFormat())
                                       .build(m_Context);
        renderBundleEncoder->setRenderPipeline(renderPipeline);
        renderBundleEncoder->setBindGroup(0, cameraBindGroup);
        renderBundleEncoder->setBindGroup(1, bindGroup);
        renderBundleEncoder->setVertexBuffer(0, vertexBuffer);
        renderBundleEncoder->drawIndexed(indexBuffer);
        m_RenderBundle = renderBundleEncoder->finish();

        auto [width, height] = m_Window->getSize();
        m_Depthbuffer = TextureBuilder::New()
                            .setLabel("Yulduz Main Depth Buffer")
                            .setFormat(TextureFormat::Depth32Float)
                            .emptyFramebuffer(width, height, m_Context);
        m_Framebuffer = TextureBuilder::New()
                            .setLabel("ImGui Framebuffer")
                            .addTextureUsage(TextureUsage::CopySrc)
                            .emptyFramebuffer(width, height, m_Context);

        m_Camera.setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
    }

    App::~App() {
        ShutdownImGui();
    }

    void App::run() {
        while (m_Window->isRunning()) {
            m_Timer.start();

            Window::PollEvents();
            m_EventDispatcher.dispatch();

            if (m_Window->isKeyDown(KeyCode::W) && m_Window->getCursorMode() == CursorMode::DISABLED)
                m_Camera.move(CameraMovement::WorldForward, m_Timer.getElapsedSeconds());
            if (m_Window->isKeyDown(KeyCode::S) && m_Window->getCursorMode() == CursorMode::DISABLED)
                m_Camera.move(CameraMovement::WorldBackward, m_Timer.getElapsedSeconds());
            if (m_Window->isKeyDown(KeyCode::D) && m_Window->getCursorMode() == CursorMode::DISABLED)
                m_Camera.move(CameraMovement::Right, m_Timer.getElapsedSeconds());
            if (m_Window->isKeyDown(KeyCode::A) && m_Window->getCursorMode() == CursorMode::DISABLED)
                m_Camera.move(CameraMovement::Left, m_Timer.getElapsedSeconds());
            if (m_Window->isKeyDown(KeyCode::SPACE) && m_Window->getCursorMode() == CursorMode::DISABLED)
                m_Camera.move(CameraMovement::WorldUp, m_Timer.getElapsedSeconds());
            if (m_Window->isKeyDown(KeyCode::LEFT_SHIFT) && m_Window->getCursorMode() == CursorMode::DISABLED)
                m_Camera.move(CameraMovement::WorldDown, m_Timer.getElapsedSeconds());

            glm::mat4 projection = m_Camera.getProjection();
            glm::mat4 view = m_Camera.getView();

            m_ProjectionUniform->write(&projection, m_Context);
            m_ViewUniform->write(&view, m_Context);

            ImGuiFrame(std::bind(&App::renderImGui, this));

            m_Context->renderFrameOnSurface(std::bind(&App::renderFrame, this, std::placeholders::_1));

            m_Timer.stop();
        }
    }

    void App::renderFrame(const std::shared_ptr<Framebuffer> &frame) {
        std::vector<std::shared_ptr<CommandBuffer>> commands;
        std::shared_ptr<CommandEncoder> encoder = CommandEncoderBuilder::New().build(m_Context);
        std::shared_ptr<RenderPass> renderPass = RenderPassBuilder::New()
                                                     .setLabel("Yulduz Render Pass")
                                                     .addColorAttachment(ColorAttachment::New(m_Framebuffer)
                                                                             .withClearColor(0.1f, 0.3f, 0.3f, 1.0f))
                                                     .setDepthStencilAttachment(DepthStencilAttachment::New(m_Depthbuffer))
                                                     .build(encoder);
        if (!static_cast<int>(m_Framebuffer->getUsage() & TextureUsage::RenderAttachment)) LOG_WARN("HEY");
        std::vector bundles{m_RenderBundle};
        renderPass->executeRenderBundles(bundles);
        renderPass->finish();

        renderPass = RenderPassBuilder::New()
                         .setLabel("Yulduz ImGui Render Pass")
                         .addColorAttachment(ColorAttachment::New(frame))
                         .setDepthStencilAttachment(DepthStencilAttachment::New(m_Context->getDepthTexture()))
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

        if (ImGui::Begin("Game View")) {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 windowPos = ImGui::GetWindowPos();
            ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
            ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
            ImVec2 contentSize = ImGui::GetContentRegionAvail();

            ImVec2 contentMinScreen = ImVec2(windowPos.x + contentMin.x, windowPos.y + contentMin.y);
            ImVec2 contentMaxScreen = ImVec2(windowPos.x + contentMax.x, windowPos.y + contentMax.y);
            m_IsInGameWindow =
                mousePos.x >= contentMinScreen.x && mousePos.x <= contentMaxScreen.x &&
                mousePos.y >= contentMinScreen.y && mousePos.y <= contentMaxScreen.y &&
                ImGui::IsWindowHovered();
            ImVec2 size = ImGui::GetWindowSize();
            ImGui::Image(m_Framebuffer->getView(), ImVec2(contentSize.x, contentSize.y));
            m_Camera.setAspectRatio(contentSize.x / contentSize.y);
        } else {
            m_IsInGameWindow = false;
        }
        ImGui::Begin("Camera");
        glm::vec3 position = m_Camera.getPosition();
        ImGui::Text("Position: x=%.6f, y=%.6f, z=%.6f", position.x, position.y, position.z);
        ImGui::End();
        ImGui::PopFont();
        ImGui::End();
    }

    void App::keyCallback(const WindowKeyEvent &event) {
        if (event.action != KeyAction::PRESS) return;
        if (event.key == KeyCode::ESCAPE) m_Window->close();
        if (event.key == KeyCode::R) m_Context->printWGPUReport();
    }

    void App::mouseButtonCallback(const WindowMouseButtonEvent &event) {
        if (event.action != KeyAction::PRESS) return;

        if (event.button == MouseButton::LEFT && m_IsInGameWindow && m_Window->getCursorMode() != CursorMode::DISABLED)
            m_Window->setCursorMode(CursorMode::DISABLED);
        else if (event.button == MouseButton::RIGHT && m_Window->getCursorMode() != CursorMode::NORMAL)
            m_Window->setCursorMode(CursorMode::NORMAL);
    }

    void App::resizeCallback(const WindowResizeEvent &event) {
        m_Depthbuffer = TextureBuilder::New()
                            .setLabel("Yulduz Main Depth Buffer")
                            .setFormat(TextureFormat::Depth32Float)
                            .emptyFramebuffer(event.width, event.height, m_Context);
        m_Framebuffer = TextureBuilder::New()
                            .setLabel("ImGui Framebuffer")
                            .addTextureUsage(TextureUsage::CopySrc)
                            .emptyFramebuffer(event.width, event.height, m_Context);
    }

    void App::mouseMoveCallback(const WindowMouseMoveEvent &event) {
        if (m_Window->getCursorMode() != CursorMode::DISABLED) return;
        m_Camera.moveMouse(event.x, event.y);
    }

    void App::mouseScrollCallback(const WindowMouseScrollEvent &event) {
        if (!m_IsInGameWindow) return;
        m_Camera.scrollMouse(event.yOffset);
    }

}  // namespace Yulduz
