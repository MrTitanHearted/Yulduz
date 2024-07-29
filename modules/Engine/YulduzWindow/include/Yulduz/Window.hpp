#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzWindow/Enums.hpp>
#include <YulduzWindow/Events.hpp>

namespace Yulduz {
    class Window {
       public:
        struct Settings {
            std::string Title = "Yulduz Window";
            std::uint32_t Width = 1200;
            std::uint32_t Height = 1000;
            bool Resizable = true;
            bool Fullscreen = false;
            bool FullscreenSize = true;
            std::optional<std::reference_wrapper<EventDispatcher>> EventDispatcher = std::nullopt;
        };

       public:
        static void PollEvents();

        static std::shared_ptr<Window> New(const Settings &settings);

       public:
        Window(GLFWwindow *window);
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        void registerCallbacks(EventDispatcher &dispatcher);
        void setSize(std::uint32_t width, std::uint32_t height);
        void setWidth(std::uint32_t width);
        void setHeight(std::uint32_t height);
        void setCursorMode(CursorMode mode);
        void maximize();
        void minimize();
        void restore();
        void makeFullscreen(bool screenSize = true);
        void makeWindowed();
        void close();

        GLFWwindow *get() const;
        std::string getTitle() const;
        std::array<std::uint32_t, 2> getSize() const;
        std::uint32_t getWidth() const;
        std::uint32_t getHeight() const;
        std::array<double, 2> getMousePosition() const;
        std::uint32_t getMouseX() const;
        std::uint32_t getMouseY() const;
        CursorMode getCursorMode() const;

        bool isClosed() const;
        bool isRunning() const;
        bool isMaximized() const;
        bool isMinimized() const;
        bool isFocused() const;
        bool isFullscreen() const;

        bool isKeyModPressed(KeyMod mod) const;
        bool isKeyDown(KeyCode key) const;
        bool isKeyUp(KeyCode key) const;
        bool isMouseButtonDown(MouseButton button) const;
        bool isMouseButtonUp(MouseButton button) const;

        WGPUSurface getWGPUSurface(WGPUInstance instance) const;

       private:
        class GlfwState {
           public:
            GlfwState();
            ~GlfwState();
        };

       private:
        GLFWwindow *m_Window;
        std::optional<std::reference_wrapper<EventDispatcher>> m_Dispatcher;
        std::uint32_t m_PrevWidth;
        std::uint32_t m_PrevHeight;

        KeyMod m_KeyMods;
        bool m_Keys[GLFW_KEY_LAST + 1];
        bool m_MouseButtons[GLFW_MOUSE_BUTTON_LAST + 1];

        static GlfwState g_GlfwState;

       private:
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
