#include <Yulduz/Yulduz.hpp>

namespace Yulduz {
    void App::Run() {
        App app;
        app.run();
    }

    App::App()
        : m_Camera{45.0f, 0.1f, 100.0f},
          m_DeltaTime{0},
          m_EventDispatchTime{0},
          m_CameraTime{0},
          m_ImGuiTime{0},
          m_RenderTime{0},
          m_RayTracingTime{0} {
        m_Window = Window::New(Window::Settings{
            .Title = "Yulduz Cherno Raytracing",
            .Width = 1200,
            .Height = 800,
            .EventDispatcher = m_EventDispatcher,
        });
        auto [width, height] = m_Window->getSize();
        m_Context = RenderContextBuilder::New().build(m_Window);
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
        m_Renderer.setRenderContext(m_Context);

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
            .Roughness = 0.1f,
            .EmissionPower = 2.0f,
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
    }

    App::~App() {
        ShutdownImGui();
    }

    void App::run() {
        while (m_Window->isRunning()) {
            static Milliseconds::Timer timer;
            static Milliseconds::Timer eventTimer;
            static Milliseconds::Timer cameraTimer;
            static Milliseconds::Timer imguiTimer;
            static Milliseconds::Timer renderTimer;

            timer.start();

            eventTimer.start();
            Window::PollEvents();
            m_EventDispatcher.dispatch();
            eventTimer.stop();
            m_EventDispatchTime = eventTimer.getElapsed();

            cameraTimer.start();
            if (m_Camera.OnUpdate(m_DeltaTime, m_Window)) m_Renderer.resetFrameIndex();
            cameraTimer.stop();
            m_CameraTime = cameraTimer.getElapsed();

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

        static Milliseconds::Timer settingsTimer;
        static Milliseconds::Timer sceneTimer;
        static Milliseconds::Timer viewportTimer;
        static double settingsTime;
        static double sceneTime;
        static double viewportTime;

        settingsTimer.start();
        ImGui::Begin("Settings");
        ImGui::Text("Delta Time: %.3fms", m_DeltaTime);
        ImGui::Text("Event Dispatch Time: %.3fms", m_EventDispatchTime);
        ImGui::Text("Camera Time: %.3fms", m_CameraTime);
        ImGui::Text("ImGui Time: %.3fms", m_ImGuiTime);
        ImGui::Text("\tSettings Time: %.3fms", settingsTime);
        ImGui::Text("\tScene Time: %.3fms", sceneTime);
        ImGui::Text("\tViewport Time: %.3fms", viewportTime);
        ImGui::Text("\tRayTracing Time: %.3fms", m_RayTracingTime);
        ImGui::Text("Render Time: %.3fms", m_RenderTime);
        if (ImGui::Button("Render"))
            updateFramedata();
        ImGui::Checkbox("Accumulate", &m_Renderer.getSettings().Accumulate);
        ImGui::Checkbox("Slow Random", &m_Renderer.getSettings().SlowRandom);
        if (ImGui::Button("Reset"))
            m_Renderer.resetFrameIndex();
        ImGui::End();
        settingsTimer.stop();
        settingsTime = settingsTimer.getElapsed();

        sceneTimer.start();
        ImGui::Begin("Scene");
        for (std::size_t i = 0; i < m_Scene.Spheres.size(); i++) {
            ImGui::PushID(i);

            Sphere &sphere = m_Scene.Spheres[i];
            ImGui::DragFloat3("Position", glm::value_ptr(sphere.Position), 0.1f);
            ImGui::DragFloat("Radius", &sphere.Radius, 0.1f);
            ImGui::DragInt("Material", &sphere.MaterialIndex, 1.0f, 0, static_cast<std::int32_t>(m_Scene.Materials.size() - 1));

            ImGui::Separator();

            ImGui::PopID();
        }

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
        ImGui::End();
        sceneTimer.stop();
        sceneTime = sceneTimer.getElapsed();

        viewportTimer.start();
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{0, 0});
        ImGui::Begin("Viewport");

        m_Viewport = ImGui::GetContentRegionAvail();

        std::shared_ptr<Framebuffer> image = m_Renderer.getFinalImage();
        if (image)
            ImGui::Image(image->getView(), ImVec2{static_cast<float>(image->getWidth()), static_cast<float>(image->getHeight())}, ImVec2{0, 1}, ImVec2{1, 0});

        ImGui::End();
        ImGui::PopStyleVar();
        viewportTimer.stop();
        viewportTime = viewportTimer.getElapsed();

        ImGui::PopFont();
    }

    void App::updateFramedata() {
        if (m_Viewport.x == 0 || m_Viewport.y == 0) return;

        static Milliseconds::Timer rayTracingTimer;
        rayTracingTimer.start();

        m_Renderer.resize(m_Viewport.x, m_Viewport.y);
        m_Camera.OnResize(m_Viewport.x, m_Viewport.y);
        m_Renderer.render(m_Scene, m_Camera);

        rayTracingTimer.stop();

        m_RayTracingTime = rayTracingTimer.getElapsed();
    }

    void App::keyCallback(const WindowKeyEvent &event) {
        if (event.action != KeyAction::PRESS) return;
        if (event.key == KeyCode::ESCAPE) m_Window->close();
        if (event.key == KeyCode::R) m_Context->printWGPUReport();
    }

    void App::resizeCallback(const WindowResizeEvent &event) {
        ImGui_ImplWGPU_InvalidateDeviceObjects();
        m_Depthbuffer = TextureBuilder::New()
                            .setLabel("Yulduz Context Depth Buffer")
                            .setFormat(TextureFormat::Depth32Float)
                            .emptyFramebuffer(event.width, event.height, m_Context);
        ImGui_ImplWGPU_CreateDeviceObjects();
    }
}  // namespace Yulduz
