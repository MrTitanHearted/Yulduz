#pragma once

#include <Yulduz/Engine.hpp>

namespace Yulduz {
    class EditorLayer : public ILayer {
       public:
        EditorLayer()           = default;
        ~EditorLayer() override = default;

        void onAttach() override;
        void onDetach() override;

       private:
        void onUpdate(glm::f64 delta_time) override;
        void onRender(RenderGraph &render_graph) override;
        void onGUI() override;

        void onBeginGUI() override;
        void onEndGUI() override;
        void onRenderGUI(RenderGraph &render_graph) override;

        void initializeImGUI() const;
        void releaseImGUI() const;

        void initializePipeline();
        void releasePipeline() const;

       private:
        bool m_Play = false;

        Camera m_Camera{};

        SDL_GPUGraphicsPipeline *m_Pipeline = nullptr;

        SDL_GPUBuffer *m_VertexBuffer = nullptr;
        SDL_GPUBuffer *m_IndexBuffer  = nullptr;

        SDL_GPUBufferBinding m_VertexBufferBinding{};
        SDL_GPUBufferBinding m_IndexBufferBinding{};
    };
}  // namespace Yulduz