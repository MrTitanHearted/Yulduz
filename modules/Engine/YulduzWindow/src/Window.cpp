#include <Yulduz/Window.hpp>

#if defined(YULDUZ_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <dwmapi.h>
#include <GLFW/glfw3native.h>
#endif

namespace Yulduz {
    Window::GlfwState Window::g_GlfwState{};

    void Window::PollEvents() {
        glfwPollEvents();
    }

    std::shared_ptr<Window> Window::New(const Settings &settings) {
        glfwWindowHint(GLFW_RESIZABLE, settings.Resizable ? GLFW_TRUE : GLFW_FALSE);

        std::uint32_t width = settings.Width;
        std::uint32_t height = settings.Height;

        GLFWmonitor *monitor = settings.Fullscreen ? glfwGetPrimaryMonitor() : nullptr;
        if (settings.Fullscreen && settings.FullscreenSize) {
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            width = mode->width;
            height = mode->height;
        }

        GLFWwindow *glfwWindow = glfwCreateWindow(width, height, settings.Title.c_str(), monitor, nullptr);

        glfwShowWindow(glfwWindow);

        std::shared_ptr<Window> window = std::make_shared<Window>(glfwWindow);
        if (settings.EventDispatcher) {
            window->registerCallbacks(settings.EventDispatcher.value());
        }

        return window;
    }

    Window::Window(GLFWwindow *window)
        : m_Window(window), m_Dispatcher(std::nullopt) {
        YZDEBUG("Initializing Window: '{}'", glfwGetWindowTitle(window));

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
            DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &enabled, sizeof(BOOL));
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
#if defined(YULDUZ_BUILD_TYPE_DEBUG)
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
#endif
    }

    void Window::setSize(std::uint32_t width, std::uint32_t height) {
        glfwSetWindowSize(m_Window, width, height);
    }

    void Window::setWidth(std::uint32_t width) {
        std::int32_t height;
        glfwGetWindowSize(m_Window, nullptr, &height);
        glfwSetWindowSize(m_Window, width, height);
    }

    void Window::setHeight(std::uint32_t height) {
        std::int32_t width;
        glfwGetWindowSize(m_Window, &width, nullptr);
        glfwSetWindowSize(m_Window, width, height);
    }

    void Window::setCursorMode(CursorMode mode) {
        glfwSetInputMode(m_Window, GLFW_CURSOR, static_cast<std::int32_t>(mode));
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
        std::uint32_t x = mode->width / 2 - m_PrevWidth / 2;
        std::uint32_t y = mode->height / 2 - m_PrevHeight / 2;
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

    std::array<std::uint32_t, 2> Window::getSize() const {
        std::int32_t width, height;
        glfwGetWindowSize(m_Window, &width, &height);
        return {static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)};
    }

    std::uint32_t Window::getWidth() const {
        std::int32_t width;
        glfwGetWindowSize(m_Window, &width, nullptr);
        return width;
    }

    std::uint32_t Window::getHeight() const {
        std::int32_t height;
        glfwGetWindowSize(m_Window, nullptr, &height);
        return height;
    }

    std::array<double, 2> Window::getMousePosition() const {
        double x, y;
        glfwGetCursorPos(m_Window, &x, &y);
        return {x, y};
    }

    std::uint32_t Window::getMouseX() const {
        double x;
        glfwGetCursorPos(m_Window, &x, nullptr);
        return x;
    }

    std::uint32_t Window::getMouseY() const {
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
        return static_cast<std::int32_t>(m_KeyMods & mod) > 0;
    }

    bool Window::isKeyDown(KeyCode key) const {
        return m_Keys[static_cast<std::size_t>(key)];
    }

    bool Window::isKeyUp(KeyCode key) const {
        return !m_Keys[static_cast<std::size_t>(key)];
    }

    bool Window::isMouseButtonDown(MouseButton button) const {
        return m_MouseButtons[static_cast<std::size_t>(button)];
    }

    bool Window::isMouseButtonUp(MouseButton button) const {
        return !m_MouseButtons[static_cast<std::size_t>(button)];
    }

    WGPUSurface Window::getWGPUSurface(WGPUInstance instance) const {
        return glfwGetWGPUSurface(m_Window, instance);
    }

    Window::GlfwState::GlfwState() {
        LOG_INFO("Initializing GLFW");

        if (glfwInit() == GLFW_FALSE) {
            LOG_FATAL("Failed to initialize GLFW");
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    Window::GlfwState::~GlfwState() {
        LOG_INFO("Terminating GLFW");

        glfwTerminate();
    }

}  // namespace Yulduz
