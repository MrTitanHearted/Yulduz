#include <Yulduz/Yulduz.hpp>

namespace Yulduz {
    void App::Run() {
        App app;
        app.run();
    }

    App::App()
        : m_DeltaTime{0},
          m_EventDispatchTime{0},
          m_ImGuiTime{0},
          m_RenderTime{0},
          m_ImGuiPassTime{0},
          m_CommandSubmissionTime{0},
          m_Time{0} {
        m_EventDispatcher = std::make_shared<EventDispatcher>();
        m_Window = Window::New(Window::Settings{
            .Title = "Yulduz Cherno Raytracing",
            .Width = 1200,
            .Height = 800,
            .EventDispatcher = m_EventDispatcher,
        });
        auto [width, height] = m_Window->getSize();
        m_Context = RenderContextBuilder::New().build(m_Window);
        m_EventDispatcher->addCallback<WindowKeyEvent>(std::bind(&App::keyCallback, this, std::placeholders::_1));
        m_EventDispatcher->addCallback<WindowResizeEvent>(std::bind(&App::resizeCallback, this, std::placeholders::_1));
        m_EventDispatcher->addCallback<WindowMouseMoveEvent>(std::bind(&App::mouseMoveCallback, this, std::placeholders::_1));
        m_EventDispatcher->addCallback<WindowMouseScrollEvent>(std::bind(&App::mouseScrollCallback, this, std::placeholders::_1));
        RenderContext::SetupWGPULogging(WebGPULogLevel::Error);
        InitImGui(m_Context);
        ImGuiIO &io = ImGui::GetIO();
        m_Font = io.Fonts->AddFontFromFileTTF(YULDUZ_SOURCE_DIR "/assets/fonts/JetBrainsMonoNerdFont-Medium.ttf", 18.0f);
        m_Depthbuffer = TextureBuilder::New()
                            .setLabel("ImGui Depthbuffer")
                            .setFormat(TextureFormat::Depth32Float)
                            .emptyFramebuffer(width, height, m_Context);
        m_RayTracer = std::make_shared<RayTracer>(RayTracer::Settings{.RenderContext = m_Context});

        m_Scene.Materials.emplace_back(Material{
            .Albedo = glm::vec3{1.0f, 0.0f, 1.0f},
            .Roughness = 0.0f,
        });
        m_Scene.Materials.emplace_back(Material{
            .Albedo = glm::vec3{0.2f, 0.3f, 1.0f},
            .Roughness = 0.1f,
        });
        Material &orangeSphere = m_Scene.Materials.emplace_back(Material{
            .Albedo = glm::vec3{0.8f, 0.5f, 0.2f},
            .EmissionPower = 2.0f,
            .Roughness = 0.1f,
        });
        orangeSphere.EmissionColor = orangeSphere.Albedo;

        m_Scene.Spheres.emplace_back(Sphere{
            .Position = glm::vec3{0.0f},
            .Radius = 1.0f,
            .MaterialIndex = 0,
        });
        m_Scene.Spheres.emplace_back(Sphere{
            .Position = glm::vec3{2.0f, 0.0f, 0.0f},
            .Radius = 1.0f,
            .MaterialIndex = 2,
        });
        m_Scene.Spheres.emplace_back(Sphere{
            .Position = glm::vec3{0.0f, -101.0f, 0.0f},
            .Radius = 100.0f,
            .MaterialIndex = 1,
        });

        m_Camera.setPosition(glm::vec3{0, 0, 6});
    }

    App::~App() {
        ShutdownImGui();
    }

    void App::run() {
        while (m_Window->isRunning()) {
            static Milliseconds::Timer timer;
            static Milliseconds::Timer eventTimer;
            static Milliseconds::Timer imguiTimer;
            static Milliseconds::Timer renderTimer;

            timer.start();

            eventTimer.start();
            Window::PollEvents();
            m_EventDispatcher->dispatch();
            eventTimer.stop();
            m_EventDispatchTime = eventTimer.getElapsed();

            if (m_Window->isMouseButtonDown(MouseButton::RIGHT)) {
                m_Window->setCursorMode(CursorMode::DISABLED);
            } else {
                if (m_Window->getCursorMode() != CursorMode::NORMAL)
                    m_Window->setCursorMode(CursorMode::NORMAL);
                m_Camera.setFirstMouse(true);
            }

            moveCamera();

            imguiTimer.start();
            ImGuiFrame(std::bind(&App::renderImGui, this));
            imguiTimer.stop();
            m_ImGuiTime = imguiTimer.getElapsed();

            renderTimer.start();
            m_Context->renderFrameOnSurface(std::bind(&App::renderFrame, this, std::placeholders::_1));
            renderTimer.stop();
            m_RenderTime = renderTimer.getElapsed();

            timer.stop();
            m_DeltaTime = timer.getElapsed();
            m_Time += m_DeltaTime;
        }
    }

    void App::renderFrame(const std::shared_ptr<Framebuffer> &frame) {
        static Milliseconds::Timer computePassTimer;
        static Milliseconds::Timer imguiPassTimer;
        static Milliseconds::Timer commandSubmissionTimer;

        std::vector<std::shared_ptr<CommandBuffer>> commands;
        std::shared_ptr<CommandEncoder> encoder = CommandEncoderBuilder::New().build(m_Context);

        computePassTimer.start();
        m_RayTracer->updateSceneBuffers(m_Scene);
        std::shared_ptr<ComputePass> computePass = ComputePassBuilder::New().build(encoder);
        m_RayTracer->render(m_Time, m_Camera, computePass);
        computePass->finish();
        computePassTimer.stop();
        m_ComputePassTime = computePassTimer.getElapsed();

        imguiPassTimer.start();
        std::shared_ptr<RenderPass> renderPass =
            RenderPassBuilder::New()
                .setLabel("Yulduz ImGui Render Pass")
                .addColorAttachment(ColorAttachment::New(frame))
                .setDepthStencilAttachment(DepthStencilAttachment::New(m_Depthbuffer))
                .build(encoder);
        RenderImGui(renderPass);
        renderPass->finish();
        imguiPassTimer.stop();
        m_ImGuiPassTime = imguiPassTimer.getElapsed();

        commands.emplace_back(encoder->finish());

        commandSubmissionTimer.start();
        m_Context->submitCommands(commands);
        commandSubmissionTimer.stop();
        m_CommandSubmissionTime = commandSubmissionTimer.getElapsed();
    }

    void App::renderImGui() {
        ImGuiIO &io = ImGui::GetIO();
        ImGui::DockSpaceOverViewport();
        ImGui::PushFont(m_Font);

        ImGui::Begin("Settings");
        ImGui::Text("Delta Time: %.3fms", m_DeltaTime);
        ImGui::Text("Event Dispatch Time: %.3fms", m_EventDispatchTime);
        ImGui::Text("ImGui Time: %.3fms", m_ImGuiTime);
        ImGui::Text("Render Time: %.3fms", m_RenderTime);
        ImGui::Text("\tCompute Pass Time: %.3fms", m_ComputePassTime);
        ImGui::Text("\tImGui Pass Time: %.3fms", m_ImGuiPassTime);
        ImGui::Text("\tCommand Submission Time: %.3fms", m_CommandSubmissionTime);

        ImGui::Separator();

        RayTracer::Options &options = m_RayTracer->getOptionsRef();

        ImGui::Checkbox("Accumulate", &options.Accumulate);
        if (ImGui::Button("Reset"))
            m_RayTracer->reset();
        ImGui::DragInt("Max Ray Bounces", &options.Bounces, 1, 1, 50);

        ImGui::Checkbox("Add Sky", &options.AddSky);
        if (options.AddSky)
            ImGui::ColorEdit3("Sky Color", glm::value_ptr(options.SkyColor));

        ImGui::Separator();

        ImGui::Text("Current Present Mode: %s", GetPresentMode(m_Context->getPresentMode()));
        ImGui::Text("Present Modes");
        SurfaceCapabilities caps = m_Context->getSurfaceCapabilities();
        static std::uint32_t selected = static_cast<std::uint32_t>(m_Context->getPresentMode());
        for (const PresentMode &mode : caps.PresentModes) {
            if (ImGui::Selectable(GetPresentMode(mode), selected == static_cast<std::uint32_t>(mode)) && m_Context->getPresentMode() != mode) {
                selected = static_cast<std::uint32_t>(mode);
                m_Context->setPresentMode(mode);
            }
        }

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::End();

        ImGui::Begin("Scene");
        ImGui::Spacing();
        ImGui::Spacing();

        static bool addSphere = false;
        static bool addMaterial = false;
        static Sphere dummySphere{};
        static Material dummyMaterial{};

        ImGui::Checkbox("Add Sphere", &addSphere);
        if (addSphere) {
            ImGui::Text("Sphere parameters");
            ImGui::DragFloat3("Position", glm::value_ptr(dummySphere.Position), 0.1f);
            ImGui::DragFloat("Radius", &dummySphere.Radius, 0.1f);
            ImGui::DragInt("Material", &dummySphere.MaterialIndex, 1.0f, 0, static_cast<std::int32_t>(m_Scene.Materials.size() - 1));

            if (ImGui::Button("Reset Default Values"))
                dummySphere = Sphere{};
            if (ImGui::Button("Add"))
                m_Scene.Spheres.emplace_back(dummySphere);
        }
        ImGui::Separator();
        ImGui::Checkbox("Add Material", &addMaterial);
        if (addMaterial) {
            ImGui::Text("Material parameters");
            ImGui::ColorEdit3("Albedo", glm::value_ptr(dummyMaterial.Albedo));
            ImGui::DragFloat("Roughness", &dummyMaterial.Roughness, 0.05f, 0.0f, 1.0f);
            ImGui::DragFloat("Metallic", &dummyMaterial.Metallic, 0.05f, 0.0f, 1.0f);
            ImGui::ColorEdit3("Emission Color", glm::value_ptr(dummyMaterial.EmissionColor));
            ImGui::DragFloat("Emission Power", &dummyMaterial.EmissionPower, 0.05f, 0.0f, FLT_MAX);

            if (ImGui::Button("Reset Default Values"))
                dummyMaterial = Material{};
            if (ImGui::Button("Add"))
                m_Scene.Materials.emplace_back(dummyMaterial);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Spheres");
        for (std::size_t i = 0; i < m_Scene.Spheres.size(); i++) {
            ImGui::PushID(i);

            Sphere &sphere = m_Scene.Spheres[i];
            ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
            ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
            ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, static_cast<std::int32_t>(m_Scene.Materials.size() - 1));

            ImGui::Separator();

            ImGui::PopID();
        }
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::Text("Materials");
        for (std::size_t i = 0; i < m_Scene.Materials.size(); i++) {
            ImGui::PushID(i);

            Material &material = m_Scene.Materials[i];
            ImGui::ColorEdit3("Albedo", glm::value_ptr(material.Albedo));
            ImGui::DragFloat("Roughness", &material.Roughness, 0.05f, 0.0f, 1.0f);
            ImGui::DragFloat("Metallic", &material.Metallic, 0.05f, 0.0f, 1.0f);
            ImGui::ColorEdit3("Emission Color", glm::value_ptr(material.EmissionColor));
            ImGui::DragFloat("Emission Power", &material.EmissionPower, 0.05f, 0.0f, FLT_MAX);

            ImGui::Separator();

            ImGui::PopID();
        }
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");
        ImGui::Spacing();
        ImGui::Spacing();

        m_Viewport = ImGui::GetContentRegionAvail();

        m_Camera.setAspectRatio(m_Viewport.x / m_Viewport.y);
        m_RayTracer->resize(m_Viewport.x, m_Viewport.y);

        std::shared_ptr<Framebuffer> image = m_RayTracer->getFinalImageRef();
        ImGui::Image(image->getView(), ImVec2{static_cast<float>(image->getWidth()), static_cast<float>(image->getHeight())}, ImVec2{0, 1}, ImVec2{1, 0});

        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::PopFont();
    }

    void App::moveCamera() {
        if (!m_Window->isMouseButtonDown(MouseButton::RIGHT)) return;

        bool reset = false;
        if (m_Window->isKeyDown(KeyCode::W)) {
            m_Camera.move(CameraMovement::WorldForward, m_DeltaTime);
            reset = true;
        }
        if (m_Window->isKeyDown(KeyCode::S)) {
            m_Camera.move(CameraMovement::WorldBackward, m_DeltaTime);
            reset = true;
        }
        if (m_Window->isKeyDown(KeyCode::D)) {
            m_Camera.move(CameraMovement::Right, m_DeltaTime);
            reset = true;
        }
        if (m_Window->isKeyDown(KeyCode::A)) {
            m_Camera.move(CameraMovement::Left, m_DeltaTime);
            reset = true;
        }
        if (m_Window->isKeyDown(KeyCode::E)) {
            m_Camera.move(CameraMovement::WorldUp, m_DeltaTime);
            reset = true;
        }
        if (m_Window->isKeyDown(KeyCode::Q)) {
            m_Camera.move(CameraMovement::WorldDown, m_DeltaTime);
            reset = true;
        }

        if (reset) {
            m_RayTracer->reset();
        }
    }

    void App::keyCallback(const WindowKeyEvent &event) {
        if (event.action == KeyAction::PRESS) {
            if (event.key == KeyCode::ESCAPE) m_Window->close();
            if (event.key == KeyCode::R) m_Context->printWGPUReport();
            if (event.key == KeyCode::T && !m_Window->isMinimized())
                m_Window->minimize();
            if (event.key == KeyCode::F) {
                if (m_Window->isFullscreen())
                    m_Window->makeWindowed();
                else
                    m_Window->makeFullscreen();
                auto [width, height] = m_Window->getSize();
                m_Context->resize(width, height);
                m_Depthbuffer->resize2D(width, height, m_Context);
            }
            if (event.key == KeyCode::LEFT_SHIFT)
                m_Camera.setSpeed(m_Camera.getSpeed() * 10.0);
        } else if (event.action == KeyAction::RELEASE) {
            if (event.key == KeyCode::LEFT_SHIFT)
                m_Camera.setSpeed(m_Camera.getSpeed() / 10.0);
        }
    }

    void App::resizeCallback(const WindowResizeEvent &event) {
        ImGui_ImplWGPU_InvalidateDeviceObjects();
        m_Context->resize(event.width, event.height);
        m_Depthbuffer->resize2D(event.width, event.height, m_Context);
        ImGui_ImplWGPU_CreateDeviceObjects();
    }

    void App::mouseMoveCallback(const WindowMouseMoveEvent &event) {
        if (!m_Window->isMouseButtonDown(MouseButton::RIGHT)) return;

        m_Camera.moveMouse(event.x, event.y);
        m_RayTracer->reset();
    }

    void App::mouseScrollCallback(const WindowMouseScrollEvent &event) {
        if (!m_Window->isMouseButtonDown(MouseButton::RIGHT)) return;

        m_Camera.scrollMouse(event.yOffset);
        m_RayTracer->reset();
    }
}  // namespace Yulduz
