#include <Yulduz/Window.hpp>

#if defined(YULDUZ_PLATFORM_WINDOWS)
#include <dwmapi.h>
#endif

namespace Yulduz {
    void Window::InitializeGlfw() {
        if (glfwInit() == GLFW_FALSE) {
            YZERROR("Failed to initialize GLFW!");
            throw std::runtime_error("Failed to initialize GLFW!");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    void Window::ShutdownGlfw() {
        glfwTerminate();
    }

    void Window::PollEvents() {
        glfwPollEvents();
    }

    std::shared_ptr<Window> Window::New(Settings settings) {
        glfwWindowHint(GLFW_RESIZABLE, settings.resizable ? GLFW_TRUE : GLFW_FALSE);

        GLFWmonitor *monitor = settings.fullscreen ? glfwGetPrimaryMonitor() : nullptr;
        if (settings.fullscreen && settings.fullscreenSize) {
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            settings.width = mode->width;
            settings.height = mode->height;
        }

        GLFWwindow *window = glfwCreateWindow(settings.width, settings.height, settings.title.c_str(), monitor, nullptr);

        glfwShowWindow(window);

        std::shared_ptr<Window> yzWindow = std::make_shared<Window>(window);
        if (settings.eventDispatcher) {
            yzWindow->registerCallbacks(settings.eventDispatcher.value());
        }

        return yzWindow;
    }

    Window::Window(GLFWwindow *window) {
        YZDEBUG("Initializing Window: '{}'", glfwGetWindowTitle(window));

        m_Window = window;
        m_Dispatcher = std::nullopt;
        m_PrevWidth = 0;
        m_PrevHeight = 0;

        glfwSetWindowUserPointer(m_Window, this);
        glfwSetWindowCloseCallback(m_Window, GlfwWindowCloseCallback);
        glfwSetWindowSizeCallback(m_Window, GlfwWindowSizeCallback);
        glfwSetWindowPosCallback(m_Window, GlfwWindowPositionCallback);
        glfwSetWindowContentScaleCallback(m_Window, GlfwWindowContentScaleCallback);
        glfwSetWindowPosCallback(m_Window, GlfwWindowPositionCallback);
        glfwSetWindowMaximizeCallback(m_Window, GlfwWindowMaximizeCallback);
        glfwSetWindowIconifyCallback(m_Window, GlfwWindowMinimizeCallback);
        glfwSetWindowFocusCallback(m_Window, GlfwWindowFocusCallback);
        glfwSetKeyCallback(m_Window, GlfwWindowKeyCallback);
        glfwSetCharCallback(m_Window, GlfwWindowCharCallback);
        glfwSetCursorPosCallback(m_Window, GlfwWindowMousePositionCallback);
        glfwSetCursorEnterCallback(m_Window, GlfwWindowMouseEnterCallback);
        glfwSetMouseButtonCallback(m_Window, GlfwWindowMouseButtonCallback);
        glfwSetScrollCallback(m_Window, GlfwWindowMouseScrollCallback);

#if defined(YULDUZ_PLATFORM_WINDOWS)
        {
            HWND hWnd = glfwGetWin32Window(m_Window);
            BOOL enabled = TRUE;

            DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &enabled, sizeof(enabled));
        }
#endif

        m_KeyMods = KeyMod::NONE;
        std::fill(std::begin(m_Keys), std::end(m_Keys), false);
        std::fill(std::begin(m_MouseButtons), std::end(m_MouseButtons), false);
    }

    Window::~Window() {
        YZDEBUG("Releasing Window: '{}'", glfwGetWindowTitle(m_Window));

        glfwDestroyWindow(m_Window);
    }

    void Window::registerCallbacks(EventDispatcher &dispatcher) {
        if (m_Dispatcher) return;

        m_Dispatcher = dispatcher;

        dispatcher.addCallback<WindowCloseEvent>(std::bind(&Window::closeCallback, this, std::placeholders::_1));
        dispatcher.addCallback<WindowResizeEvent>(std::bind(&Window::resizeCallback, this, std::placeholders::_1));
        dispatcher.addCallback<WindowMoveEvent>(std::bind(&Window::moveCallback, this, std::placeholders::_1));
        dispatcher.addCallback<WindowContentScaleEvent>(std::bind(&Window::contentScaleCallback, this, std::placeholders::_1));
        dispatcher.addCallback<WindowMouseMoveEvent>(std::bind(&Window::mouseMoveCallback, this, std::placeholders::_1));
        dispatcher.addCallback<WindowMaximizeEvent>(std::bind(&Window::maximizeCallback, this, std::placeholders::_1));
        dispatcher.addCallback<WindowMinimizeEvent>(std::bind(&Window::minimizeCallback, this, std::placeholders::_1));
        dispatcher.addCallback<WindowGainFocusEvent>(std::bind(&Window::gainFocusCallback, this, std::placeholders::_1));
        dispatcher.addCallback<WindowLoseFocusEvent>(std::bind(&Window::loseFocusCallback, this, std::placeholders::_1));
        dispatcher.addCallback<WindowKeyEvent>(std::bind(&Window::keyCallback, this, std::placeholders::_1));
        dispatcher.addCallback<WindowCharEvent>(std::bind(&Window::charCallback, this, std::placeholders::_1));
        dispatcher.addCallback<WindowMouseButtonEvent>(std::bind(&Window::mouseButtonCallback, this, std::placeholders::_1));
        dispatcher.addCallback<WindowMouseScrollEvent>(std::bind(&Window::mouseScrollCallback, this, std::placeholders::_1));
    }

    void Window::setSize(uint32_t width, uint32_t height) {
        glfwSetWindowSize(m_Window, width, height);
    }

    void Window::setCursorMode(CursorMode mode) {
        glfwSetInputMode(m_Window, GLFW_CURSOR, static_cast<int>(mode));
    }

    void Window::maximize() {
        glfwMaximizeWindow(m_Window);
    }

    void Window::minimize() {
        glfwIconifyWindow(m_Window);
    }

    void Window::restore() {
        glfwRestoreWindow(m_Window);
    }

    void Window::makeFullscreen(bool screenSize) {
        auto [width, height] = getSize();
        m_PrevWidth = width;
        m_PrevHeight = height;

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        if (screenSize) {
            width = mode->width;
            height = mode->height;
        }
        glfwSetWindowMonitor(m_Window, monitor, 0, 0, width, height, mode->refreshRate);
    }

    void Window::makeWindowed() {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        uint32_t x = mode->width / 2 - m_PrevWidth / 2;
        uint32_t y = mode->height / 2 - m_PrevHeight / 2;
        glfwSetWindowMonitor(m_Window, nullptr, x, y, m_PrevWidth, m_PrevHeight, mode->refreshRate);
    }

    void Window::close() {
        glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
    }

    GLFWwindow *Window::get() const {
        return m_Window;
    }

    std::string Window::getTitle() const {
        return glfwGetWindowTitle(m_Window);
    }

    std::array<uint32_t, 2> Window::getSize() const {
        int width, height;
        glfwGetWindowSize(m_Window, &width, &height);
        return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
    }

    uint32_t Window::getWidth() const {
        int width;
        glfwGetWindowSize(m_Window, &width, nullptr);
        return static_cast<uint32_t>(width);
    }

    uint32_t Window::getHeight() const {
        int height;
        glfwGetWindowSize(m_Window, nullptr, &height);
        return static_cast<uint32_t>(height);
    }

    std::array<double, 2> Window::getMousePosition() const {
        double x, y;
        glfwGetCursorPos(m_Window, &x, &y);
        return {x, y};
    }

    uint32_t Window::getMouseX() const {
        double x;
        glfwGetCursorPos(m_Window, &x, nullptr);
        return x;
    }

    uint32_t Window::getMouseY() const {
        double y;
        glfwGetCursorPos(m_Window, nullptr, &y);
        return y;
    }

    CursorMode Window::getCursorMode() const {
        return static_cast<CursorMode>(glfwGetInputMode(m_Window, GLFW_CURSOR));
    }

    bool Window::isClosed() const {
        return glfwWindowShouldClose(m_Window) == GLFW_TRUE;
    }

    bool Window::isRunning() const {
        return glfwWindowShouldClose(m_Window) == GLFW_FALSE;
    }

    bool Window::isMaximized() const {
        return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED) == GLFW_TRUE;
    }

    bool Window::isMinimized() const {
        return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED) == GLFW_TRUE;
    }

    bool Window::isFocused() const {
        return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) == GLFW_TRUE;
    }

    bool Window::isFullscreen() const {
        return glfwGetWindowMonitor(m_Window) != nullptr;
    }

    bool Window::isKeyModPressed(KeyMod mod) const {
        return static_cast<int>(m_KeyMods & mod);
    }

    bool Window::isKeyDown(KeyCode key) const {
        return m_Keys[static_cast<size_t>(key)];
    }

    bool Window::isKeyUp(KeyCode key) const {
        return !m_Keys[static_cast<size_t>(key)];
    }

    bool Window::isMouseButtonDown(MouseButton button) const {
        return m_MouseButtons[static_cast<size_t>(button)];
    }

    bool Window::isMouseButtonUp(MouseButton button) const {
        return !m_MouseButtons[static_cast<size_t>(button)];
    }

    WGPUSurface Window::getWGPUSurface(WGPUInstance instance) const {
        return glfwGetWGPUSurface(m_Window, instance);
    }
}  // namespace Yulduz
