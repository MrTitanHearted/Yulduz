#include <Yulduz/Editor.hpp>

namespace Yulduz {
    void EditorLayer::onAttach() {
        attachCallback<SDL_KeyboardEvent>([this](const SDL_KeyboardEvent &event) {
            if (!event.down) return false;

            if (!m_Play && SDLK_RETURN == event.key) {
                m_Play = true;
                SDL_SetWindowRelativeMouseMode(Window::GetHandle(), true);
            } else if (m_Play && SDLK_ESCAPE == event.key) {
                m_Play = false;
                SDL_SetWindowRelativeMouseMode(Window::GetHandle(), false);
            } else if (!m_Play && SDLK_F == event.key) {
                if (SDL_GetWindowFlags(Window::GetHandle()) & SDL_WINDOW_FULLSCREEN > 0) {
                    SDL_SetWindowFullscreen(Window::GetHandle(), false);
                } else {
                    SDL_SetWindowFullscreen(Window::GetHandle(), true);
                }
            }

            return false;
        });
        attachCallback<SDL_MouseMotionEvent>([this](const SDL_MouseMotionEvent &event) -> bool {
            if (!m_Play) return false;

            m_Camera.processMouseMovement(event.xrel, -event.yrel);

            return false;
        });
        attachCallback<SDL_MouseWheelEvent>([this](const SDL_MouseWheelEvent &event) -> bool {
            if (!m_Play) return false;

            m_Camera.processMouseScroll(static_cast<glm::f32>(event.integer_y));

            return false;
        });

        initializeImGUI();
        initializePipeline();

        DYULDUZ_LOG_INFO("Yulduz::EditorLayer attached");
    }

    void EditorLayer::onDetach() {
        releasePipeline();
        releaseImGUI();

        DYULDUZ_LOG_INFO("Yulduz::EditorLayer detached");
    }

    void EditorLayer::onUpdate(const glm::f64 delta_time) {
        if (m_Play) {
            const bool *keyboard = SDL_GetKeyboardState(nullptr);
            if (keyboard[SDL_SCANCODE_W])
                m_Camera.processCameraMovement(Camera::Direction::eWorldForward, delta_time);
            if (keyboard[SDL_SCANCODE_S])
                m_Camera.processCameraMovement(Camera::Direction::eWorldBackward, delta_time);
            if (keyboard[SDL_SCANCODE_A])
                m_Camera.processCameraMovement(Camera::Direction::eLeft, delta_time);
            if (keyboard[SDL_SCANCODE_D])
                m_Camera.processCameraMovement(Camera::Direction::eRight, delta_time);
            if (keyboard[SDL_SCANCODE_E])
                m_Camera.processCameraMovement(Camera::Direction::eWorldUp, delta_time);
            if (keyboard[SDL_SCANCODE_Q])
                m_Camera.processCameraMovement(Camera::Direction::eWorldDown, delta_time);
        }
    }

    void EditorLayer::onRender(RenderGraph &render_graph) {
        {
            const auto info   = Render::GetViewportInfo({"Yulduz::EditorLayer::MainViewport"});
            const auto width  = static_cast<glm::f32>(info.Width);
            const auto height = static_cast<glm::f32>(info.Height);

            Render::SetViewportCamera(
                {"Yulduz::EditorLayer::MainViewport"},
                {
                    m_Camera.getProjection(width / height),
                    m_Camera.getView(),
                    m_Camera.getPosition(),
                });
        }

        RenderPass pass{};

        pass
            .writeTexture(m_ViewportTextureAlias)
            .setExecuteFn([this](const RenderContext &context) {
                SDL_GPUCommandBuffer *command_buffer = context.getCommandBuffer();

                SDL_GPUColorTargetInfo color_target_info{};
                color_target_info.texture     = context.getTexture(m_ViewportTextureAlias);
                color_target_info.clear_color = SDL_FColor{0.2f, 0.3f, 0.3f, 1.0f};
                color_target_info.load_op     = SDL_GPU_LOADOP_CLEAR;
                color_target_info.store_op    = SDL_GPU_STOREOP_STORE;

                SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(command_buffer, &color_target_info, 1, nullptr);

                glm::u32 width;
                glm::u32 height;
                {
                    const auto info = Render::GetViewportInfo(m_Viewport);

                    width  = info.Width;
                    height = info.Height;
                }

                SDL_GPUViewport viewport{};
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.w = static_cast<glm::f32>(width);
                viewport.h = static_cast<glm::f32>(height);

                viewport.min_depth = 0.0f;
                viewport.max_depth = 1.0f;

                SDL_SetGPUViewport(render_pass, &viewport);

                const glm::mat4x4 projection_view = m_Camera.getProjection(viewport.w / viewport.h) * m_Camera.getView();
                SDL_PushGPUVertexUniformData(command_buffer, 0, &projection_view, sizeof(glm::mat4x4));

                SDL_BindGPUGraphicsPipeline(render_pass, m_Pipeline);
                SDL_BindGPUVertexBuffers(render_pass, 0, &m_VertexBufferBinding, 1);
                SDL_BindGPUIndexBuffer(render_pass, &m_IndexBufferBinding, SDL_GPU_INDEXELEMENTSIZE_32BIT);
                SDL_DrawGPUIndexedPrimitives(render_pass, 3, 1, 0, 0, 0);

                SDL_EndGPURenderPass(render_pass);
            });

        render_graph.addRenderPass(pass);
    }

    void EditorLayer::onGUI() {
        ImGui::ShowMetricsWindow();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Yulduz::EditorLayer::Viewport");
        ImGui::PopStyleVar(2);

        const ImVec2 viewport_size = ImGui::GetContentRegionAvail();

        if (const auto info = Render::GetViewportInfo(m_Viewport);
            static_cast<glm::u32>(viewport_size.x) != info.Width ||
            static_cast<glm::u32>(viewport_size.y) != info.Height) {
            Render::ResizeViewport(m_Viewport, static_cast<glm::u32>(viewport_size.x), static_cast<glm::u32>(viewport_size.y));
        }
        ImGui::Image(Render::GetViewportTexture(m_Viewport), viewport_size);

        ImGui::End();
    }

    void EditorLayer::onBeginGUI() {
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
    }

    void EditorLayer::onEndGUI() {
        ImGui::EndFrame();
        ImGui::Render();
    }

    void EditorLayer::onRenderGUI(RenderGraph &render_graph) {
        RenderPass gui_pass{};
        gui_pass
            .writeTexture(TextureAlias("Yulduz::SwapchainTexture"))
            .setExecuteFn([](const RenderContext &context) {
                SDL_GPUCommandBuffer *command_buffer = context.getCommandBuffer();

                ImGui_ImplSDLGPU3_PrepareDrawData(ImGui::GetDrawData(), command_buffer);

                SDL_GPUTexture *swapchain_texture = context.getTexture(TextureAlias("Yulduz::SwapchainTexture"));

                SDL_GPUColorTargetInfo color_target_info{};
                color_target_info.texture  = swapchain_texture;
                color_target_info.load_op  = SDL_GPU_LOADOP_LOAD;
                color_target_info.store_op = SDL_GPU_STOREOP_STORE;

                SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(command_buffer, &color_target_info, 1, nullptr);
                ImGui_ImplSDLGPU3_RenderDrawData(ImGui::GetDrawData(), command_buffer, render_pass);
                SDL_EndGPURenderPass(render_pass);
            });
        render_graph.addRenderPass(gui_pass);
    }

    void EditorLayer::initializeImGUI() const {
        attachCallback<SDL_Event>([this](const SDL_Event &event) -> bool {
            if (m_Play) return false;

            ImGui_ImplSDL3_ProcessEvent(&event);

            return false;
        });

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        YULDUZ_LOG_ENGINE_INFO(ImGui::GetVersion());

        ImGui::StyleColorsDark();

        ImGui_ImplSDL3_InitForSDLGPU(Window::GetHandle());

        ImGui_ImplSDLGPU3_InitInfo init_info{};
        init_info.Device               = Render::GetDevice();
        init_info.MSAASamples          = SDL_GPU_SAMPLECOUNT_1;
        init_info.ColorTargetFormat    = SDL_GetGPUSwapchainTextureFormat(Render::GetDevice(), Window::GetHandle());
        init_info.PresentMode          = SDL_GPU_PRESENTMODE_MAILBOX;
        init_info.SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
        ImGui_ImplSDLGPU3_Init(&init_info);
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    }

    void EditorLayer::releaseImGUI() const {
        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui::DestroyContext();
        detachCallback<SDL_Event>();
    }

    void EditorLayer::initializePipeline() {
        m_Viewport = Render::AddViewport(
            "Yulduz::EditorLayer::MainViewport",
            {
                .Usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,

                .Width  = 1,
                .Height = 1,
            });
        m_ViewportTextureAlias = TextureAlias("Yulduz::EditorLayer::MainViewport");

        struct Vertex {
            glm::vec3 Position;
            glm::vec3 Color;
        };

        SDL_GPUDevice *device = Render::GetDevice();

        size_t shader_source_code_size;

        const auto shader_source_code = SDL_LoadFile("Assets/Shaders/Triangle.spv", &shader_source_code_size);

        SDL_GPUShaderCreateInfo vs_create_info{};
        vs_create_info.code       = static_cast<const glm::uint8 *>(shader_source_code);
        vs_create_info.code_size  = shader_source_code_size;
        vs_create_info.entrypoint = "vs_main";
        vs_create_info.format     = SDL_GPU_SHADERFORMAT_SPIRV;
        vs_create_info.stage      = SDL_GPU_SHADERSTAGE_VERTEX;

        vs_create_info.num_uniform_buffers = 1;

        SDL_GPUShaderCreateInfo fs_create_info{};
        fs_create_info.code       = static_cast<const glm::uint8 *>(shader_source_code);
        fs_create_info.code_size  = shader_source_code_size;
        fs_create_info.entrypoint = "fs_main";
        fs_create_info.format     = SDL_GPU_SHADERFORMAT_SPIRV;
        fs_create_info.stage      = SDL_GPU_SHADERSTAGE_FRAGMENT;

        SDL_GPUShader *vs = SDL_CreateGPUShader(device, &vs_create_info);
        SDL_GPUShader *fs = SDL_CreateGPUShader(device, &fs_create_info);

        const auto viewport_info = Render::GetViewportInfo(Render::Viewport{"Yulduz::EditorLayer::MainViewport"});

        SDL_GPUColorTargetDescription color_target_description{};
        color_target_description.format = viewport_info.Format;
        color_target_description
            .blend_state
            .enable_blend = false;

        SDL_GPUVertexBufferDescription vertex_buffer_description{};
        vertex_buffer_description.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
        vertex_buffer_description.pitch      = sizeof(Vertex);
        vertex_buffer_description.slot       = 0;

        eastl::vector vertex_attributes{
            SDL_GPUVertexAttribute{0, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Vertex, Position)},
            SDL_GPUVertexAttribute{1, 0, SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3, offsetof(Vertex, Color)},
        };

        SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info{};
        pipeline_create_info.vertex_shader   = vs;
        pipeline_create_info.fragment_shader = fs;

        pipeline_create_info.vertex_input_state.vertex_buffer_descriptions = &vertex_buffer_description;
        pipeline_create_info.vertex_input_state.num_vertex_buffers         = 1;
        pipeline_create_info.vertex_input_state.vertex_attributes          = vertex_attributes.data();
        pipeline_create_info.vertex_input_state.num_vertex_attributes      = vertex_attributes.size();

        pipeline_create_info.target_info.color_target_descriptions = &color_target_description;
        pipeline_create_info.target_info.num_color_targets         = 1;
        pipeline_create_info.target_info.has_depth_stencil_target  = false;

        pipeline_create_info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
        pipeline_create_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;

        pipeline_create_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

        pipeline_create_info.multisample_state.sample_count = SDL_GPU_SAMPLECOUNT_1;

        pipeline_create_info.depth_stencil_state.enable_depth_write  = false;
        pipeline_create_info.depth_stencil_state.enable_depth_test   = false;
        pipeline_create_info.depth_stencil_state.enable_stencil_test = false;

        m_Pipeline = SDL_CreateGPUGraphicsPipeline(device, &pipeline_create_info);

        eastl::vector<Vertex> vertices{
            Vertex{{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
            Vertex{{0.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
            Vertex{{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        };

        eastl::vector<glm::u32> indices{0, 1, 2};

        SDL_GPUBufferCreateInfo vertex_buffer_create_info{};
        vertex_buffer_create_info.size  = sizeof(Vertex) * vertices.size();
        vertex_buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;

        SDL_GPUBufferCreateInfo index_buffer_create_info{};
        index_buffer_create_info.size  = sizeof(glm::u32) * indices.size();
        index_buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_INDEX;

        m_VertexBuffer = SDL_CreateGPUBuffer(device, &vertex_buffer_create_info);
        m_IndexBuffer  = SDL_CreateGPUBuffer(device, &index_buffer_create_info);

        m_VertexBufferBinding.buffer = m_VertexBuffer;
        m_VertexBufferBinding.offset = 0;

        m_IndexBufferBinding.buffer = m_IndexBuffer;
        m_IndexBufferBinding.offset = 0;

        SDL_GPUTransferBufferCreateInfo transfer_buffer_create_info{};
        transfer_buffer_create_info.size  = vertex_buffer_create_info.size + index_buffer_create_info.size;
        transfer_buffer_create_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;

        SDL_GPUTransferBuffer *transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_buffer_create_info);

        {
            void *mapped_dst = SDL_MapGPUTransferBuffer(device, transfer_buffer, false);

            size_t offset = 0;
            SDL_memcpy(static_cast<char *>(mapped_dst) + offset, vertices.data(), vertex_buffer_create_info.size);
            offset += vertex_buffer_create_info.size;
            SDL_memcpy(static_cast<char *>(mapped_dst) + offset, indices.data(), index_buffer_create_info.size);
            offset += index_buffer_create_info.size;

            SDL_UnmapGPUTransferBuffer(device, transfer_buffer);
        }

        {
            SDL_GPUCommandBuffer *command_buffer = SDL_AcquireGPUCommandBuffer(device);

            SDL_GPUCopyPass *copy_pass = SDL_BeginGPUCopyPass(command_buffer);

            SDL_GPUTransferBufferLocation v_source{};
            v_source.transfer_buffer = transfer_buffer;
            v_source.offset          = 0;

            SDL_GPUBufferRegion v_destination{};
            v_destination.buffer = m_VertexBuffer;
            v_destination.size   = vertex_buffer_create_info.size;
            v_destination.offset = 0;

            SDL_GPUTransferBufferLocation i_source{};
            i_source.transfer_buffer = transfer_buffer;
            i_source.offset          = vertex_buffer_create_info.size;

            SDL_GPUBufferRegion i_destination{};
            i_destination.buffer = m_IndexBuffer;
            i_destination.size   = index_buffer_create_info.size;
            i_destination.offset = 0;

            SDL_UploadToGPUBuffer(copy_pass, &v_source, &v_destination, false);
            SDL_UploadToGPUBuffer(copy_pass, &i_source, &i_destination, false);

            SDL_EndGPUCopyPass(copy_pass);

            SDL_GPUFence *fence = SDL_SubmitGPUCommandBufferAndAcquireFence(command_buffer);
            SDL_WaitForGPUFences(device, true, &fence, 1);
            SDL_ReleaseGPUFence(device, fence);
        }

        SDL_ReleaseGPUTransferBuffer(device, transfer_buffer);

        SDL_ReleaseGPUShader(device, fs);
        SDL_ReleaseGPUShader(device, vs);

        SDL_free(shader_source_code);
    }

    void EditorLayer::releasePipeline() const {
        SDL_GPUDevice *device = Render::GetDevice();

        SDL_ReleaseGPUBuffer(device, m_IndexBuffer);
        SDL_ReleaseGPUBuffer(device, m_VertexBuffer);

        SDL_ReleaseGPUGraphicsPipeline(device, m_Pipeline);

        Render::RemoveViewport(m_Viewport);
    }
}  // namespace Yulduz