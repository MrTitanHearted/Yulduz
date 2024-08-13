#include <Yulduz/Window.hpp>

#if defined(YULDUZ_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#endif

namespace Yulduz {
    Window::GlfwState Window::g_GlfwState{};
    EventDispatcher *Window::g_EventDispatcher{nullptr};

    void Window::SetEventDispatcher(EventDispatcher &dispatcher) {
        g_EventDispatcher = &dispatcher;
    }

    void Window::PollEvents() {
        glfwPollEvents();
    }

    Window::Window(const Settings &settings) {
        assert(g_EventDispatcher != nullptr && "EventDispatcher must be set first");
        YZDEBUG("Initializing Window: '{}'", settings.Title);

        glfwWindowHint(GLFW_RESIZABLE, settings.Resizable ? GLFW_TRUE : GLFW_FALSE);
        std::uint32_t width = settings.Width;
        std::uint32_t height = settings.Height;
        GLFWmonitor *monitor = settings.Fullscreen ? glfwGetPrimaryMonitor() : nullptr;
        if (settings.Fullscreen && settings.FullscreenMonitorSize && monitor != nullptr) {
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            width = mode->width;
            height = mode->height;
        }

        m_Window = glfwCreateWindow(width, height, settings.Title.c_str(), monitor, nullptr);
        glfwShowWindow(m_Window);

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

        m_KeyMods = KeyMod::None;
        std::fill(std::begin(m_Keys), std::end(m_Keys), false);
        std::fill(std::begin(m_MouseButtons), std::end(m_MouseButtons), false);

#if defined(YULDUZ_BUILD_TYPE_DEBUG)
        g_EventDispatcher->addCallback<WindowCloseEvent>(&Window::closeCallback, this);
        g_EventDispatcher->addCallback<WindowResizeEvent>(&Window::resizeCallback, this);
        g_EventDispatcher->addCallback<WindowMoveEvent>(&Window::moveCallback, this);
        g_EventDispatcher->addCallback<WindowContentScaleEvent>(&Window::contentScaleCallback, this);
        g_EventDispatcher->addCallback<WindowMouseMoveEvent>(&Window::mouseMoveCallback, this);
        g_EventDispatcher->addCallback<WindowMaximizeEvent>(&Window::maximizeCallback, this);
        g_EventDispatcher->addCallback<WindowMinimizeEvent>(&Window::minimizeCallback, this);
        g_EventDispatcher->addCallback<WindowGainFocusEvent>(&Window::gainFocusCallback, this);
        g_EventDispatcher->addCallback<WindowLoseFocusEvent>(&Window::loseFocusCallback, this);
        g_EventDispatcher->addCallback<WindowKeyEvent>(&Window::keyCallback, this);
        g_EventDispatcher->addCallback<WindowCharEvent>(&Window::charCallback, this);
        g_EventDispatcher->addCallback<WindowMouseButtonEvent>(&Window::mouseButtonCallback, this);
        g_EventDispatcher->addCallback<WindowMouseScrollEvent>(&Window::mouseScrollCallback, this);
#endif

        m_PrevWidth = 0;
        m_PrevHeight = 0;
        m_PrevX = 0;
        m_PrevY = 0;

        if (monitor != nullptr) {
            m_PrevWidth = width / 2;
            m_PrevHeight = height / 2;
            m_PrevX = (width - width) / 2;
            m_PrevY = (height - height) / 2;
        }
    }

    Window::~Window() {
        YZDEBUG("Releasing Window: '{}'", getTitle());

        if (m_Window)
            glfwDestroyWindow(m_Window);
    }

    void Window::setTitle(const std::string &title) {
        glfwSetWindowTitle(m_Window, title.c_str());
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

    void Window::setPosition(std::uint32_t x, std::uint32_t y) {
        glfwSetWindowPos(m_Window, x, y);
    }

    void Window::setX(std::uint32_t x) {
        std::int32_t y;
        glfwGetWindowPos(m_Window, nullptr, &y);
        glfwSetWindowPos(m_Window, x, y);
    }

    void Window::setY(std::uint32_t y) {
        std::int32_t x;
        glfwGetWindowPos(m_Window, &x, nullptr);
        glfwSetWindowPos(m_Window, x, y);
    }

    void Window::setMousePosition(double x, double y) {
        glfwSetCursorPos(m_Window, x, y);
    }

    void Window::setMouseX(double x) {
        double y;
        glfwGetCursorPos(m_Window, nullptr, &y);
        glfwSetCursorPos(m_Window, x, y);
    }

    void Window::setMouseY(double y) {
        double x;
        glfwGetCursorPos(m_Window, &x, nullptr);
        glfwSetCursorPos(m_Window, x, y);
    }

    void Window::setCursorMode(CursorMode mode) {
        glfwSetInputMode(m_Window, GLFW_CURSOR, static_cast<std::int32_t>(mode));
    }

    void Window::setResizable(bool resizable) {
        glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
    }

    void Window::setRunning(bool running) {
        glfwSetWindowShouldClose(m_Window, running ? GLFW_FALSE : GLFW_TRUE);
    }

    void Window::makeFullscreen(bool monitorSize) {
        auto [width, height] = getSize();
        auto [x, y] = getPosition();
        m_PrevWidth = width;
        m_PrevHeight = height;
        m_PrevX = x;
        m_PrevY = y;

        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        if (monitorSize) {
            width = mode->width;
            height = mode->height;
        }

        glfwSetWindowMonitor(m_Window, monitor, 0, 0, width, height, mode->refreshRate);
    }

    void Window::makeWindowed() {
        glfwSetWindowMonitor(m_Window, nullptr, m_PrevX, m_PrevY, m_PrevWidth, m_PrevHeight, 0);
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
        std::int32_t height;
        glfwGetWindowSize(m_Window, nullptr, &height);
        return height;
    }

    std::uint32_t Window::getHeight() const {
        std::int32_t width;
        glfwGetWindowSize(m_Window, &width, nullptr);
        return width;
    }

    std::array<std::uint32_t, 2> Window::getPosition() const {
        std::int32_t x, y;
        glfwGetWindowPos(m_Window, &x, &y);
        return {static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y)};
    }

    std::uint32_t Window::getX() const {
        std::int32_t y;
        glfwGetWindowPos(m_Window, nullptr, &y);
        return y;
    }

    std::uint32_t Window::getY() const {
        std::int32_t x;
        glfwGetWindowPos(m_Window, &x, nullptr);
        return x;
    }

    std::array<double, 2> Window::getMousePosition() const {
        double x, y;
        glfwGetCursorPos(m_Window, &x, &y);
        return {x, y};
    }

    double Window::getMouseX() const {
        double y;
        glfwGetCursorPos(m_Window, nullptr, &y);
        return y;
    }

    double Window::getMouseY() const {
        double x;
        glfwGetCursorPos(m_Window, &x, nullptr);
        return x;
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

    bool Window::isResizable() const {
        return glfwGetWindowAttrib(m_Window, GLFW_RESIZABLE) == GLFW_TRUE;
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

    const char *GetKeyCodeName(KeyCode key) {
        const char *keyName = glfwGetKeyName(static_cast<std::int32_t>(key), 0);
        if (keyName != nullptr) return keyName;
        switch (key) {
            case KeyCode::Escape:
                return "Escape";
            case KeyCode::Delete:
                return "Delete";
            case KeyCode::LeftAlt:
                return "Left Alt";
            case KeyCode::RightAlt:
                return "Right Alt";
            case KeyCode::LeftControl:
                return "Left Ctrl";
            case KeyCode::RightControl:
                return "Right Ctrl";
            case KeyCode::LeftShift:
                return "Left Shift";
            case KeyCode::RightShift:
                return "Right Shift";
            case KeyCode::LeftSuper:
                return "Left Super";
            case KeyCode::RightSuper:
                return "Right Super";
            case KeyCode::CapsLock:
                return "Caps Lock";
            case KeyCode::Tab:
                return "Tab";
            case KeyCode::Backspace:
                return "Backspace";
            case KeyCode::Left:
                return "Left Arrow";
            case KeyCode::Down:
                return "Down Arrow";
            case KeyCode::Up:
                return "Up Arrow";
            case KeyCode::Right:
                return "Right Arrow";
            case KeyCode::NumLock:
                return "Num Lock";
            case KeyCode::PrintScreen:
                return "Print Screen";
            case KeyCode::ScrollLock:
                return "Scroll Lock";
            case KeyCode::Pause:
                return "Pause";
            case KeyCode::Home:
                return "Home";
            case KeyCode::End:
                return "End";
            case KeyCode::PageUp:
                return "Page Up";
            case KeyCode::PageDown:
                return "Page Down";
            case KeyCode::Space:
                return "Space";
            case KeyCode::Enter:
            case KeyCode::KpEnter:
                return "Enter";
            default:
                return "Unknown Key";
        }
    }

    const char *GetKeyModName(KeyMod mod) {
        if (mod == KeyMod::None) {
            return "None";
        } else if (mod == KeyMod::All) {
            return "All Modifiers";
        }

        static std::string name;
        name.clear();
        if (static_cast<std::uint32_t>(mod) & static_cast<std::uint32_t>(KeyMod::Shift)) {
            name += "Shift";
        }
        if (static_cast<std::uint32_t>(mod) & static_cast<std::uint32_t>(KeyMod::Control)) {
            if (!name.empty()) name += " + ";
            name += "Control";
        }
        if (static_cast<std::uint32_t>(mod) & static_cast<std::uint32_t>(KeyMod::Alt)) {
            if (!name.empty()) name += " + ";
            name += "Alt";
        }
        if (static_cast<std::uint32_t>(mod) & static_cast<std::uint32_t>(KeyMod::Super)) {
            if (!name.empty()) name += " + ";
            name += "Super";
        }
        if (static_cast<std::uint32_t>(mod) & static_cast<std::uint32_t>(KeyMod::CapsLock)) {
            if (!name.empty()) name += " + ";
            name += "Caps Lock";
        }
        if (static_cast<std::uint32_t>(mod) & static_cast<std::uint32_t>(KeyMod::NumLock)) {
            if (!name.empty()) name += " + ";
            name += "Num Lock";
        }
        return &name[0];
    }

    const char *GetKeyActionName(KeyAction action) {
        switch (action) {
            case KeyAction::Press:
                return "Press";
            case KeyAction::Release:
                return "Release";
            case KeyAction::Repeat:
                return "Repeat";
        }
    }

    const char *GetMouseButtonName(MouseButton button) {
        switch (button) {
            case MouseButton::Left:
                return "Left Mouse Button";
            case MouseButton::Right:
                return "Right Mouse Button";
            case MouseButton::Middle:
                return "Middle Mouse Button";
            case MouseButton::Last:
                return "Last Mouse Button";
            default:
                static std::array<char, 16> buttonName;
                std::uint32_t buttonFmt = static_cast<std::int32_t>(button);
                std::vformat_to(&buttonName[0], "Mouse Button {}", std::make_format_args(buttonFmt));
                return &buttonName[0];
        }
    }

    const char *GetCursorModeName(CursorMode mode) {
        switch (mode) {
            case CursorMode::Normal:
                return "Cursor Mode Normal";
            case CursorMode::Hidden:
                return "Cursor Mode Hidden";
            case CursorMode::Disabled:
                return "Cursor Mode Disabled";
            case CursorMode::Captured:
                return "Cursor Mode Captured";
            default:
                return "Unknown Cursor Mode";
        }
    }
}  // namespace Yulduz
