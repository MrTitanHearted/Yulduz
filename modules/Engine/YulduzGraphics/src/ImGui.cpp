#include <YulduzGraphics/ImGui.hpp>
#include <YulduzGraphics/Context/RenderPass.hpp>
#include <Yulduz/Window.hpp>

namespace Yulduz {
    static bool ImGuiInitialized = false;

    bool IsImGuiInitialized() {
        return ImGuiInitialized;
    }

    void InitImGui(const ImGuiSettings &settings) {
        if (ImGuiInitialized) return;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplWGPU_InitInfo initInfo{};
        initInfo.Instance = settings.Context.get().getInstance();
        initInfo.Device = settings.Context.get().getDevice();
        initInfo.RenderTargetFormat = settings.Context.get().getConfig().format;
        initInfo.DepthStencilFormat = static_cast<WGPUTextureFormat>(settings.DepthFormat);
        initInfo.ViewportPresentMode = static_cast<WGPUPresentMode>(settings.Context.get().getPresentMode());

        const Window &window = settings.Context.get().getWindow();
        ImGui_ImplGlfw_InitForOther(window.get(), true);
        ImGui_ImplWGPU_Init(&initInfo);

        ImGuiInitialized = true;
    }

    void ImGuiFrame(void (*callback)(void)) {
        assert(ImGuiInitialized && "ImGui not initialized");

        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        (*callback)();

        ImGui::EndFrame();
        ImGui::Render();
    }

    void RenderImGuiPass(const RenderPass &renderPass) {
        assert(ImGuiInitialized && "ImGui not initialized");

        ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPass.get());
    }

    void ShutdownImGui() {
        assert(ImGuiInitialized && "ImGui not initialized");

        ImGui_ImplWGPU_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        ImGuiInitialized = false;
    }
}  // namespace Yulduz
