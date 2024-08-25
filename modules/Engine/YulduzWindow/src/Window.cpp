#include <Yulduz/Window.hpp>

#if defined(YULDUZ_PLATFORM_WINDOWS)
#define WIN32_LEAN_AND_MEAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <dwmapi.h>
#endif

namespace Yulduz {
    Window::GlfwState Window::g_GlfwState{};

    void Window::PollEvents() {
        glfwPollEvents();
    }

    Window::Window(const Settings &settings) {
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
        EventObserver &observer = EventObserver::GetDefault();
        m_WindowCloseCallbackIndex = observer.addCallback<WindowCloseEvent>(&Window::closeCallback, this);
        m_WindowResizeCallbackIndex = observer.addCallback<WindowResizeEvent>(&Window::resizeCallback, this);
        m_WindowMoveCallbackIndex = observer.addCallback<WindowMoveEvent>(&Window::moveCallback, this);
        m_WindowContentScaleCallbackIndex = observer.addCallback<WindowContentScaleEvent>(&Window::contentScaleCallback, this);
        m_WindowMouseMoveCallbackIndex = observer.addCallback<WindowMouseMoveEvent>(&Window::mouseMoveCallback, this);
        m_WindowMaximizeCallbackIndex = observer.addCallback<WindowMaximizeEvent>(&Window::maximizeCallback, this);
        m_WindowMinimizeCallbackIndex = observer.addCallback<WindowMinimizeEvent>(&Window::minimizeCallback, this);
        m_WindowGainFocusCallbackIndex = observer.addCallback<WindowGainFocusEvent>(&Window::gainFocusCallback, this);
        m_WindowLoseFocusCallbackIndex = observer.addCallback<WindowLoseFocusEvent>(&Window::loseFocusCallback, this);
        m_WindowKeyCallbackIndex = observer.addCallback<WindowKeyEvent>(&Window::keyCallback, this);
        m_WindowCharCallbackIndex = observer.addCallback<WindowCharEvent>(&Window::charCallback, this);
        m_WindowMouseButtonCallbackIndex = observer.addCallback<WindowMouseButtonEvent>(&Window::mouseButtonCallback, this);
        m_WindowMouseScrollCallbackIndex = observer.addCallback<WindowMouseScrollEvent>(&Window::mouseScrollCallback, this);
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

    Window::Window()
        : m_Window{nullptr},
          m_PrevWidth{},
          m_PrevHeight{},
          m_PrevX{},
          m_PrevY{},
          m_KeyMods{KeyMod::None},
          m_Keys{},
          m_MouseButtons{} {}

    Window::~Window() {
        if (!m_Window) return;
        YZDEBUG("Releasing Window: '{}'", getTitle());

        glfwDestroyWindow(m_Window);
#if defined(YULDUZ_BUILD_TYPE_DEBUG)
        EventObserver &observer = EventObserver::GetDefault();
        observer.remove<WindowCloseEvent>(m_WindowCloseCallbackIndex);
        observer.remove<WindowResizeEvent>(m_WindowResizeCallbackIndex);
        observer.remove<WindowMoveEvent>(m_WindowMoveCallbackIndex);
        observer.remove<WindowContentScaleEvent>(m_WindowContentScaleCallbackIndex);
        observer.remove<WindowMouseMoveEvent>(m_WindowMouseMoveCallbackIndex);
        observer.remove<WindowMaximizeEvent>(m_WindowMaximizeCallbackIndex);
        observer.remove<WindowMinimizeEvent>(m_WindowMinimizeCallbackIndex);
        observer.remove<WindowGainFocusEvent>(m_WindowGainFocusCallbackIndex);
        observer.remove<WindowLoseFocusEvent>(m_WindowLoseFocusCallbackIndex);
        observer.remove<WindowKeyEvent>(m_WindowKeyCallbackIndex);
        observer.remove<WindowCharEvent>(m_WindowCharCallbackIndex);
        observer.remove<WindowMouseButtonEvent>(m_WindowMouseButtonCallbackIndex);
        observer.remove<WindowMouseScrollEvent>(m_WindowMouseScrollCallbackIndex);
#endif
    }

    Window::Window(Window &&other)
        : m_Window{other.m_Window},
          m_PrevWidth{other.m_PrevWidth},
          m_PrevHeight{other.m_PrevHeight},
          m_PrevX{other.m_PrevX},
          m_PrevY{other.m_PrevY},
          m_KeyMods{other.m_KeyMods},
          m_Keys{std::move(other.m_Keys)},
          m_MouseButtons{std::move(other.m_MouseButtons)} {
        assert(other.m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwSetWindowUserPointer(m_Window, this);
        other.m_Window = nullptr;
        other.m_PrevWidth = 0;
        other.m_PrevHeight = 0;
        other.m_PrevX = 0;
        other.m_PrevY = 0;
        other.m_KeyMods = KeyMod::None;
        std::fill(std::begin(other.m_Keys), std::end(other.m_Keys), false);
        std::fill(std::begin(other.m_MouseButtons), std::end(other.m_MouseButtons), false);
#if defined(YULDUZ_BUILD_TYPE_DEBUG)
        EventObserver &observer = EventObserver::GetDefault();
        observer.remove<WindowCloseEvent>(other.m_WindowCloseCallbackIndex);
        observer.remove<WindowResizeEvent>(other.m_WindowResizeCallbackIndex);
        observer.remove<WindowMoveEvent>(other.m_WindowMoveCallbackIndex);
        observer.remove<WindowContentScaleEvent>(other.m_WindowContentScaleCallbackIndex);
        observer.remove<WindowMouseMoveEvent>(other.m_WindowMouseMoveCallbackIndex);
        observer.remove<WindowMaximizeEvent>(other.m_WindowMaximizeCallbackIndex);
        observer.remove<WindowMinimizeEvent>(other.m_WindowMinimizeCallbackIndex);
        observer.remove<WindowGainFocusEvent>(other.m_WindowGainFocusCallbackIndex);
        observer.remove<WindowLoseFocusEvent>(other.m_WindowLoseFocusCallbackIndex);
        observer.remove<WindowKeyEvent>(other.m_WindowKeyCallbackIndex);
        observer.remove<WindowCharEvent>(other.m_WindowCharCallbackIndex);
        observer.remove<WindowMouseButtonEvent>(other.m_WindowMouseButtonCallbackIndex);
        observer.remove<WindowMouseScrollEvent>(other.m_WindowMouseScrollCallbackIndex);
        m_WindowCloseCallbackIndex = observer.addCallback<WindowCloseEvent>(&Window::closeCallback, this);
        m_WindowResizeCallbackIndex = observer.addCallback<WindowResizeEvent>(&Window::resizeCallback, this);
        m_WindowMoveCallbackIndex = observer.addCallback<WindowMoveEvent>(&Window::moveCallback, this);
        m_WindowContentScaleCallbackIndex = observer.addCallback<WindowContentScaleEvent>(&Window::contentScaleCallback, this);
        m_WindowMouseMoveCallbackIndex = observer.addCallback<WindowMouseMoveEvent>(&Window::mouseMoveCallback, this);
        m_WindowMaximizeCallbackIndex = observer.addCallback<WindowMaximizeEvent>(&Window::maximizeCallback, this);
        m_WindowMinimizeCallbackIndex = observer.addCallback<WindowMinimizeEvent>(&Window::minimizeCallback, this);
        m_WindowGainFocusCallbackIndex = observer.addCallback<WindowGainFocusEvent>(&Window::gainFocusCallback, this);
        m_WindowLoseFocusCallbackIndex = observer.addCallback<WindowLoseFocusEvent>(&Window::loseFocusCallback, this);
        m_WindowKeyCallbackIndex = observer.addCallback<WindowKeyEvent>(&Window::keyCallback, this);
        m_WindowCharCallbackIndex = observer.addCallback<WindowCharEvent>(&Window::charCallback, this);
        m_WindowMouseButtonCallbackIndex = observer.addCallback<WindowMouseButtonEvent>(&Window::mouseButtonCallback, this);
        m_WindowMouseScrollCallbackIndex = observer.addCallback<WindowMouseScrollEvent>(&Window::mouseScrollCallback, this);
#endif
    }

    Window &Window::operator=(Window &&other) {
        assert(other.m_Window != nullptr && "GLFWwindow cannot be nullptr");

        if (&other != this) {
            if (m_Window) glfwDestroyWindow(m_Window);

            m_Window = other.m_Window;
            m_PrevWidth = other.m_PrevWidth;
            m_PrevHeight = other.m_PrevHeight;
            m_PrevX = other.m_PrevX;
            m_PrevY = other.m_PrevY;
            m_KeyMods = other.m_KeyMods;
            m_Keys = std::move(other.m_Keys);
            m_MouseButtons = std::move(other.m_MouseButtons);
            glfwSetWindowUserPointer(m_Window, this);

            other.m_Window = nullptr;
            other.m_PrevWidth = 0;
            other.m_PrevHeight = 0;
            other.m_PrevX = 0;
            other.m_PrevY = 0;
            other.m_KeyMods = KeyMod::None;
            std::fill(std::begin(other.m_Keys), std::end(other.m_Keys), false);
            std::fill(std::begin(other.m_MouseButtons), std::end(other.m_MouseButtons), false);

#if defined(YULDUZ_BUILD_TYPE_DEBUG)
        EventObserver &observer = EventObserver::GetDefault();
        observer.remove<WindowCloseEvent>(other.m_WindowCloseCallbackIndex);
        observer.remove<WindowResizeEvent>(other.m_WindowResizeCallbackIndex);
        observer.remove<WindowMoveEvent>(other.m_WindowMoveCallbackIndex);
        observer.remove<WindowContentScaleEvent>(other.m_WindowContentScaleCallbackIndex);
        observer.remove<WindowMouseMoveEvent>(other.m_WindowMouseMoveCallbackIndex);
        observer.remove<WindowMaximizeEvent>(other.m_WindowMaximizeCallbackIndex);
        observer.remove<WindowMinimizeEvent>(other.m_WindowMinimizeCallbackIndex);
        observer.remove<WindowGainFocusEvent>(other.m_WindowGainFocusCallbackIndex);
        observer.remove<WindowLoseFocusEvent>(other.m_WindowLoseFocusCallbackIndex);
        observer.remove<WindowKeyEvent>(other.m_WindowKeyCallbackIndex);
        observer.remove<WindowCharEvent>(other.m_WindowCharCallbackIndex);
        observer.remove<WindowMouseButtonEvent>(other.m_WindowMouseButtonCallbackIndex);
        observer.remove<WindowMouseScrollEvent>(other.m_WindowMouseScrollCallbackIndex);
        m_WindowCloseCallbackIndex = observer.addCallback<WindowCloseEvent>(&Window::closeCallback, this);
        m_WindowResizeCallbackIndex = observer.addCallback<WindowResizeEvent>(&Window::resizeCallback, this);
        m_WindowMoveCallbackIndex = observer.addCallback<WindowMoveEvent>(&Window::moveCallback, this);
        m_WindowContentScaleCallbackIndex = observer.addCallback<WindowContentScaleEvent>(&Window::contentScaleCallback, this);
        m_WindowMouseMoveCallbackIndex = observer.addCallback<WindowMouseMoveEvent>(&Window::mouseMoveCallback, this);
        m_WindowMaximizeCallbackIndex = observer.addCallback<WindowMaximizeEvent>(&Window::maximizeCallback, this);
        m_WindowMinimizeCallbackIndex = observer.addCallback<WindowMinimizeEvent>(&Window::minimizeCallback, this);
        m_WindowGainFocusCallbackIndex = observer.addCallback<WindowGainFocusEvent>(&Window::gainFocusCallback, this);
        m_WindowLoseFocusCallbackIndex = observer.addCallback<WindowLoseFocusEvent>(&Window::loseFocusCallback, this);
        m_WindowKeyCallbackIndex = observer.addCallback<WindowKeyEvent>(&Window::keyCallback, this);
        m_WindowCharCallbackIndex = observer.addCallback<WindowCharEvent>(&Window::charCallback, this);
        m_WindowMouseButtonCallbackIndex = observer.addCallback<WindowMouseButtonEvent>(&Window::mouseButtonCallback, this);
        m_WindowMouseScrollCallbackIndex = observer.addCallback<WindowMouseScrollEvent>(&Window::mouseScrollCallback, this);
#endif
        }

        return *this;
    }

    Window Window::New(const Settings &settings) {
        return Window{settings};
    }

    void Window::setTitle(const std::string &title) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwSetWindowTitle(m_Window, title.c_str());
    }

    void Window::setSize(std::uint32_t width, std::uint32_t height) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwSetWindowSize(m_Window, width, height);
    }

    void Window::setWidth(std::uint32_t width) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        std::int32_t height;
        glfwGetWindowSize(m_Window, nullptr, &height);
        glfwSetWindowSize(m_Window, width, height);
    }

    void Window::setHeight(std::uint32_t height) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        std::int32_t width;
        glfwGetWindowSize(m_Window, &width, nullptr);
        glfwSetWindowSize(m_Window, width, height);
    }

    void Window::setPosition(std::uint32_t x, std::uint32_t y) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwSetWindowPos(m_Window, x, y);
    }

    void Window::setX(std::uint32_t x) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        std::int32_t y;
        glfwGetWindowPos(m_Window, nullptr, &y);
        glfwSetWindowPos(m_Window, x, y);
    }

    void Window::setY(std::uint32_t y) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        std::int32_t x;
        glfwGetWindowPos(m_Window, &x, nullptr);
        glfwSetWindowPos(m_Window, x, y);
    }

    void Window::setMousePosition(double x, double y) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwSetCursorPos(m_Window, x, y);
    }

    void Window::setMouseX(double x) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        double y;
        glfwGetCursorPos(m_Window, nullptr, &y);
        glfwSetCursorPos(m_Window, x, y);
    }

    void Window::setMouseY(double y) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        double x;
        glfwGetCursorPos(m_Window, &x, nullptr);
        glfwSetCursorPos(m_Window, x, y);
    }

    void Window::setCursorMode(CursorMode mode) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwSetInputMode(m_Window, GLFW_CURSOR, static_cast<std::int32_t>(mode));
    }

    void Window::setResizable(bool resizable) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwSetWindowAttrib(m_Window, GLFW_RESIZABLE, resizable ? GLFW_TRUE : GLFW_FALSE);
    }

    void Window::setRunning(bool running) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwSetWindowShouldClose(m_Window, running ? GLFW_FALSE : GLFW_TRUE);
    }

    void Window::makeFullscreen(bool monitorSize) {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
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
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwSetWindowMonitor(m_Window, nullptr, m_PrevX, m_PrevY, m_PrevWidth, m_PrevHeight, 0);
    }

    void Window::maximize() {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwMaximizeWindow(m_Window);
    }

    void Window::minimize() {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwIconifyWindow(m_Window);
    }

    void Window::restore() {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwRestoreWindow(m_Window);
    }

    void Window::close() {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        glfwSetWindowShouldClose(m_Window, GLFW_TRUE);
    }

    GLFWwindow *Window::get() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return m_Window;
    }

    std::string Window::getTitle() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return glfwGetWindowTitle(m_Window);
    }

    std::array<std::uint32_t, 2> Window::getSize() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        std::int32_t width, height;
        glfwGetWindowSize(m_Window, &width, &height);
        return {static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)};
    }

    std::uint32_t Window::getWidth() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        std::int32_t height;
        glfwGetWindowSize(m_Window, nullptr, &height);
        return height;
    }

    std::uint32_t Window::getHeight() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        std::int32_t width;
        glfwGetWindowSize(m_Window, &width, nullptr);
        return width;
    }

    std::array<std::uint32_t, 2> Window::getPosition() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        std::int32_t x, y;
        glfwGetWindowPos(m_Window, &x, &y);
        return {static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y)};
    }

    std::uint32_t Window::getX() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        std::int32_t y;
        glfwGetWindowPos(m_Window, nullptr, &y);
        return y;
    }

    std::uint32_t Window::getY() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        std::int32_t x;
        glfwGetWindowPos(m_Window, &x, nullptr);
        return x;
    }

    std::array<double, 2> Window::getMousePosition() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        double x, y;
        glfwGetCursorPos(m_Window, &x, &y);
        return {x, y};
    }

    double Window::getMouseX() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        double y;
        glfwGetCursorPos(m_Window, nullptr, &y);
        return y;
    }

    double Window::getMouseY() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        double x;
        glfwGetCursorPos(m_Window, &x, nullptr);
        return x;
    }

    CursorMode Window::getCursorMode() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return static_cast<CursorMode>(glfwGetInputMode(m_Window, GLFW_CURSOR));
    }

    bool Window::isClosed() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return glfwWindowShouldClose(m_Window) == GLFW_TRUE;
    }

    bool Window::isRunning() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return glfwWindowShouldClose(m_Window) == GLFW_FALSE;
    }

    bool Window::isMaximized() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return glfwGetWindowAttrib(m_Window, GLFW_MAXIMIZED) == GLFW_TRUE;
    }

    bool Window::isMinimized() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return glfwGetWindowAttrib(m_Window, GLFW_ICONIFIED) == GLFW_TRUE;
    }

    bool Window::isFocused() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return glfwGetWindowAttrib(m_Window, GLFW_FOCUSED) == GLFW_TRUE;
    }

    bool Window::isFullscreen() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return glfwGetWindowMonitor(m_Window) != nullptr;
    }

    bool Window::isResizable() const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return glfwGetWindowAttrib(m_Window, GLFW_RESIZABLE) == GLFW_TRUE;
    }

    bool Window::isKeyModPressed(KeyMod mod) const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return static_cast<std::int32_t>(m_KeyMods & mod) > 0;
    }

    bool Window::isKeyDown(KeyCode key) const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return m_Keys[static_cast<std::size_t>(key)];
    }

    bool Window::isKeyUp(KeyCode key) const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return !m_Keys[static_cast<std::size_t>(key)];
    }

    bool Window::isMouseButtonDown(MouseButton button) const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return m_MouseButtons[static_cast<std::size_t>(button)];
    }

    bool Window::isMouseButtonUp(MouseButton button) const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
        return !m_MouseButtons[static_cast<std::size_t>(button)];
    }

    WGPUSurface Window::getWGPUSurface(WGPUInstance instance) const {
        assert(m_Window != nullptr && "GLFWwindow cannot be nullptr");
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
            default:
                return "Unknown";
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
