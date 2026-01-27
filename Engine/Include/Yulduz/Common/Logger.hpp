#pragma once

#include <Yulduz/Common/API.hpp>

#ifndef YULDUZ_LOGGER_CALL
    #define YULDUZ_LOGGER_CALL(logger, level, fmt, ...) (logger)->log(spdlog::source_loc{YULDUZ_RELATIVE_FILE_, __LINE__, SPDLOG_FUNCTION}, level, fmt, ##__VA_ARGS__)
#endif

#ifndef YULDUZ_LOGGER_CRITICAL
    #define YULDUZ_LOGGER_CRITICAL(logger, fmt, ...) YULDUZ_LOGGER_CALL(logger, spdlog::level::critical, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOGGER_ERROR
    #define YULDUZ_LOGGER_ERROR(logger, fmt, ...) YULDUZ_LOGGER_CALL(logger, spdlog::level::err, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOGGER_DEBUG
    #define YULDUZ_LOGGER_DEBUG(logger, fmt, ...) YULDUZ_LOGGER_CALL(logger, spdlog::level::debug, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOGGER_WARN
    #define YULDUZ_LOGGER_WARN(logger, fmt, ...) YULDUZ_LOGGER_CALL(logger, spdlog::level::warn, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOGGER_INFO
    #define YULDUZ_LOGGER_INFO(logger, fmt, ...) YULDUZ_LOGGER_CALL(logger, spdlog::level::info, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOGGER_TRACE
    #define YULDUZ_LOGGER_TRACE(logger, fmt, ...) YULDUZ_LOGGER_CALL(logger, spdlog::level::trace, fmt, ##__VA_ARGS__)
#endif

#ifndef YULDUZ_LOG_ENGINE_CALL
    #define YULDUZ_LOG_ENGINE_CALL(level, fmt, ...) YULDUZ_LOGGER_CALL(::Yulduz::Logger::GetEngineLogger(), level, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ENGINE_CRITICAL
    #define YULDUZ_LOG_ENGINE_CRITICAL(fmt, ...) YULDUZ_LOG_ENGINE_CALL(spdlog::level::critical, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ENGINE_ERROR
    #define YULDUZ_LOG_ENGINE_ERROR(fmt, ...) YULDUZ_LOG_ENGINE_CALL(spdlog::level::err, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ENGINE_DEBUG
    #define YULDUZ_LOG_ENGINE_DEBUG(fmt, ...) YULDUZ_LOG_ENGINE_CALL(spdlog::level::debug, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ENGINE_WARN
    #define YULDUZ_LOG_ENGINE_WARN(fmt, ...) YULDUZ_LOG_ENGINE_CALL(spdlog::level::warn, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ENGINE_INFO
    #define YULDUZ_LOG_ENGINE_INFO(fmt, ...) YULDUZ_LOG_ENGINE_CALL(spdlog::level::info, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ENGINE_TRACE
    #define YULDUZ_LOG_ENGINE_TRACE(fmt, ...) YULDUZ_LOG_ENGINE_CALL(spdlog::level::trace, fmt, ##__VA_ARGS__)
#endif

#ifndef YULDUZ_LOG_CALL
    #define YULDUZ_LOG_CALL(level, fmt, ...) YULDUZ_LOGGER_CALL(::Yulduz::Logger::GetLogger(), level, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_CRITICAL
    #define YULDUZ_LOG_CRITICAL(fmt, ...) YULDUZ_LOG_CALL(spdlog::level::critical, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ERROR
    #define YULDUZ_LOG_ERROR(fmt, ...) YULDUZ_LOG_CALL(spdlog::level::err, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_DEBUG
    #define YULDUZ_LOG_DEBUG(fmt, ...) YULDUZ_LOG_CALL(spdlog::level::debug, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_WARN
    #define YULDUZ_LOG_WARN(fmt, ...) YULDUZ_LOG_CALL(spdlog::level::warn, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_INFO
    #define YULDUZ_LOG_INFO(fmt, ...) YULDUZ_LOG_CALL(spdlog::level::info, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_TRACE
    #define YULDUZ_LOG_TRACE(fmt, ...) YULDUZ_LOG_CALL(spdlog::level::trace, fmt, ##__VA_ARGS__)
#endif

#if defined(YULDUZ_BUILD_TYPE_DEBUG) || defined(YULDUZ_BUILD_TYPE_RELWITHDEBINFO)
    #ifndef DYULDUZ_LOGGER_CALL
        #define DYULDUZ_LOGGER_CALL(logger, level, fmt, ...) YULDUZ_LOGGER_CALL(logger, level, fmt, ##__VA_ARGS__)
    #endif
#else
    #ifndef DYULDUZ_LOGGER_CALL
        #define DYULDUZ_LOGGER_CALL(logger, level, fmt, ...)
    #endif
#endif

#ifndef DYULDUZ_LOGGER_CRITICAL
    #define DYULDUZ_LOGGER_CRITICAL(logger, fmt, ...) DYULDUZ_LOGGER_CALL(logger, spdlog::level::critical, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOGGER_ERROR
    #define DYULDUZ_LOGGER_ERROR(logger, fmt, ...) DYULDUZ_LOGGER_CALL(logger, spdlog::level::err, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOGGER_DEBUG
    #define DYULDUZ_LOGGER_DEBUG(logger, fmt, ...) DYULDUZ_LOGGER_CALL(logger, spdlog::level::debug, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOGGER_WARN
    #define DYULDUZ_LOGGER_WARN(logger, fmt, ...) DYULDUZ_LOGGER_CALL(logger, spdlog::level::warn, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOGGER_INFO
    #define DYULDUZ_LOGGER_INFO(logger, fmt, ...) DYULDUZ_LOGGER_CALL(logger, spdlog::level::info, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOGGER_TRACE
    #define DYULDUZ_LOGGER_TRACE(logger, fmt, ...) DYULDUZ_LOGGER_CALL(logger, spdlog::level::trace, fmt, ##__VA_ARGS__)
#endif

#ifndef DYULDUZ_LOG_ENGINE_CALL
    #define DYULDUZ_LOG_ENGINE_CALL(level, fmt, ...) DYULDUZ_LOGGER_CALL(::Yulduz::Logger::GetEngineLogger(), level, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOG_ENGINE_CRITICAL
    #define DYULDUZ_LOG_ENGINE_CRITICAL(fmt, ...) DYULDUZ_LOG_ENGINE_CALL(spdlog::level::critical, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOG_ENGINE_ERROR
    #define DYULDUZ_LOG_ENGINE_ERROR(fmt, ...) DYULDUZ_LOG_ENGINE_CALL(spdlog::level::err, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOG_ENGINE_DEBUG
    #define DYULDUZ_LOG_ENGINE_DEBUG(fmt, ...) DYULDUZ_LOG_ENGINE_CALL(spdlog::level::debug, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOG_ENGINE_WARN
    #define DYULDUZ_LOG_ENGINE_WARN(fmt, ...) DYULDUZ_LOG_ENGINE_CALL(spdlog::level::warn, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOG_ENGINE_INFO
    #define DYULDUZ_LOG_ENGINE_INFO(fmt, ...) DYULDUZ_LOG_ENGINE_CALL(spdlog::level::info, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOG_ENGINE_TRACE
    #define DYULDUZ_LOG_ENGINE_TRACE(fmt, ...) DYULDUZ_LOG_ENGINE_CALL(spdlog::level::trace, fmt, ##__VA_ARGS__)
#endif

#ifndef DYULDUZ_LOG_CALL
    #define DYULDUZ_LOG_CALL(level, fmt, ...) DYULDUZ_LOGGER_CALL(::Yulduz::Logger::GetLogger(), level, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOG_CRITICAL
    #define DYULDUZ_LOG_CRITICAL(fmt, ...) DYULDUZ_LOG_CALL(spdlog::level::critical, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOG_ERROR
    #define DYULDUZ_LOG_ERROR(fmt, ...) DYULDUZ_LOG_CALL(spdlog::level::err, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOG_DEBUG
    #define DYULDUZ_LOG_DEBUG(fmt, ...) DYULDUZ_LOG_CALL(spdlog::level::debug, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOG_WARN
    #define DYULDUZ_LOG_WARN(fmt, ...) DYULDUZ_LOG_CALL(spdlog::level::warn, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOG_INFO
    #define DYULDUZ_LOG_INFO(fmt, ...) DYULDUZ_LOG_CALL(spdlog::level::info, fmt, ##__VA_ARGS__)
#endif
#ifndef DYULDUZ_LOG_TRACE
    #define DYULDUZ_LOG_TRACE(fmt, ...) DYULDUZ_LOG_CALL(spdlog::level::trace, fmt, ##__VA_ARGS__)
#endif

#ifndef YULDUZ_ASSERT
    #define YULDUZ_ASSERT(condition, ...) ::Yulduz::Logger::AssertImpl((condition), #condition, spdlog::source_loc{YULDUZ_RELATIVE_FILE_, __LINE__, SPDLOG_FUNCTION}, ##__VA_ARGS__)
#endif

#ifndef DYULDUZ_ASSERT
    #if defined(YULDUZ_BUILD_TYPE_DEBUG) || defined(YULDUZ_BUILD_TYPE_RELWITHDEBINFO)
        #define DYULDUZ_ASSERT(condition, ...) YULDUZ_ASSERT(condition, ##__VA_ARGS__)
    #else
        #define DYULDUZ_ASSERT(condition, ...) \
            do {                               \
                (void)(condition);             \
            } while (false);
    #endif
#endif

namespace Yulduz {
    template <typename... FormatArgs>
    std::string FormatString(const std::string_view fmt, FormatArgs &&...args) {
        return std::vformat(fmt, std::make_format_args(args...));
    }

    class YULDUZ_API Logger {
       public:
        struct Settings {
            eastl::string_view LoggerName = "APPLICATION";

            spdlog::level::level_enum       LogLevel = spdlog::level::trace;
            eastl::vector<spdlog::sink_ptr> Sinks{};

            spdlog::level::level_enum       EngineLogLevel = spdlog::level::warn;
            eastl::vector<spdlog::sink_ptr> EngineSinks{};
        };

       public:
        static Logger &GetRef();

        static std::shared_ptr<spdlog::logger> GetLogger();
        static std::shared_ptr<spdlog::logger> GetEngineLogger();

        static eastl::string_view GetFileSinkPattern();
        static eastl::string_view GetConsoleSinkPattern();

        template <typename... Args>
        static void AssertImpl(
            const bool condition,

            const eastl::string_view condition_string,
            const spdlog::source_loc location,

            Args &&...args) {
            YULDUZ_IF_DEBUG(assert(nullptr != s_pLogger && "Yulduz::Logger is not initialized"));

            if (condition) return;

            if constexpr (sizeof...(args) > 0) {
                s_pLogger->m_EngineLogger->log(
                    location, spdlog::level::critical,
                    "Assertion failed for: '{}' with message: '{}'",
                    condition_string,
                    FormatString(std::forward<Args>(args)...));
            } else {
                s_pLogger->m_EngineLogger->log(
                    location, spdlog::level::critical,
                    "Assertion failed for: '{}'",
                    condition_string);
            }

            std::abort();
        }

       public:
        Logger()  = default;
        ~Logger() = default;

        void initialize(const Settings &settings);
        void release();

       private:
        YULDUZ_IF_DEBUG(class State {
           public:
            State() = default;
            ~State();
        });

       private:
        std::shared_ptr<spdlog::logger> m_Logger       = nullptr;
        std::shared_ptr<spdlog::logger> m_EngineLogger = nullptr;

       private:
        static Logger *s_pLogger;

        YULDUZ_IF_DEBUG(static State s_State);
    };
}  // namespace Yulduz