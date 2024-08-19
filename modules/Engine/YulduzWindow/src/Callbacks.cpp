#include <Yulduz/Window.hpp>

namespace Yulduz {
#if defined(YULDUZ_BUILD_TYPE_DEBUG)
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
        const std::string &keyName = GetKeyCodeName(event.key);
        if (event.action == KeyAction::Press) {
            YZINFO("Pressing Key '{}' on Window: '{}'", keyName, getTitle());
        } else if (event.action == KeyAction::Release) {
            YZINFO("Releasing Key '{}' on Window: '{}'", keyName, getTitle());
        } else {
            YZINFO("Repeatedly Pressing Key '{}' on Window: '{}'", keyName, getTitle());
        }
    }

    void Window::charCallback(const WindowCharEvent &event) {
        YZINFO("Pressing Character '{}' on Window: '{}'", static_cast<char>(event.codepoint), getTitle());
    }

    void Window::mouseButtonCallback(const WindowMouseButtonEvent &event) {
        const std::string &buttonName = GetMouseButtonName(event.button);
        if (event.action == KeyAction::Press) {
            YZINFO("Pressing Mouse Button '{}' on Window: '{}'", buttonName, getTitle());
        } else if (event.action == KeyAction::Release) {
            YZINFO("Releasing Mouse Button '{}' on Window: '{}'", buttonName, getTitle());
        } else {
            YZINFO("Repeatedly Pressing Mouse Button '{}' on Window: '{}'", buttonName, getTitle());
        }
    }

    void Window::mouseScrollCallback(const WindowMouseScrollEvent &event) {
        YZINFO("Scrolling Mouse on Window: '{}' by {}x{}", getTitle(), event.xOffset, event.yOffset);
    }
#endif
    void Window::GlfwWindowCloseCallback(GLFWwindow *window) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        g_EventObserver->process<WindowCloseEvent>(WindowCloseEvent{yz});
    }

    void Window::GlfwWindowSizeCallback(GLFWwindow *window, std::int32_t width, std::int32_t height) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        g_EventObserver->process<WindowResizeEvent>(WindowResizeEvent{yz, static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)});
    }

    void Window::GlfwWindowContentScaleCallback(GLFWwindow *window, float xScale, float yScale) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        g_EventObserver->process<WindowContentScaleEvent>(WindowContentScaleEvent{yz, xScale, yScale});
    }

    void Window::GlfwWindowPositionCallback(GLFWwindow *window, std::int32_t x, std::int32_t y) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        g_EventObserver->process<WindowMoveEvent>(WindowMoveEvent{yz, static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y)});
    }

    void Window::GlfwWindowMaximizeCallback(GLFWwindow *window, std::int32_t isMaximized) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (isMaximized == GLFW_TRUE)
            g_EventObserver->process<WindowMaximizeEvent>(WindowMaximizeEvent{yz});
        else
            g_EventObserver->process<WindowRestoreEvent>(WindowRestoreEvent{yz});
    }

    void Window::GlfwWindowMinimizeCallback(GLFWwindow *window, std::int32_t isMinimized) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (isMinimized == GLFW_TRUE)
            g_EventObserver->process<WindowMinimizeEvent>(WindowMinimizeEvent{yz});
        else
            g_EventObserver->process<WindowRestoreEvent>(WindowRestoreEvent{yz});
    }

    void Window::GlfwWindowFocusCallback(GLFWwindow *window, std::int32_t focused) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (focused == GLFW_TRUE)
            g_EventObserver->process<WindowGainFocusEvent>(WindowGainFocusEvent{yz});
        else
            g_EventObserver->process<WindowLoseFocusEvent>(WindowLoseFocusEvent{yz});
    }

    void Window::GlfwWindowKeyCallback(GLFWwindow *window, std::int32_t key, std::int32_t scancode, std::int32_t action, std::int32_t mods) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        yz->m_KeyMods = static_cast<KeyMod>(mods);

        if (action == GLFW_PRESS)
            yz->m_Keys[key] = true;
        else if (action == GLFW_RELEASE)
            yz->m_Keys[key] = false;
        g_EventObserver->process<WindowKeyEvent>(WindowKeyEvent{yz, static_cast<KeyCode>(key), static_cast<KeyMod>(mods), static_cast<KeyAction>(action)});
    }

    void Window::GlfwWindowCharCallback(GLFWwindow *window, std::uint32_t codepoint) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        g_EventObserver->process<WindowCharEvent>(WindowCharEvent{yz, codepoint});
    }

    void Window::GlfwWindowMousePositionCallback(GLFWwindow *window, double x, double y) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        g_EventObserver->process<WindowMouseMoveEvent>(WindowMouseMoveEvent{yz, x, y});
    }

    void Window::GlfwWindowMouseEnterCallback(GLFWwindow *window, std::int32_t entered) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        if (entered == GLFW_TRUE)
            g_EventObserver->process<WindowMouseEnterEvent>(WindowMouseEnterEvent{yz});
        else
            g_EventObserver->process<WindowMouseLeaveEvent>(WindowMouseLeaveEvent{yz});
    }

    void Window::GlfwWindowMouseButtonCallback(GLFWwindow *window, std::int32_t button, std::int32_t action, std::int32_t mods) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        yz->m_KeyMods = static_cast<KeyMod>(mods);

        if (action == GLFW_PRESS)
            yz->m_MouseButtons[button] = true;
        if (action == GLFW_RELEASE)
            yz->m_MouseButtons[button] = false;

        g_EventObserver->process<WindowMouseButtonEvent>(WindowMouseButtonEvent{yz, static_cast<MouseButton>(button), static_cast<KeyMod>(mods), static_cast<KeyAction>(action)});
    }

    void Window::GlfwWindowMouseScrollCallback(GLFWwindow *window, double xOffset, double yOffset) {
        Window *yz = static_cast<Window *>(glfwGetWindowUserPointer(window));
        g_EventObserver->process<WindowMouseScrollEvent>(WindowMouseScrollEvent{yz, xOffset, yOffset});
    }

}  // namespace Yulduz