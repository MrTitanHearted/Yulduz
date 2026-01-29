#pragma once

#include <Yulduz/Common.hpp>

#include <Yulduz/Render/Graph.hpp>

namespace Yulduz {
    class YULDUZ_API Render {
       public:
        struct Viewport {
            size_t Hash = 0;

            Viewport()  = default;
            ~Viewport() = default;

            Viewport(const size_t hash)
                : Hash{hash} {}

            Viewport(const eastl::string_view name)
                : Hash{eastl::hash<eastl::string_view>{}(name)} {}

            bool operator==(const Viewport &) const = default;
        };

        struct ViewportCamera {
            glm::mat4x4 Projection;
            glm::mat4x4 View;

            glm::vec3 Position;

            glm::f32 _yulduz_padding;
        };

        struct ViewportInfo {
            SDL_GPUTextureFormat     Format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
            SDL_GPUTextureType       Type   = SDL_GPU_TEXTURETYPE_2D;
            SDL_GPUTextureUsageFlags Usage  = 0;

            glm::u32 Width  = 0;
            glm::u32 Height = 0;

            glm::u32 LayerCount = 1;
            glm::u32 LevelCount = 1;

            SDL_GPUSampleCount SampleCount = SDL_GPU_SAMPLECOUNT_1;
        };

        struct ViewportImpl {
            eastl::string Name;

            SDL_GPUTexture *Target = nullptr;

            ViewportInfo   Info;
            ViewportCamera Camera;
        };

        struct Settings {
            SDL_GPUShaderFormat ShaderFormat = SDL_GPU_SHADERFORMAT_SPIRV;
            SDL_GPUPresentMode  PresentMode  = SDL_GPU_PRESENTMODE_MAILBOX;

            SDL_GPUSwapchainComposition SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;

            glm::u32 MaxFramesInFlight = 3;
        };

        static Viewport k_InvalidViewport;

       public:
        static Render &GetRef();

        static SDL_GPUDevice *GetDevice();

        static Viewport AddViewport(eastl::string_view name, ViewportInfo info);
        static void     RemoveViewport(Viewport handle);

        static void ResizeViewport(Viewport handle, glm::u32 width, glm::u32 height);
        static void SetViewportCamera(Viewport handle, const ViewportCamera &camera);

        static SDL_GPUTexture *GetViewportTexture(Viewport handle);
        static ViewportInfo    GetViewportInfo(Viewport handle);
        static ViewportCamera  GetViewportCamera(Viewport handle);

       public:
        Render()  = default;
        ~Render() = default;

        void initialize(const Settings &settings, SDL_Window *window, RenderGraph &render_graph);
        void release();

        SDL_GPUDevice *getDevice() const;

        Viewport addViewport(eastl::string_view name, ViewportInfo info);
        void     removeViewport(Viewport handle);

        void resizeViewport(Viewport handle, glm::u32 width, glm::u32 height);
        void setViewportCamera(Viewport handle, const ViewportCamera &camera);

        SDL_GPUTexture *getViewportTexture(Viewport handle) const;
        ViewportInfo    getViewportInfo(Viewport handle) const;
        ViewportCamera  getViewportCamera(Viewport handle) const;

       private:
        YULDUZ_IF_DEBUG(class State {
           public:
            State() = default;
            ~State();
        });

        struct ViewportHasher {
            size_t operator()(const Viewport &handle) const noexcept {
                return handle.Hash;
            }
        };

       private:
        bool beginFrame(SDL_GPUCommandBuffer **command_buffer, SDL_GPUTexture **swapchain_texture) const;
        void endFrame(SDL_GPUCommandBuffer *command_buffer) const;

       private:
        SDL_Window    *m_Window = nullptr;
        SDL_GPUDevice *m_Device = nullptr;

        RenderGraph *m_pRenderGraph = nullptr;

        eastl::hash_map<Viewport, ViewportImpl, ViewportHasher> m_Viewports{};

       private:
        static Render *s_pInstance;

        YULDUZ_IF_DEBUG(static State s_State);

       private:
        friend class Engine;
    };
}  // namespace Yulduz

template <>
struct YULDUZ_API eastl::hash<Yulduz::Render::Viewport> {
    size_t operator()(const Yulduz::Render::Viewport &handle) const {
        return handle.Hash;
    }
};