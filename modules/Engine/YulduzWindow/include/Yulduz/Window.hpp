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
            EventDispatcher *eventDispatcher = nullptr;
        };

       public:
        static std::shared_ptr<Window> New(Settings settings);
        inline static std::shared_ptr<Window> Default() { return New(Settings{}); }
        static void PollEvents();

       public:
        Window(GLFWwindow *window);
        ~Window();

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

        bool isKeyModifierPressed(KeyMod mod) const;

        bool isKeyDown(KeyCode key) const;
        bool isKeyUp(KeyCode key) const;

        bool isMouseButtonDown(MouseButton button) const;
        bool isMouseButtonUp(MouseButton button) const;

        WGPUSurface getWGPUSurface(WGPUInstance instance) const;

       private:
        GLFWwindow *m_Window;
        EventDispatcher *m_Dispatcher;
        bool m_QuitOnEscape;

        KeyMod m_KeyMods;

        bool m_Keys[GLFW_KEY_LAST+1]{false};
        bool m_MouseButtons[GLFW_KEY_LAST+1]{false};

       private:
        void closeCallback(const WindowCloseEvent &event);
        void resizeCallback(const WindowResizeEvent &event);
        void moveCallback(const WindowMoveEvent &event);
        void contentScaleCallback(const WindowContentScaleEvent &event);
        void mouseMoveCallback(const WindowMouseMoveEvent &event);
        void minimizeCallback(const WindowMinimizeEvent &event);
        void maximizeCallback(const WindowMaximizeEvent &event);
        void gainFocusCallback(const WindowGainFocusEvent &event);
        void loseFocusCallback(const WindowLoseFocusEvent &event);
        void keyCallback(const WindowKeyEvent &event);
        void charCallback(const WindowCharEvent &event);
        void mouseButtonCallback(const WindowMouseButtonEvent &event);
        void mouseScrollCallback(const WindowMouseScrollEvent &event);

        static void GlfwWindowCloseCallback(GLFWwindow *window);
        static void GlfwWindowSizeCallback(GLFWwindow *window, int width, int height);
        static void GlfwWindowContentScaleCallback(GLFWwindow *window, float xScale, float yScale);
        static void GlfwWindowPositionCallback(GLFWwindow *window, int x, int y);
        static void GlfwWindowMinimizeCallback(GLFWwindow *window, int isMinimized);
        static void GlfwWindowMaximizeCallback(GLFWwindow *window, int isMaximized);
        static void GlfwWindowFocusCallback(GLFWwindow *window, int focused);
        static void GlfwWindowKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void GlfwWindowCharCallback(GLFWwindow *window, unsigned int codepoint);
        static void GlfwWindowMousePositionCallback(GLFWwindow *window, double x, double y);
        static void GlfwWindowMouseEnterCallback(GLFWwindow *window, int entered);
        static void GlfwWindowMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        static void GlfwWindowMouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
    };
}  // namespace Yulduz
