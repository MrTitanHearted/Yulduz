#include <Yulduz/Yulduz.hpp>

#include <Yulduz/Core.hpp>
#include <Yulduz/Assets.hpp>
#include <Yulduz/Window.hpp>
#include <Yulduz/Render.hpp>
#include <Yulduz/Engine.hpp>

namespace Yulduz {
    class MyEvent : public IEvent {
       public:
        // MyEvent(const std::string &msg) : message(msg) {}
        MyEvent(const std::string &msg) {
            message = msg;
        }

        std::string message;
    };

    class YourEvent : public IEvent {
       public:
        YourEvent(float x, float y) {
            this->x = x;
            this->y = y;
        }

        float x, y;
    };

    void printMyEvent(const MyEvent &event) {
        YZDEBUG("My Event: '{}'", event.message);
    }

    void logYourEvent(const YourEvent &event) {
        YZFATAL("Your Event: x={}, y={}", event.x, event.y);
    }

    void TestYulduz() {
        Logger::Initialize(LogLevel::Debug);

        EventDispatcher eventDispatcher{};

        eventDispatcher.setCallback<MyEvent>(printMyEvent);

        eventDispatcher.addCallback<MyEvent>([](const MyEvent &event) {
            YZWARN("Second Callback for My Event: '{}'", event.message);
        });

        eventDispatcher.addCallback<YourEvent>(logYourEvent);

        eventDispatcher.addEvent(MyEvent("Hello, World!"));
        eventDispatcher.addEvent(MyEvent("Hooray"));
        eventDispatcher.addEvent(YourEvent(34.5, 54.1));
        eventDispatcher.addEvent(YourEvent(342432.5, -32454.1));

        eventDispatcher.dispatch();

        eventDispatcher.removeFirstCallback<MyEvent>();
        eventDispatcher.removeLastCallback<YourEvent>();

        eventDispatcher.addEvent(MyEvent("Hello, World!"));
        eventDispatcher.addEvent(MyEvent("Hooray"));
        eventDispatcher.addEvent(YourEvent(34.5, 54.1));
        eventDispatcher.addEvent(YourEvent(342432.5, -32454.1));

        eventDispatcher.dispatch();

        Logger::Shutdown();
    }
}  // namespace Yulduz
