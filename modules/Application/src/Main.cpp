#include <Yulduz/Yulduz.hpp>

int YULDUZ_MAIN() {
    Yulduz::Logger::Initialize(Yulduz::LogLevel::Debug);
    Yulduz::Window::InitializeGlfw();

    try {
        Yulduz::App::Run();
    } catch (const std::runtime_error &e) {
        YZFATAL("Runtime Error: '{}'", e.what());
    } catch (const std::exception &e) {
        YZFATAL("Exception: '{}'", e.what());
    } catch (...) {
        YZFATAL("Unknown Exception");
    }

    Yulduz::Window::ShutdownGlfw();
    Yulduz::Logger::Shutdown();

#if defined(YULDUZ_BUILD_TYPE_DEBUG)
    LOG_INFO("Press Enter to exit...");
    std::cin.get();
#endif

    return 0;
}