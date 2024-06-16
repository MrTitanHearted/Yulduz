#include <Yulduz/Window.hpp>

namespace Yulduz {
    const char *getKeyName(KeyCode key);

    void Window::closeCallback(const WindowCloseEvent &event) {
        YZINFO("Closing Window: '{}'", getTitle());
    }

    void Window::resizeCallback(const WindowResizeEvent &event) {
        YZINFO("Resizing Window: '{}' to {}x{}", getTitle(), event.width, event.height);
    }

    void Window::moveCallback(const WindowMoveEvent &event) {
        YZINFO("Moving Window: '{}' to {}x{}", getTitle(), event.x, event.y);
    }

    void Window::contentScaleCallback(const WindowContentScaleEvent &event) {
        YZINFO("Scaling Window: '{}' to {}x{}", getTitle(), event.xScale, event.yScale);
    }

    void Window::mouseMoveCallback(const WindowMouseMoveEvent &event) {
        YZINFO("Moving Mouse on Window: '{}' to {}x{}", getTitle(), event.x, event.y);
    }

    void Window::maximizeCallback(const WindowMaximizeEvent &event) {
        YZINFO("Maximized Window: '{}'", getTitle());
    }

    void Window::minimizeCallback(const WindowMinimizeEvent &event) {
        YZINFO("Minimized Window: '{}'", getTitle());
    }

    void Window::gainFocusCallback(const WindowGainFocusEvent &event) {
        YZINFO("Gaining Focus on Window: '{}'", getTitle());
    }

    void Window::loseFocusCallback(const WindowLoseFocusEvent &event) {
        YZINFO("Losing Focus on Window: '{}'", getTitle());
    }

    void Window::keyCallback(const WindowKeyEvent &event) {
        const char *keyName = getKeyName(event.key);
        if (event.action == KeyAction::PRESS) {
            YZINFO("Pressing Key '{}' on Window: '{}'", keyName, getTitle());
        } else if (event.action == KeyAction::RELEASE) {
            YZINFO("Releasing Key '{}' on Window: '{}'", keyName, getTitle());
        } else {
            YZINFO("Repeatedly Pressing Key '{}' on Window: '{}'", keyName, getTitle());
        }
    }

    void Window::charCallback(const WindowCharEvent &event) {
        YZINFO("Pressing Character '{}' on Window: '{}'", static_cast<char>(event.codepoint), getTitle());
    }

    void Window::mouseButtonCallback(const WindowMouseButtonEvent &event) {
        const std::string &buttonName = event.button == MouseButton::LEFT
                                            ? "Left Mouse Button"
                                        : event.button == MouseButton::RIGHT
                                            ? "Right Mouse Button"
                                        : event.button == MouseButton::MIDDLE
                                            ? "Middle Mouse Button"
                                            : std::vformat("Mouse Button {}", std::make_format_args(static_cast<int>(event.button) + 1));
        if (event.action == KeyAction::PRESS) {
            YZINFO("Pressing Mouse Button '{}' on Window: '{}'", buttonName, getTitle());
        } else if (event.action == KeyAction::RELEASE) {
            YZINFO("Releasing Mouse Button '{}' on Window: '{}'", buttonName, getTitle());
        } else {
            YZINFO("Repeatedly Pressing Mouse Button '{}' on Window: '{}'", buttonName, getTitle());
        }
    }

    void Window::mouseScrollCallback(const WindowMouseScrollEvent &event) {
        YZINFO("Scrolling Mouse on Window: '{}' by {}x{}", getTitle(), event.xOffset, event.yOffset);
    }

    void Window::GlfwWindowCloseCallback(GLFWwindow *window) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        if (!yz->m_Dispatcher) return;
        yz->m_Dispatcher.value().get().addEvent<WindowCloseEvent>(WindowCloseEvent(yz));
    }

    void Window::GlfwWindowSizeCallback(GLFWwindow *window, int width, int height) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        if (!yz->m_Dispatcher) return;
        yz->m_Dispatcher.value().get().addEvent<WindowResizeEvent>(WindowResizeEvent(yz, width, height));
    }

    void Window::GlfwWindowContentScaleCallback(GLFWwindow *window, float xScale, float yScale) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        if (!yz->m_Dispatcher) return;
        yz->m_Dispatcher.value().get().addEvent<WindowContentScaleEvent>(WindowContentScaleEvent(yz, xScale, yScale));
    }

    void Window::GlfwWindowPositionCallback(GLFWwindow *window, int x, int y) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        if (!yz->m_Dispatcher) return;
        yz->m_Dispatcher.value().get().addEvent<WindowMoveEvent>(WindowMoveEvent(yz, x, y));
    }

    void Window::GlfwWindowMaximizeCallback(GLFWwindow *window, int isMaximized) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        if (!yz->m_Dispatcher) return;
        if (isMaximized == GLFW_TRUE)
            yz->m_Dispatcher.value().get().addEvent<WindowMaximizeEvent>(WindowMaximizeEvent(yz));
        else
            yz->m_Dispatcher.value().get().addEvent<WindowRestoreEvent>(WindowRestoreEvent(yz));
    }

    void Window::GlfwWindowMinimizeCallback(GLFWwindow *window, int isMinimized) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        if (!yz->m_Dispatcher) return;
        if (isMinimized == GLFW_TRUE)
            yz->m_Dispatcher.value().get().addEvent<WindowMinimizeEvent>(WindowMinimizeEvent(yz));
        else
            yz->m_Dispatcher.value().get().addEvent<WindowRestoreEvent>(WindowRestoreEvent(yz));
    }

    void Window::GlfwWindowFocusCallback(GLFWwindow *window, int focused) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        if (!yz->m_Dispatcher) return;
        if (focused == GLFW_TRUE)
            yz->m_Dispatcher.value().get().addEvent<WindowGainFocusEvent>(WindowGainFocusEvent(yz));
        else
            yz->m_Dispatcher.value().get().addEvent<WindowLoseFocusEvent>(WindowLoseFocusEvent(yz));
    }

    void Window::GlfwWindowKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        yz->m_KeyMods = static_cast<KeyMod>(mods);

        if (action == GLFW_PRESS)
            yz->m_Keys[key] = true;
        else if (action == GLFW_RELEASE)
            yz->m_Keys[key] = false;

        if (!yz->m_Dispatcher) return;
        yz->m_Dispatcher.value().get().addEvent<WindowKeyEvent>(WindowKeyEvent(yz, static_cast<KeyCode>(key), static_cast<KeyMod>(mods), static_cast<KeyAction>(action)));
    }

    void Window::GlfwWindowCharCallback(GLFWwindow *window, unsigned int codepoint) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        if (!yz->m_Dispatcher) return;
        yz->m_Dispatcher.value().get().addEvent<WindowCharEvent>(WindowCharEvent(yz, codepoint));
    }

    void Window::GlfwWindowMousePositionCallback(GLFWwindow *window, double x, double y) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        if (!yz->m_Dispatcher) return;
        yz->m_Dispatcher.value().get().addEvent<WindowMouseMoveEvent>(WindowMouseMoveEvent(yz, x, y));
    }

    void Window::GlfwWindowMouseEnterCallback(GLFWwindow *window, int entered) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        if (!yz->m_Dispatcher) return;
        if (entered == GLFW_TRUE)
            yz->m_Dispatcher.value().get().addEvent<WindowMouseEnterEvent>(WindowMouseEnterEvent(yz));
        else
            yz->m_Dispatcher.value().get().addEvent<WindowMouseLeaveEvent>(WindowMouseLeaveEvent(yz));
    }

    void Window::GlfwWindowMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        yz->m_KeyMods = static_cast<KeyMod>(mods);

        if (action == GLFW_PRESS)
            yz->m_MouseButtons[button] = true;
        if (action == GLFW_RELEASE)
            yz->m_MouseButtons[button] = false;

        if (!yz->m_Dispatcher) return;
        yz->m_Dispatcher.value().get().addEvent<WindowMouseButtonEvent>(WindowMouseButtonEvent(yz, static_cast<MouseButton>(button), static_cast<KeyMod>(mods), static_cast<KeyAction>(action)));
    }

    void Window::GlfwWindowMouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (yz == nullptr) return;

        if (yz->m_Window != window) return;

        if (!yz->m_Dispatcher) return;
        yz->m_Dispatcher.value().get().addEvent<WindowMouseScrollEvent>(WindowMouseScrollEvent(yz, xOffset, yOffset));
    }

    const char *getKeyName(KeyCode key) {
        const char *keyName = glfwGetKeyName(static_cast<int>(key), 0);
        if (keyName == nullptr) {
            switch (key) {
                case KeyCode::_DELETE:
                    keyName = "Delete";
                    break;
                case KeyCode::LEFT_ALT:
                    keyName = "Left Alt";
                    break;
                case KeyCode::RIGHT_ALT:
                    keyName = "Rigth Alt";
                    break;
                case KeyCode::LEFT_CONTROL:
                    keyName = "Left Ctrl";
                    break;
                case KeyCode::RIGHT_CONTROL:
                    keyName = "Right Ctrl";
                    break;
                case KeyCode::LEFT_SHIFT:
                    keyName = "Left Shift";
                    break;
                case KeyCode::RIGHT_SHIFT:
                    keyName = "Right Shift";
                    break;
                case KeyCode::LEFT_SUPER:
                    keyName = "Left Super";
                    break;
                case KeyCode::RIGHT_SUPER:
                    keyName = "Right Super";
                    break;
                case KeyCode::CAPS_LOCK:
                    keyName = "Caps Lock";
                    break;
                case KeyCode::TAB:
                    keyName = "Tab";
                    break;
                case KeyCode::BACKSPACE:
                    keyName = "Backspace";
                    break;
                case KeyCode::LEFT:
                    keyName = "Left Arrow";
                    break;
                case KeyCode::DOWN:
                    keyName = "Down Arrow";
                    break;
                case KeyCode::UP:
                    keyName = "Up Arrow";
                    break;
                case KeyCode::RIGHT:
                    keyName = "Right Arrow";
                    break;
                case KeyCode::NUM_LOCK:
                    keyName = "Num Lock";
                    break;
                case KeyCode::F1:
                    keyName = "F1";
                    break;
                case KeyCode::F2:
                    keyName = "F2";
                    break;
                case KeyCode::F3:
                    keyName = "F3";
                    break;
                case KeyCode::F4:
                    keyName = "F4";
                    break;
                case KeyCode::F5:
                    keyName = "F5";
                    break;
                case KeyCode::F6:
                    keyName = "F6";
                    break;
                case KeyCode::F7:
                    keyName = "F7";
                    break;
                case KeyCode::F8:
                    keyName = "F8";
                    break;
                case KeyCode::F9:
                    keyName = "F9";
                    break;
                case KeyCode::F10:
                    keyName = "F10";
                    break;
                case KeyCode::F11:
                    keyName = "F11";
                    break;
                case KeyCode::F12:
                    keyName = "F12";
                    break;
                case KeyCode::PRINT_SCREEN:
                    keyName = "Print Screen";
                    break;
                case KeyCode::SCROLL_LOCK:
                    keyName = "Scroll Lock";
                    break;
                case KeyCode::PAUSE:
                    keyName = "Pause";
                    break;
                case KeyCode::HOME:
                    keyName = "Home";
                    break;
                case KeyCode::END:
                    keyName = "End";
                    break;
                case KeyCode::PAGE_UP:
                    keyName = "Page Up";
                    break;
                case KeyCode::PAGE_DOWN:
                    keyName = "Page Down";
                    break;
                case KeyCode::SPACE:
                    keyName = "Space";
                    break;
                case KeyCode::ENTER:
                case KeyCode::KP_ENTER:
                    keyName = "Enter";
                    break;
                default:
                    keyName = "Unknown Key";
            }
        }

        return keyName;
    }
}  // namespace Yulduz
