#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzGraphics/Context/GraphicsContext.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_wgpu.h>

namespace Yulduz {
    class RenderPass;

    struct ImGuiSettings {
        TextureFormat DepthFormat = TextureFormat::Depth32Float;
        std::reference_wrapper<const GraphicsContext> Context;
    };

    bool IsImGuiInitialized();
    void InitImGui(const ImGuiSettings &settings);
    void ImGuiFrame(void (*callback)(void));
    void RenderImGuiPass(const RenderPass &renderPass);
    void ShutdownImGui();
    
    template <typename T>
    static void ImGuiFrame(void (T::*callback)(void), T *self) {
        assert(IsImGuiInitialized() && "ImGui not initialized");

        ImGui_ImplWGPU_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        (self->*callback)();

        ImGui::EndFrame();
        ImGui::Render();
    }
}  // namespace Yulduz