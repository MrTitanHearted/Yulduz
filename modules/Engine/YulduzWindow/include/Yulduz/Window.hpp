#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzWindow/Enums.hpp>
#include <YulduzWindow/Events.hpp>

namespace Yulduz {
    class Window {
       public:
        struct Settings {
            std::string title = "Yulduz Window";
            uint32_t width = 1200;
            uint32_t height = 1000;
            bool resizable = true;
            bool fullscreen = false;
            bool fullscreenSize = true;
            std::optional<std::reference_wrapper<EventDispatcher>> eventDispatcher = std::nullopt;
        };

       public:
        static void InitializeGlfw();
        static void ShutdownGlfw();
        static void PollEvents();

        static std::shared_ptr<Window> New(Settings Settings);

       public:
        Window(GLFWwindow *window);
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        void registerCallbacks(EventDispatcher &dispatcher);
        void setSize(uint32_t width, uint32_t height);
        void setCursorMode(CursorMode mode);
        void maximize();
        void minimize();
        void restore();
        void makeFullscreen(bool screenSize = true);
        void makeWindowed();
        void close();
        
        GLFWwindow *get() const;
        std::string getTitle() const;
        std::array<uint32_t, 2> getSize() const;
        uint32_t getWidth() const;
        uint32_t getHeight() const;
        std::array<double, 2> getMousePosition() const;
        uint32_t getMouseX() const;
        uint32_t getMouseY() const;
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
        GLFWwindow *m_Window;
        std::optional<std::reference_wrapper<EventDispatcher>> m_Dispatcher;
        uint32_t m_PrevWidth;
        uint32_t m_PrevHeight;

        KeyMod m_KeyMods;
        bool m_Keys[GLFW_KEY_LAST + 1]{false};
        bool m_MouseButtons[GLFW_MOUSE_BUTTON_LAST + 1]{false};

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
        static void GlfwWindowSizeCallback(GLFWwindow *window, int width, int height);
        static void GlfwWindowContentScaleCallback(GLFWwindow *window, float xScale, float yScale);
        static void GlfwWindowPositionCallback(GLFWwindow *window, int x, int y);
        static void GlfwWindowMaximizeCallback(GLFWwindow *window, int isMaximized);
        static void GlfwWindowMinimizeCallback(GLFWwindow *window, int isMinimized);
        static void GlfwWindowFocusCallback(GLFWwindow *window, int focused);
        static void GlfwWindowKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void GlfwWindowCharCallback(GLFWwindow *window, unsigned int codepoint);
        static void GlfwWindowMousePositionCallback(GLFWwindow *window, double x, double y);
        static void GlfwWindowMouseEnterCallback(GLFWwindow *window, int entered);
        static void GlfwWindowMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        static void GlfwWindowMouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset);
    };
}  // namespace Yulduz
