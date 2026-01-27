#include <Yulduz/Common/Logger.hpp>

namespace Yulduz {
    Logger *Logger::s_pLogger = nullptr;

    YULDUZ_IF_DEBUG(Logger::State Logger::s_State{});

    Logger &Logger::GetRef() {
        assert(nullptr != s_pLogger && "Yulduz::Logger is not initialized");
        return *s_pLogger;
    }

    std::shared_ptr<spdlog::logger> Logger::GetLogger() {
        YULDUZ_IF_DEBUG(assert(nullptr != s_pLogger && "Yulduz::Logger is not initialized"););
        return s_pLogger->m_Logger;
    }

    std::shared_ptr<spdlog::logger> Logger::GetEngineLogger() {
        YULDUZ_IF_DEBUG(assert(nullptr != s_pLogger && "Yulduz::Logger is not initialized"););
        return s_pLogger->m_EngineLogger;
    }

    eastl::string_view Logger::GetFileSinkPattern() {
        return "[%Y-%m-%d %T]::[%@]::[%!]::[%l]: %v";
    }

    eastl::string_view Logger::GetConsoleSinkPattern() {
        return "[%n] [%^%l%$] [%s:%#] %v";
    }

    void Logger::initialize(const Settings &settings) {
        assert(nullptr == s_pLogger && "Yulduz::Logger already initialized");

        m_Logger       = std::make_shared<spdlog::logger>(settings.LoggerName.data(), spdlog::sinks_init_list{});
        m_EngineLogger = std::make_shared<spdlog::logger>("ENGINE", spdlog::sinks_init_list{});

        for (const auto &sink : settings.Sinks) {
            m_Logger->sinks().emplace_back(sink);
        }
        for (const auto &sink : settings.EngineSinks) {
            m_EngineLogger->sinks().emplace_back(sink);
        }

        m_Logger->set_level(settings.LogLevel);
        m_EngineLogger->set_level(settings.EngineLogLevel);

        spdlog::register_logger(m_Logger);
        spdlog::register_logger(m_EngineLogger);

        s_pLogger = this;

        DYULDUZ_LOG_ENGINE_INFO("Yulduz::Logger initialized");
    }

    void Logger::release() {
        assert(nullptr != s_pLogger && "Yulduz::Logger is not initialized");

        DYULDUZ_LOG_ENGINE_INFO("Yulduz::Logger released");

        spdlog::drop_all();
        spdlog::shutdown();

        m_Logger       = nullptr;
        m_EngineLogger = nullptr;

        s_pLogger = nullptr;
    }

    YULDUZ_IF_DEBUG(Logger::State::~State() {
        assert(nullptr == s_pLogger && "Forgot to release Yulduz::Logger");
    });
}  // namespace Yulduz