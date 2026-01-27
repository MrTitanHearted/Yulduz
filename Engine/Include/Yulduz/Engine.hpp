#pragma once

#include <Yulduz/Assets.hpp>
#include <Yulduz/Common.hpp>
#include <Yulduz/Render.hpp>
#include <Yulduz/Window.hpp>

#include <Yulduz/Engine/Layer.hpp>

namespace Yulduz {
    class YULDUZ_API Engine {
       public:
        struct Settings {
            Window::Settings WindowSettings;
            Render::Settings RenderSettings;

            TextureAlias SwapchainTextureAlias = TextureAlias("Yulduz::SwapchainTexture");
        };

       public:
        static Engine &GetRef();

        static bool IsRunning();

        static RenderGraph &GetRenderGraph();

        template <typename TLayer, typename... Args>
            requires(std::is_base_of_v<ILayer, TLayer>)
        static TLayer *PushLayer(Args &&...args);

        template <typename TLayer>
            requires(std::is_base_of_v<ILayer, TLayer>)
        static TLayer *GetLayer();

       public:
        Engine()  = default;
        ~Engine() = default;

        void initialize(const Settings &settings);
        void release();

        void run();

        bool isRunning() const;

        RenderGraph &getRenderGraph();

        template <typename TLayer, typename... Args>
            requires(std::is_base_of_v<ILayer, TLayer>)
        TLayer *pushLayer(Args &&...args);

        template <typename TLayer>
            requires(std::is_base_of_v<ILayer, TLayer>)
        TLayer *getLayer();

       private:
        YULDUZ_IF_DEBUG(class State {
           public:
            ~State();
        });

       private:
        void pollEvents();

        bool onQuit(const SDL_QuitEvent &event);

       private:
        eastl::atomic<bool> m_IsRunning{false};

        glm::f64 m_DeltaTime{0.0};

        EventDispatcher m_Dispatcher{};

        Window m_Window{};
        Render m_Render{};

        RenderGraph m_RenderGraph{};

        eastl::vector<std::unique_ptr<ILayer>> m_LayerStack{};

       private:
        static Engine *s_pInstance;

        YULDUZ_IF_DEBUG(static State s_State);

       private:
        friend class ILayer;
    };
}  // namespace Yulduz

#include <Yulduz/Engine/LayerImpl.hpp>
#include <Yulduz/Engine/EngineImpl.hpp>
