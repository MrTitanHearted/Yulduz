#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzWindow/Enums.hpp>
#include <YulduzWindow/Events.hpp>

namespace Yulduz {
    class Window {
       public:
        struct Settings {
            std::string name = "YulduzWindow";
            uint32_t width = 1200;
            uint32_t height = 1000;
            bool resizable = true;
            bool quitOnEscape = true;
            bool fullscreen = false;
            bool fullscreenSize = false;
            uint32_t doublePressTime = 200;
        };

       public:
        static std::shared_ptr<Window> New(Settings settings);
        inline static std::shared_ptr<Window> Default() { return New(Settings{}); }

       public:
        Window(GLFWwindow *window);
        ~Window();

        void pollEvents();
        void registerCallbacks(EventDispatcher &dispatcher);
        void setSize(uint32_t width, uint32_t height);
        void setCursorMode(CursorMode mode);
        void minimize();
        void maximize();
        void restore();
        void close();
        void setQuitOnEscape(bool quitOnEscape);

        std::string getTitle() const;
        std::array<uint32_t, 2> getSize() const;
        uint32_t getWidth() const;
        uint32_t getHeight() const;
        std::array<double, 2> getMousePosition() const;
        double getMouseX() const;
        double getMouseY() const;
        CursorMode getCursorMode() const;

        bool isClosed() const;
        bool isRunning() const;
        bool isMinimized() const;
        bool isMaximized() const;
        bool isFocused() const;

        bool isKeyPressed(KeyCode key, KeyMod mods = KeyMod::NONE) const;
        bool isKeyDoublePressed(KeyCode key, KeyMod mods = KeyMod::NONE) const;
        bool isKeyReleased(KeyCode Key, KeyMod mods = KeyMod::NONE) const;
        bool isKeyDown(KeyCode Key, KeyMod mods = KeyMod::NONE) const;
        bool isKeyUp(KeyCode Key, KeyMod mods = KeyMod::NONE) const;

        bool isMouseButtonPressed(KeyCode key, KeyMod mods = KeyMod::NONE) const;
        bool isMouseButtonDoublePressed(KeyCode key, KeyMod mods = KeyMod::NONE) const;
        bool isMouseButtonReleased(KeyCode Key, KeyMod mods = KeyMod::NONE) const;
        bool isMouseButtonDown(KeyCode Key, KeyMod mods = KeyMod::NONE) const;
        bool isMouseButtonUp(KeyCode Key, KeyMod mods = KeyMod::NONE) const;

        WGPUSurface getWGPUSurface(WGPUInstance instance) const;

       private:
        GLFWwindow *m_Window;
        KeyMod m_KeyMods;

        bool m_Keys[GLFW_KEY_LAST + 1]{false};
        bool m_KeyDoubleClicks[GLFW_KEY_LAST + 1]{false};
        double m_KeyLastClickTime[GLFW_KEY_LAST + 1]{0.0};

        bool m_MouseButtons[GLFW_MOUSE_BUTTON_LAST + 1]{false};
        bool m_MouseButtonDoubleClicks[GLFW_MOUSE_BUTTON_LAST + 1]{false};
        double m_MousebuttonClickTime[GLFW_MOUSE_BUTTON_LAST + 1]{false};
    };
}  // namespace Yulduz
