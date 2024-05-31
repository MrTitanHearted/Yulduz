#pragma once

#include <YulduzCore/YulduzPCH.hpp>

#ifdef YULDUZ_BUILD_TYPE_DEBUG
#define LOG_LEVEL(level, fmt, ...) Yulduz::log(level, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) Yulduz::log(Yulduz::LogLevel::Fatal, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Yulduz::log(Yulduz::LogLevel::Error, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) Yulduz::log(Yulduz::LogLevel::Warn, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) Yulduz::log(Yulduz::LogLevel::Debug, __FUNCTION__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) Yulduz::log(Yulduz::LogLevel::Info, __FUNCTION__, fmt, ##__VA_ARGS__)

#define YZLOG(level, fmt, ...) Yulduz::Logger::Log(level, __FUNCTION__, fmt, ##__VA_ARGS__)
#define YZFATAL(fmt, ...) Yulduz::Logger::Log(Yulduz::LogLevel::Fatal, __FUNCTION__, fmt, ##__VA_ARGS__)
#define YZERROR(fmt, ...) Yulduz::Logger::Log(Yulduz::LogLevel::Error, __FUNCTION__, fmt, ##__VA_ARGS__)
#define YZWARN(fmt, ...) Yulduz::Logger::Log(Yulduz::LogLevel::Warn, __FUNCTION__, fmt, ##__VA_ARGS__)
#define YZDEBUG(fmt, ...) Yulduz::Logger::Log(Yulduz::LogLevel::Debug, __FUNCTION__, fmt, ##__VA_ARGS__)
#define YZINFO(fmt, ...) Yulduz::Logger::Log(Yulduz::LogLevel::Info, __FUNCTION__, fmt, ##__VA_ARGS__)
#else
#define LOG_LEVEL(level, fmt, ...)
#define LOG_FATAL(fmt, ...)
#define LOG_ERROR(fmt, ...)
#define LOG_WARN(fmt, ...)
#define LOG_DEBUG(fmt, ...)
#define LOG_INFO(fmt, ...)

#define YZLOG(level, fmt, ...)
#define YZFATAL(fmt, ...)
#define YZERROR(fmt, ...)
#define YZWARN(fmt, ...)
#define YZDEBUG(fmt, ...)
#define YZINFO(fmt, ...)
#endif

namespace Yulduz {
    enum LogLevel {
        Fatal = 0,
        Error = 1,
        Warn = 2,
        Debug = 3,
        Info = 4,
    };

    void printLog(LogLevel level, const std::string &function, const std::string &message);
    std::string makeLogStr(LogLevel level, const std::string &function, const std::string &message);

    template <typename... Args>
    void log(LogLevel level, const std::string &function, const std::string &fmt, Args &&...args) {
        std::string message = std::vformat(fmt, std::make_format_args(args...));
        printLog(level, function, message);
    }

    class Logger {
       public:
        static void Initialize(LogLevel level, bool trace, const std::string &tracePath);
        static void Shutdown();

        inline static void Initialize(LogLevel level, bool trace) { Initialize(level, trace, "YulduzTrace.txt"); }
        inline static void Initialize(LogLevel level) { Initialize(level, false, "YulduzTrace.txt"); }
        inline static void Initialize(bool trace, const std::string &tracePath) { Initialize(LogLevel::Warn, trace, tracePath); }
        inline static void Initialize(bool trace) { Initialize(LogLevel::Warn, trace, "YulduzTrace.txt"); }
        inline static void Initialize() { Initialize(LogLevel::Warn, false, "YulduzTrace.txt"); }

        template <typename... Args>
        static void Log(LogLevel level, const std::string &function, const std::string &fmt, Args &&...args) {
#if defined(YULDUZ_BUILD_TYPE_DEBUG)
            if (g_Logger == nullptr) {
                LOG_WARN("Logger is not initialized. Logger should be initialized first!");
                return;
            }

            std::string message = std::vformat(fmt, std::make_format_args(args...));
            printLog(level, function, message);

            if (!g_Logger->m_Trace) return;

            g_Logger->m_TraceContent << makeLogStr(level, function, message) << std::endl;
#endif
        }

       private:
        static Logger *g_Logger;

        LogLevel m_Level;
        bool m_Trace;
        std::string m_TracePath;
        std::stringstream m_TraceContent;

        Logger(LogLevel level, bool trace, const std::string &tracePath);
        ~Logger();
    };

}  // namespace Yulduz