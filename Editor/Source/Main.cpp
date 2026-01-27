#include <Yulduz/Editor.hpp>

glm::i32 main(const glm::i32 argc, char **argv) {
    (void)argc;
    (void)argv;

    Yulduz::Logger::Settings logger_settings{};
    logger_settings.LoggerName = "EDITOR";
#if defined(YULDUZ_BUILD_TYPE_DEBUG) || defined(YULDUZ_BUILD_TYPE_RELWITHDEBINFO)
    logger_settings.EngineLogLevel = spdlog::level::trace;
    logger_settings.LogLevel       = spdlog::level::trace;
#else
    logger_settings.EngineLogLevel = spdlog::level::err;
    logger_settings.LogLevel       = spdlog::level::warn;
#endif

    const spdlog::sink_ptr console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_pattern(Yulduz::Logger::GetConsoleSinkPattern().data());

    logger_settings.Sinks.push_back(console_sink);
    const spdlog::sink_ptr editor_file_sink =
        logger_settings.Sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            "YULDUZ_EDITOR.log", true));
    editor_file_sink->set_pattern(Yulduz::Logger::GetFileSinkPattern().data());

    logger_settings.EngineSinks.push_back(console_sink);
    const spdlog::sink_ptr engine_file_sink =
        logger_settings.EngineSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>(
            "YULDUZ_ENGINE.log", true));
    engine_file_sink->set_pattern(Yulduz::Logger::GetFileSinkPattern().data());

    Yulduz::Engine::Settings engine_settings{};
    engine_settings.WindowSettings.Title = "Yulduz::Editor";
    engine_settings.WindowSettings.Icon  = "Assets/Icons/BlueYulduz.png";

    Yulduz::Logger logger{};
    Yulduz::Engine engine{};
    logger.initialize(logger_settings);
    engine.initialize(engine_settings);

    engine.pushLayer<Yulduz::EditorLayer>();

    engine.run();

    engine.release();
    logger.release();

    return 0;
}