#pragma once

#include <YulduzCore/YulduzPCH.hpp>

#ifdef YULDUZ_BUILD_TYPE_DEBUG
#define LOG_LEVEL(level, fmt, ...) Yulduz::log(level, __FUNCTION__, std::source_location::current(), fmt, __VA_ARGS__)
#define LOG_FATAL(fmt, ...) Yulduz::log(Yulduz::LogLevel::Fatal, __FUNCTION__, std::source_location::current(), fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) Yulduz::log(Yulduz::LogLevel::Error, __FUNCTION__, std::source_location::current(), fmt, __VA_ARGS__)
#define LOG_WARN(fmt, ...) Yulduz::log(Yulduz::LogLevel::Warn, __FUNCTION__, std::source_location::current(), fmt, __VA_ARGS__)
#define LOG_DEBUG(fmt, ...) Yulduz::log(Yulduz::LogLevel::Debug, __FUNCTION__, std::source_location::current(), fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...) Yulduz::log(Yulduz::LogLevel::Info, __FUNCTION__, std::source_location::current(), fmt, __VA_ARGS__)

#define YZLOG(level, fmt, ...) Yulduz::Logger::Log(level, __FUNCTION__, std::source_location::current(), fmt, __VA_ARGS__)
#define YZFATAL(fmt, ...) Yulduz::Logger::Log(Yulduz::LogLevel::Fatal, __FUNCTION__, std::source_location::current(), fmt, __VA_ARGS__)
#define YZERROR(fmt, ...) Yulduz::Logger::Log(Yulduz::LogLevel::Error, __FUNCTION__, std::source_location::current(), fmt, __VA_ARGS__)
#define YZWARN(fmt, ...) Yulduz::Logger::Log(Yulduz::LogLevel::Warn, __FUNCTION__, std::source_location::current(), fmt, __VA_ARGS__)
#define YZDEBUG(fmt, ...) Yulduz::Logger::Log(Yulduz::LogLevel::Debug, __FUNCTION__, std::source_location::current(), fmt, __VA_ARGS__)
#define YZINFO(fmt, ...) Yulduz::Logger::Log(Yulduz::LogLevel::Info, __FUNCTION__, std::source_location::current(), fmt, __VA_ARGS__)
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

    bool IsLogLevelVerbose(LogLevel level);
    void SetLogLevelVerbose(LogLevel level, bool verbose);
    void SetAllLogLevelVerbose(bool verbose);

    void printLog(LogLevel level, const std::string &function, const std::source_location &location, const std::string &log);

    std::string makeLogString(LogLevel level, const std::string &function, const std::source_location &location, const std::string &log);

    template <typename... Args>
    void log(LogLevel level, const std::string &function, const std::source_location &location, const std::string &fmt, const Args &...args) {
        std::string message = std::vformat(fmt, std::make_format_args(args...));
        printLog(level, function, location, message);
    }

#if defined(YULDUZ_BUILD_TYPE_DEBUG)
    class Logger {
       public:
        static void SetLogLevel(LogLevel level);
        static void SetTraceEnable(bool trace);
        static void SetTraceFile(const std::string &file);

        template <typename... Args>
        static void     Log(LogLevel level, const std::string &function, const std::source_location &location, const std::string &fmt, Args &&...args) {
            if (g_Logger.m_Level < level) return;

            std::string message = std::vformat(fmt, std::make_format_args(args...));
            printLog(level, function, location, message);

            if (!g_Logger.m_Trace) return;

            g_Logger.m_TraceContent << makeLogString(level, function, location, message) << std::endl;
        }

       private:
        static Logger g_Logger;

        LogLevel m_Level;
        bool m_Trace;
        std::string m_TracePath;
        std::stringstream m_TraceContent;

        Logger();
        ~Logger();
    };
#else
    namespace Logger {
        static void SetLogLevel(LogLevel level) {}
        static void SetTraceEnable(bool trace) {}
        static void SetTraceFile(const std::string &file) {}

        template <typename... Args>
        static void Log(LogLevel level, const std::string &function, size_t line, const std::string &fmt, Args &&...args) {}
    }  // namespace Logger
#endif
}  // namespace Yulduz
