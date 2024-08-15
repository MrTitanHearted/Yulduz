#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzWindow/Enums.hpp>
#include <YulduzWindow/Events.hpp>

namespace Yulduz {
    class Window {
       public:
        struct Settings {
            std::string Title = "Yulduz Engine";
            std::uint32_t Width = 1200;
            std::uint32_t Height = 1000;
            bool Resizable = true;
            bool Fullscreen = false;
            bool FullscreenMonitorSize = true;
        };

       public:
        static void SetEventDispatcher(EventDispatcher &dispatcher);
        static void PollEvents();

       public:
        Window(const Settings &settings);
        ~Window();

        Window(Window &&) = delete;
        Window &operator=(Window &&) = delete;

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        void addResizeCallback(void (*callback)(const WindowResizeEvent &event));
        void setTitle(const std::string &title);
        void setSize(std::uint32_t width, std::uint32_t height);
        void setWidth(std::uint32_t width);
        void setHeight(std::uint32_t height);
        void setPosition(std::uint32_t x, std::uint32_t y);
        void setX(std::uint32_t x);
        void setY(std::uint32_t y);
        void setMousePosition(double x, double y);
        void setMouseX(double x);
        void setMouseY(double y);
        void setCursorMode(CursorMode mode);
        void setResizable(bool resizable);
        void setRunning(bool running);
        void makeFullscreen(bool monitorSize = true);
        void makeWindowed();
        void maximize();
        void minimize();
        void restore();
        void close();

        GLFWwindow *get() const;
        std::string getTitle() const;
        std::array<std::uint32_t, 2> getSize() const;
        std::uint32_t getWidth() const;
        std::uint32_t getHeight() const;
        std::array<std::uint32_t, 2> getPosition() const;
        std::uint32_t getX() const;
        std::uint32_t getY() const;
        std::array<double, 2> getMousePosition() const;
        double getMouseX() const;
        double getMouseY() const;
        CursorMode getCursorMode() const;

        bool isClosed() const;
        bool isRunning() const;
        bool isMaximized() const;
        bool isMinimized() const;
        bool isFocused() const;
        bool isFullscreen() const;
        bool isResizable() const;

        bool isKeyModPressed(KeyMod mod) const;
        bool isKeyDown(KeyCode key) const;
        bool isKeyUp(KeyCode key) const;
        bool isMouseButtonDown(MouseButton button) const;
        bool isMouseButtonUp(MouseButton button) const;

        WGPUSurface getWGPUSurface(WGPUInstance instance) const;

        template <typename T>
        void addResizeCallback(void (T::*callback)(const WindowResizeEvent &event), T *self) {
            m_ResizeCallbacks.emplace_back([callback, self](const WindowResizeEvent &event) { (self->*callback)(event); });
        }

       private:
        class GlfwState {
           public:
            GlfwState();
            ~GlfwState();
        };

       private:
        GLFWwindow *m_Window;
        std::uint32_t m_PrevWidth;
        std::uint32_t m_PrevHeight;
        std::uint32_t m_PrevX;
        std::uint32_t m_PrevY;

        KeyMod m_KeyMods;
        bool m_Keys[GLFW_KEY_LAST + 1];
        bool m_MouseButtons[GLFW_MOUSE_BUTTON_LAST + 1];

        static GlfwState g_GlfwState;
        static EventDispatcher *g_EventDispatcher;

        std::vector<std::function<void(const WindowResizeEvent &)>> m_ResizeCallbacks;

       private:
#if defined(YULDUZ_BUILD_TYPE_DEBUG)
        void closeCallback(const WindowCloseEvent &event);
        void resizeCallback(const WindowResizeEvent &event);
        void moveCallback(const WindowMoveEvent &event);
        void contentScaleCallback(const WindowContentScaleEvent &event);
        void mouseMoveCallback(const WindowMouseMoveEvent &event);
        void maximizeCallback(const WindowMaximizeEvent &event);
        void minimizeCallback(const WindowMinimizeEvent &event);
        void gainFocusCallback(const WindowGainFocusEvent &event);
        void loseFocusCallback(const WindowLoseFocusEvent &event);
        void keyCallback(const WindowKeyEvent &event);
        void charCallback(const WindowCharEvent &event);
        void mouseButtonCallback(const WindowMouseButtonEvent &event);
        void mouseScrollCallback(const WindowMouseScrollEvent &event);
#endif

        static void GlfwWindowCloseCallback(GLFWwindow *window);
        static void GlfwWindowSizeCallback(GLFWwindow *window, std::int32_t width, std::int32_t height);
        static void GlfwWindowContentScaleCallback(GLFWwindow *window, float xScale, float yScale);
        static void GlfwWindowPositionCallback(GLFWwindow *window, std::int32_t x, std::int32_t y);
        static void GlfwWindowMaximizeCallback(GLFWwindow *window, std::int32_t isMaximized);
        static void GlfwWindowMinimizeCallback(GLFWwindow *window, std::int32_t isMinimized);
        static void GlfwWindowFocusCallback(GLFWwindow *window, std::int32_t focused);
        static void GlfwWindowKeyCallback(GLFWwindow *window, std::int32_t key, std::int32_t scancode, std::int32_t action, std::int32_t mods);
        static void GlfwWindowCharCallback(GLFWwindow *window, std::uint32_t codepoint);
        static void GlfwWindowMousePositionCallback(GLFWwindow *window, double x, double y);
        static void GlfwWindowMouseEnterCallback(GLFWwindow *window, std::int32_t entered);
        static void GlfwWindowMouseButtonCallback(GLFWwindow *window, std::int32_t button, std::int32_t action, std::int32_t mods);
        static void GlfwWindowMouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
    };
}  // namespace Yulduz