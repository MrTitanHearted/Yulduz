#include <YulduzRender/ImGui.hpp>

namespace Yulduz {
    static bool ImGuiInitialized = false;

    bool IsImGuiInitialized() {
        return ImGuiInitialized;
    }

    void InitImGui(const std::shared_ptr<RenderContext> &context) {
        if (ImGuiInitialized) return;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplWGPU_InitInfo info{};
        info.Device = context->getDevice();
        info.RenderTargetFormat = context->getConfig().format;
        info.DepthStencilFormat = WGPUTextureFormat_Depth32Float;

        ImGui_ImplGlfw_InitForOther(context->getWindow()->get(), true);
        ImGui_ImplWGPU_Init(&info);

        ImGuiInitialized = true;
    }

    void ImGuiFrame(std::function<void()> frameCallback) {
        if (!ImGuiInitialized) return;

        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        frameCallback();

        ImGui::EndFrame();
        ImGui::Render();
    }

    void RenderImGui(const std::shared_ptr<RenderPass> &renderPass) {
        if (!ImGuiInitialized) return;

        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass->get());

        ImGuiIO &io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void ShutdownImGui() {
        if (!ImGuiInitialized) return;

        ImGui_ImplWGPU_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        ImGuiInitialized = false;
    }
}  // namespace Yulduz
