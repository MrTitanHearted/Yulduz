#include <Yulduz/Yulduz.hpp>

#include <Yulduz/Core.hpp>
#include <Yulduz/Assets.hpp>
#include <Yulduz/Window.hpp>
#include <Yulduz/Render.hpp>
#include <Yulduz/Engine.hpp>

namespace Yulduz {
    void keyCallback(const WindowKeyEvent &event) {
        if (event.action != Action::PRESS) return;
        if (event.key != KeyCode::KEY_ENTER) return;
        YZINFO("HEEYYYYYYYYY");
    }
    
    void testYulduz() {
        EventDispatcher eventDispatcher{};
        std::shared_ptr<Window> window = Window::New(Window::Settings{
            .eventDispatcher = &eventDispatcher,
        });

        eventDispatcher.addCallback<WindowKeyEvent>(keyCallback);

        while (window->isRunning()) {
            Window::PollEvents();
            eventDispatcher.dispatch();
        }
    }

    void TestYulduz() {
        Logger::Initialize(LogLevel::Info);

        try {
            testYulduz();
        } catch (const std::exception &e) {
            YZFATAL("Program throwed Exception: '{}'", e.what());
        }

        Logger::Shutdown();
    }
}  // namespace Yulduz
