#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzWindow/Enums.hpp>

namespace Yulduz {
    class Window;

    class WindowEvent : public IEvent {
       public:
        const Window *window;
    };

    class WindowCloseEvent : public WindowEvent {
       public:
        WindowCloseEvent() = default;
        WindowCloseEvent(const Window *window) {
            this->window = window;
        }
    };

    class WindowResizeEvent : public WindowEvent {
       public:
        uint32_t width;
        uint32_t height;

        WindowResizeEvent() = default;
        WindowResizeEvent(const Window *window, uint32_t width, uint32_t height)
            : width(width), height(height) {
            this->window = window;
        }
    };

    class WindowContentScaleEvent : public WindowEvent {
       public:
        float xScale;
        float yScale;

        WindowContentScaleEvent() = default;
        WindowContentScaleEvent(const Window *window, float xScale, float yScale)
            : xScale(xScale), yScale(yScale) {
            this->window = window;
        }
    };

    class WindowMoveEvent : public WindowEvent {
       public:
        uint32_t x;
        uint32_t y;

        WindowMoveEvent() = default;
        WindowMoveEvent(const Window *window, uint32_t x, uint32_t y) : x(x), y(y) {
            this->window = window;
        }
    };

    class WindowRestoreEvent : public WindowEvent {
       public:
        WindowRestoreEvent() = default;
        WindowRestoreEvent(const Window *window) {
            this->window = window;
        }
    };

    class WindowMinimizeEvent : public WindowEvent {
       public:
        WindowMinimizeEvent() = default;
        WindowMinimizeEvent(const Window *window) {
            this->window = window;
        }
    };

    class WindowMaximizeEvent : public WindowEvent {
       public:
        WindowMaximizeEvent() = default;
        WindowMaximizeEvent(const Window *window) {
            this->window = window;
        }
    };

    class WindowGainFocusEvent : public WindowEvent {
       public:
        WindowGainFocusEvent() = default;
        WindowGainFocusEvent(const Window *window) {
            this->window = window;
        }
    };

    class WindowLoseFocusEvent : public WindowEvent {
       public:
        WindowLoseFocusEvent() = default;
        WindowLoseFocusEvent(const Window *window) {
            this->window = window;
        }
    };

    class WindowKeyEvent : public WindowEvent {
       public:
        KeyCode key;
        KeyMod mods;
        Action action;

        WindowKeyEvent() = default;
        WindowKeyEvent(const Window *window, KeyCode key, KeyMod mods, Action action)
            : key(key), mods(mods), action(action) {
            this->window = window;
        }
    };

    class WindowCharEvent : public WindowEvent {
       public:
        uint32_t codepoint;

        WindowCharEvent() = default;
        WindowCharEvent(const Window *window, uint32_t codepoint) : codepoint(codepoint) {
            this->window = window;
        }
    };

    class WindowMouseMoveEvent : public WindowEvent {
       public:
        double x;
        double y;

        WindowMouseMoveEvent() = default;
        WindowMouseMoveEvent(const Window *window, double x, double y) : x(x), y(y) {
            this->window = window;
        }
    };

    class WindowMouseEnterEvent : public WindowEvent {
       public:
        WindowMouseEnterEvent() = default;
        WindowMouseEnterEvent(const Window *window) {
            this->window = window;
        }
    };

    class WindowMouseLeaveEvent : public WindowEvent {
       public:
        WindowMouseLeaveEvent() = default;
        WindowMouseLeaveEvent(const Window *window) {
            this->window = window;
        }
    };

    class WindowMouseButtonEvent : public WindowEvent {
       public:
        MouseButton button;
        KeyMod mods;
        Action action;

        WindowMouseButtonEvent() = default;
        WindowMouseButtonEvent(const Window *window, MouseButton button, KeyMod mods, Action action)
            : button(button), mods(mods), action(action) {
            this->window = window;
        }
    };

    class WindowMouseScrollEvent : public WindowEvent {
       public:
        double xOffset;
        double yOffset;

        WindowMouseScrollEvent() = default;
        WindowMouseScrollEvent(const Window *window, double x, double y) : xOffset(x), yOffset(y) {
            this->window = window;
        }
    };
}  // namespace Yulduz
