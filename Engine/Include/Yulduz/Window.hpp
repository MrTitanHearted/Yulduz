#pragma once

#include <Yulduz/Assets.hpp>
#include <Yulduz/Common.hpp>

namespace Yulduz {
    class YULDUZ_API Window {
       public:
        struct Settings {
            eastl::string_view Title = "Yulduz::Window";

            glm::u32 Width  = 1200;
            glm::u32 Height = 800;

            bool Resizable  = true;
            bool Fullscreen = false;

            eastl::string_view Icon = "";
        };

       public:
        static Window &GetRef();

        static SDL_Window *GetHandle();

       public:
        Window()  = default;
        ~Window() = default;

        void initialize(const Settings &settings);
        void release();

        SDL_Window *getHandle() const;

       private:
        YULDUZ_IF_DEBUG(class State {
           public:
            ~State();
        });

       private:
        SDL_Window *m_Window = nullptr;

       private:
        static Window *s_pInstance;

        YULDUZ_IF_DEBUG(static State s_State);
    };
}  // namespace Yulduz