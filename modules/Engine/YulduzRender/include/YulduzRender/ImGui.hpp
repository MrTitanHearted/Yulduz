#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzRender/Context.hpp>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_wgpu.h>

namespace Yulduz {
    bool IsImGuiInitialized();
    void InitImGui(const std::shared_ptr<RenderContext> &context);
    void ImGuiFrame(std::function<void()> frameCallback);
    void RenderImGui(const std::shared_ptr<RenderPass> &renderPass);
    void ShutdownImGui();
}  // namespace Yulduz
