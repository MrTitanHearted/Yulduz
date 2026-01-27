#include <Yulduz/Editor.hpp>

namespace Yulduz {
    void EditorLayer::onAttach() {
        attachCallback<SDL_Event>([](const SDL_Event &event) -> bool {
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

        DYULDUZ_LOG_INFO("Yulduz::EditorLayer attached");
    }

    void EditorLayer::onDetach() {
        ImGui_ImplSDL3_Shutdown();
        ImGui_ImplSDLGPU3_Shutdown();
        ImGui::DestroyContext();
        DYULDUZ_LOG_INFO("Yulduz::EditorLayer detached");
    }

    void EditorLayer::onUpdate(const glm::f64 delta_time) {
        ImGui_ImplSDLGPU3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("FPS");
        ImGui::Text("FPS: %f", 1.0f / delta_time);
        ImGui::End();

        ImGui::ShowDemoWindow();

        ImGui::EndFrame();
        ImGui::Render();
    }

    void EditorLayer::onRender(RenderGraph &render_graph) {
        RenderPass editor_pass{};
        editor_pass
            .writeTexture(TextureAlias("Yulduz::SwapchainTexture"))
            .setExecuteFn([](const RenderContext &context) {
                SDL_GPUCommandBuffer *command_buffer = context.getCommandBuffer();

                ImGui_ImplSDLGPU3_PrepareDrawData(ImGui::GetDrawData(), command_buffer);

                SDL_GPUTexture *swapchain_texture = context.getTexture(TextureAlias("Yulduz::SwapchainTexture"));

                SDL_GPUColorTargetInfo color_target_info{};
                color_target_info.texture     = swapchain_texture;
                color_target_info.clear_color = SDL_FColor{0.3f, 0.4f, 0.4f, 1.0f};
                color_target_info.load_op     = SDL_GPU_LOADOP_CLEAR;
                color_target_info.store_op    = SDL_GPU_STOREOP_STORE;

                SDL_GPURenderPass *render_pass = SDL_BeginGPURenderPass(command_buffer, &color_target_info, 1, nullptr);
                ImGui_ImplSDLGPU3_RenderDrawData(ImGui::GetDrawData(), command_buffer, render_pass);
                SDL_EndGPURenderPass(render_pass);
            });

        render_graph.addRenderPass(editor_pass);
    }
}  // namespace Yulduz