#pragma once

#include <Yulduz/Common.hpp>

#include <Yulduz/Render/Graph.hpp>

namespace Yulduz {
    class YULDUZ_API Render {
       public:
        struct Settings {
            SDL_GPUShaderFormat ShaderFormat = SDL_GPU_SHADERFORMAT_SPIRV;
            SDL_GPUPresentMode  PresentMode  = SDL_GPU_PRESENTMODE_MAILBOX;

            SDL_GPUSwapchainComposition SwapchainComposition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;

            glm::u32 MaxFramesInFlight = 3;
        };

       public:
        static Render &GetRef();

        static SDL_GPUDevice *GetDevice();

       public:
        Render()  = default;
        ~Render() = default;

        void initialize(const Settings &settings, SDL_Window *window);
        void release();

        SDL_GPUDevice *getDevice() const;

        bool beginFrame(SDL_GPUCommandBuffer **command_buffer, SDL_GPUTexture **swapchain_texture) const;
        void endFrame(SDL_GPUCommandBuffer *command_buffer) const;

       private:
        YULDUZ_IF_DEBUG(class State {
           public:
            State() = default;
            ~State();
        });

       private:
        SDL_Window    *m_Window = nullptr;
        SDL_GPUDevice *m_Device = nullptr;

       private:
        static Render *s_pInstance;

        YULDUZ_IF_DEBUG(static State s_State);
    };
}  // namespace Yulduz