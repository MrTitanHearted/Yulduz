#include <Yulduz/Window.hpp>

namespace Yulduz {
    const char *getKeyName(KeyCode key);

    void Window::closeCallback(const WindowCloseEvent &event) {
        YZDEBUG("Closing Window: '{}'", getTitle());
    }

    void Window::resizeCallback(const WindowResizeEvent &event) {
        YZDEBUG("Resizing Window: '{}' to {}x{}", getTitle(), event.width, event.height);
    }

    void Window::moveCallback(const WindowMoveEvent &event) {
        YZDEBUG("Moving Window: '{}' to {}x{}", getTitle(), event.x, event.y);
    }

    void Window::contentScaleCallback(const WindowContentScaleEvent &event) {
        YZDEBUG("Scaling Window: '{}' to {}x{}", getTitle(), event.xScale, event.yScale);
    }

    void Window::mouseMoveCallback(const WindowMouseMoveEvent &event) {
        YZDEBUG("Moving Mouse on Window: '{}' to {}x{}", getTitle(), event.x, event.y);
    }

    void Window::minimizeCallback(const WindowMinimizeEvent &event) {
        YZDEBUG("Minimizing Window: '{}'", getTitle());
    }

    void Window::maximizeCallback(const WindowMaximizeEvent &event) {
        YZDEBUG("Maximizing Window: '{}'", getTitle());
    }

    void Window::gainFocusCallback(const WindowGainFocusEvent &event) {
        YZDEBUG("Gaining Focus on Window: '{}'", getTitle());
    }

    void Window::loseFocusCallback(const WindowLoseFocusEvent &event) {
        YZDEBUG("Losing Focus on Window: '{}'", getTitle());
    }

    void Window::keyCallback(const WindowKeyEvent &event) {
        const char *keyName = getKeyName(event.key);
        if (event.action == Action::PRESS) {
            if (m_QuitOnEscape && event.key == KeyCode::KEY_ESCAPE && event.mods == KeyMod::NONE) {
                close();
            }
            YZDEBUG("Pressing Key '{}' on Window: '{}'", keyName, getTitle());
        } else if (event.action == Action::RELEASE) {
            YZDEBUG("Releasing Key '{}' on Window: '{}'", keyName, getTitle());
        } else {
            YZDEBUG("Repeating Key '{}' Press on Window: '{}'", keyName, getTitle());
        }
    }

    void Window::charCallback(const WindowCharEvent &event) {
        YZDEBUG("Pressing Character '{}' on Window: '{}'", static_cast<char>(event.codepoint), getTitle());
    }

    void Window::mouseButtonCallback(const WindowMouseButtonEvent &event) {
        const std::string buttonName = event.button == MouseButton::BUTTON_LEFT
                                           ? "Left Mouse Button"
                                       : event.button == MouseButton::BUTTON_RIGHT
                                           ? "Right Mouse Button"
                                       : event.button == MouseButton::BUTTON_MIDDLE
                                           ? "Middle Mouse Button"
                                           : std::vformat("Mouse Button {}", std::make_format_args(static_cast<int>(event.button) + 1));

        if (event.action == Action::PRESS) {
            YZDEBUG("Pressing Mouse Button '{}' on Window: '{}'", buttonName, getTitle());
        } else if (event.action == Action::RELEASE) {
            YZDEBUG("Releasing Mouse Button '{}' on Window: '{}'", buttonName, getTitle());
        } else {
            YZDEBUG("Repeating Mouse Button '{}' Press on Window: '{}'", buttonName, getTitle());
        }
    }

    void Window::mouseScrollCallback(const WindowMouseScrollEvent &event) {
        YZDEBUG("Scrolling Mouse on Window: '{}' by {}x{}", getTitle(), event.xOffset, event.yOffset);
    }

    void Window::GlfwWindowCloseCallback(GLFWwindow *window) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (yz->m_Dispatcher == nullptr) return;

        yz->m_Dispatcher->addEvent(WindowCloseEvent(yz));
    }

    void Window::GlfwWindowSizeCallback(GLFWwindow *window, int width, int height) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (yz->m_Dispatcher == nullptr) return;

        yz->m_Dispatcher->addEvent(WindowResizeEvent(yz, width, height));
    }

    void Window::GlfwWindowContentScaleCallback(GLFWwindow *window, float xScale, float yScale) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (yz->m_Dispatcher == nullptr) return;

        yz->m_Dispatcher->addEvent(WindowContentScaleEvent(yz, xScale, yScale));
    }

    void Window::GlfwWindowPositionCallback(GLFWwindow *window, int x, int y) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (yz->m_Dispatcher == nullptr) return;

        yz->m_Dispatcher->addEvent(WindowMoveEvent(yz, x, y));
    }

    void Window::GlfwWindowMinimizeCallback(GLFWwindow *window, int isMinimized) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (yz->m_Dispatcher == nullptr) return;

        if (isMinimized) {
            yz->m_Dispatcher->addEvent(WindowMinimizeEvent(yz));
        } else {
            yz->m_Dispatcher->addEvent(WindowRestoreEvent(yz));
        }
    }

    void Window::GlfwWindowMaximizeCallback(GLFWwindow *window, int isMaximized) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (yz->m_Dispatcher == nullptr) return;

        if (isMaximized) {
            yz->m_Dispatcher->addEvent(WindowMaximizeEvent(yz));
        } else {
            yz->m_Dispatcher->addEvent(WindowRestoreEvent(yz));
        }
    }

    void Window::GlfwWindowFocusCallback(GLFWwindow *window, int focused) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (yz->m_Dispatcher == nullptr) return;

        if (focused) {
            yz->m_Dispatcher->addEvent(WindowGainFocusEvent(yz));
        } else {
            yz->m_Dispatcher->addEvent(WindowLoseFocusEvent(yz));
        }
    }

    void Window::GlfwWindowKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        yz->m_KeyMods = static_cast<KeyMod>(mods);

        if (action == GLFW_PRESS) {
            yz->m_Keys[key] = true;
        } else if (action == GLFW_RELEASE) {
            yz->m_Keys[key] = false;
        }

        if (yz->m_Dispatcher == nullptr) return;

        KeyCode keyCode = static_cast<KeyCode>(key);
        Action keyAction = static_cast<Action>(action);
        KeyMod keyMods = static_cast<KeyMod>(mods);
        yz->m_Dispatcher->addEvent(WindowKeyEvent(yz, keyCode, keyMods, keyAction));
    }

    void Window::GlfwWindowCharCallback(GLFWwindow *window, unsigned int codepoint) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (yz->m_Dispatcher == nullptr) return;

        yz->m_Dispatcher->addEvent(WindowCharEvent(yz, codepoint));
    }

    void Window::GlfwWindowMousePositionCallback(GLFWwindow *window, double x, double y) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (yz->m_Dispatcher == nullptr) return;

        yz->m_Dispatcher->addEvent(WindowMouseMoveEvent(yz, x, y));
    }

    void Window::GlfwWindowMouseEnterCallback(GLFWwindow *window, int entered) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (yz->m_Dispatcher == nullptr) return;

        if (entered) {
            yz->m_Dispatcher->addEvent(WindowMouseEnterEvent(yz));
        } else {
            yz->m_Dispatcher->addEvent(WindowMouseLeaveEvent(yz));
        }
    }

    void Window::GlfwWindowMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        yz->m_KeyMods = static_cast<KeyMod>(mods);

        if (action == GLFW_PRESS) {
            yz->m_MouseButtons[button] = true;
        } else if (action == GLFW_RELEASE) {
            yz->m_MouseButtons[button] = false;
        }

        if (yz->m_Dispatcher == nullptr) return;

        MouseButton mouseButtonCode = static_cast<MouseButton>(button);
        Action mouseButtonAction = static_cast<Action>(action);
        KeyMod mouseButtonMods = static_cast<KeyMod>(mods);
        yz->m_Dispatcher->addEvent(WindowMouseButtonEvent(yz, mouseButtonCode, mouseButtonMods, mouseButtonAction));
    }

    void Window::GlfwWindowMouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
        if (glfwGetWindowUserPointer(window) == nullptr) return;

        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (yz->m_Dispatcher == nullptr) return;

        yz->m_Dispatcher->addEvent(WindowMouseScrollEvent(yz, xOffset, yOffset));
    }

    const char *getKeyName(KeyCode key) {
        const char *keyName = glfwGetKeyName(static_cast<int>(key), 0);
        if (keyName == nullptr) {
            switch (key) {
                case KeyCode::KEY_LEFT_ALT:
                    keyName = "Left Alt";
                    break;
                case KeyCode::KEY_RIGHT_ALT:
                    keyName = "Rigth Alt";
                    break;
                case KeyCode::KEY_LEFT_CONTROL:
                    keyName = "Left Ctrl";
                    break;
                case KeyCode::KEY_RIGHT_CONTROL:
                    keyName = "Right Ctrl";
                    break;
                case KeyCode::KEY_LEFT_SHIFT:
                    keyName = "Left Shift";
                    break;
                case KeyCode::KEY_RIGHT_SHIFT:
                    keyName = "Right Shift";
                    break;
                case KeyCode::KEY_LEFT_SUPER:
                    keyName = "Left Super";
                    break;
                case KeyCode::KEY_RIGHT_SUPER:
                    keyName = "Right Super";
                    break;
                case KeyCode::KEY_CAPS_LOCK:
                    keyName = "Caps Lock";
                    break;
                case KeyCode::KEY_TAB:
                    keyName = "Tab";
                    break;
                case KeyCode::KEY_BACKSPACE:
                    keyName = "Backspace";
                    break;
                case KeyCode::KEY_LEFT:
                    keyName = "Left Arrow";
                    break;
                case KeyCode::KEY_DOWN:
                    keyName = "Down Arrow";
                    break;
                case KeyCode::KEY_UP:
                    keyName = "Up Arrow";
                    break;
                case KeyCode::KEY_RIGHT:
                    keyName = "Right Arrow";
                    break;
                case KeyCode::KEY_NUM_LOCK:
                    keyName = "Num Lock";
                    break;
                case KeyCode::KEY_F1:
                    keyName = "F1";
                    break;
                case KeyCode::KEY_F2:
                    keyName = "F2";
                    break;
                case KeyCode::KEY_F3:
                    keyName = "F3";
                    break;
                case KeyCode::KEY_F4:
                    keyName = "F4";
                    break;
                case KeyCode::KEY_F5:
                    keyName = "F5";
                    break;
                case KeyCode::KEY_F6:
                    keyName = "F6";
                    break;
                case KeyCode::KEY_F7:
                    keyName = "F7";
                    break;
                case KeyCode::KEY_F8:
                    keyName = "F8";
                    break;
                case KeyCode::KEY_F9:
                    keyName = "F9";
                    break;
                case KeyCode::KEY_F10:
                    keyName = "F10";
                    break;
                case KeyCode::KEY_F11:
                    keyName = "F11";
                    break;
                case KeyCode::KEY_F12:
                    keyName = "F12";
                    break;
                case KeyCode::KEY_PRINT_SCREEN:
                    keyName = "Print Screen";
                    break;
                case KeyCode::KEY_SCROLL_LOCK:
                    keyName = "Scroll Lock";
                    break;
                case KeyCode::KEY_PAUSE:
                    keyName = "Pause";
                    break;
                case KeyCode::KEY_HOME:
                    keyName = "Home";
                    break;
                case KeyCode::KEY_END:
                    keyName = "End";
                    break;
                case KeyCode::KEY_PAGE_UP:
                    keyName = "Page Up";
                    break;
                case KeyCode::KEY_PAGE_DOWN:
                    keyName = "Page Down";
                    break;
                case KeyCode::KEY_SPACE:
                    keyName = "Space";
                    break;
                case KeyCode::KEY_ENTER:
                case KeyCode::KEY_KP_ENTER:
                    keyName = "Enter";
                    break;
                default:
                    keyName = "Unknown Key";
            }
        }

        return keyName;
    }
}  // namespace Yulduz
