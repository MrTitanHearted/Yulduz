#pragma once

#include <Yulduz/Core.hpp>
#include <YulduzWindow/Enums.hpp>

namespace Yulduz {
    class Window;

    class WindowEvent : public IEvent {
       public:
        Window *window;
    };

    class WindowCloseEvent : public WindowEvent {
       public:
        WindowCloseEvent(Window *window) {
            this->window = window;
        }
    };

    class WindowResizeEvent : public WindowEvent {
       public:
        uint32_t width;
        uint32_t height;

        WindowResizeEvent(Window *window, uint32_t width, uint32_t height)
            : width(width), height(height) {
            this->window = window;
        }
    };

    class WindowContentScaleEvent : public WindowEvent {
       public:
        float xScale;
        float yScale;

        WindowContentScaleEvent(Window *window, float xScale, float yScale)
            : xScale(xScale), yScale(yScale) {
            this->window = window;
        }
    };

    class WindowMoveEvent : public WindowEvent {
       public:
        uint32_t x;
        uint32_t y;

        WindowMoveEvent(Window *window, uint32_t x, uint32_t y) : x(x), y(y) {
            this->window = window;
        }
    };

    class WindowRestoreEvent : public WindowEvent {
       public:
        WindowRestoreEvent(Window *window) {
            this->window = window;
        }
    };

    class WindowMinimizeEvent : public WindowEvent {
       public:
        WindowMinimizeEvent(Window *window) {
            this->window = window;
        }
    };

    class WindowMaximizeEvent : public WindowEvent {
       public:
        WindowMaximizeEvent(Window *window) {
            this->window = window;
        }
    };

    class WindowGainFocusEvent : public WindowEvent {
       public:
        WindowGainFocusEvent(Window *window) {
            this->window = window;
        }
    };

    class WindowLoseFocusEvent : public WindowEvent {
       public:
        WindowLoseFocusEvent(Window *window) {
            this->window = window;
        }
    };

    class WindowKeyEvent : public WindowEvent {
       public:
        KeyCode key;
        KeyMod mods;
        KeyAction action;

        WindowKeyEvent(Window *window, KeyCode key, KeyMod mods, KeyAction action)
            : key(key), mods(mods), action(action) {
            this->window = window;
        }
    };

    class WindowCharEvent : public WindowEvent {
       public:
        uint32_t codepoint;

        WindowCharEvent(Window *window, uint32_t codepoint) : codepoint(codepoint) {
            this->window = window;
        }
    };

    class WindowMouseMoveEvent : public WindowEvent {
       public:
        double x;
        double y;

        WindowMouseMoveEvent(Window *window, double x, double y) : x(x), y(y) {
            this->window = window;
        }
    };

    class WindowMouseEnterEvent : public WindowEvent {
       public:
        WindowMouseEnterEvent(Window *window) {
            this->window = window;
        }
    };

    class WindowMouseLeaveEvent : public WindowEvent {
       public:
        WindowMouseLeaveEvent(Window *window) {
            this->window = window;
        }
    };

    class WindowMouseButtonEvent : public WindowEvent {
       public:
        MouseButton button;
        KeyMod mods;
        KeyAction action;

        WindowMouseButtonEvent(Window *window, MouseButton button, KeyMod mods, KeyAction action)
            : button(button), mods(mods), action(action) {
            this->window = window;
        }
    };

    class WindowMouseScrollEvent : public WindowEvent {
       public:
        double xOffset;
        double yOffset;

        WindowMouseScrollEvent(Window *window, double x, double y) : xOffset(x), yOffset(y) {
            this->window = window;
        }
    };
}  // namespace Yulduz
