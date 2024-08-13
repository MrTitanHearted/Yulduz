#include <YulduzCore/Logger.hpp>
#include <YulduzCore/Timer.hpp>

#ifdef YULDUZ_BUILD_TYPE_DEBUG
#ifdef YULDUZ_PLATFORM_WINDOWS
#pragma warning(disable : 5082)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
const int32_t YULDUZ_TEXT_COLOR_RED = FOREGROUND_RED | FOREGROUND_INTENSITY;
const int32_t YULDUZ_TEXT_COLOR_YELLOW = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const int32_t YULDUZ_TEXT_COLOR_BLUE = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const int32_t YULDUZ_TEXT_COLOR_GREEN = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
const int32_t YULDUZ_TEXT_COLOR_RESET = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
#elif YULDUZ_PLATFORM_UNIX
#pragma GCC diagnostic ignored "-Wno-5082"
const std::string YULDUZ_TEXT_COLOR_RED = "\033[31m";
const std::string YULDUZ_TEXT_COLOR_YELLOW = "\033[33m";
const std::string YULDUZ_TEXT_COLOR_BLUE = "\033[34m";
const std::string YULDUZ_TEXT_COLOR_GREEN = "\033[32m";
const std::string YULDUZ_TEXT_COLOR_RESET = "\033[0m";
#else
#error Not supported platform
#endif
#endif

namespace Yulduz {
#if defined(YULDUZ_BUILD_TYPE_DEBUG)
    Logger Logger::g_Logger{};
    bool g_LogLevelVerbosity[5]{true, true, true, false, false};

    bool IsLogLevelVerbose(LogLevel level) {
        return g_LogLevelVerbosity[level];
    }

    void SetLogLevelVerbose(LogLevel level, bool verbose) {
        g_LogLevelVerbosity[level] = verbose;
    }

    void SetAllLogLevelVerbose(bool verbose) {
        g_LogLevelVerbosity[LogLevel::Fatal] = verbose;
        g_LogLevelVerbosity[LogLevel::Error] = verbose;
        g_LogLevelVerbosity[LogLevel::Warn] = verbose;
        g_LogLevelVerbosity[LogLevel::Debug] = verbose;
        g_LogLevelVerbosity[LogLevel::Info] = verbose;
    }
#else
    bool IsLogLevelVerbose(LogLevel level) { return false; }
    void SetLogLevelVerbose(LogLevel level, bool verbose) {}
    void SetAllLogLevelVerbose(bool verbose) {}
#endif

    const char *LogLevelStrs[]{
        "FATAL",
        "ERROR",
        "WARN",
        "DEBUG",
        "INFO",
    };

    void printLog(LogLevel level, const std::string &function, const std::source_location &location, const std::string &message) {
#if defined(YULDUZ_BUILD_TYPE_DEBUG)
#if defined(YULDUZ_PLATFORM_WINDOWS)
        int32_t color = 0;
        switch (level) {
            case LogLevel::Fatal:
            case LogLevel::Error:
                color = YULDUZ_TEXT_COLOR_RED;
                break;
            case LogLevel::Warn:
                color = YULDUZ_TEXT_COLOR_YELLOW;
                break;
            case LogLevel::Debug:
                color = YULDUZ_TEXT_COLOR_GREEN;
                break;
            case LogLevel::Info:
                color = YULDUZ_TEXT_COLOR_BLUE;
                break;
            default:
                throw std::runtime_error("Unknown log level!");
        }
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hConsole, color);
        std::cout << makeLogString(level, function, location, message) << std::endl;
        SetConsoleTextAttribute(hConsole, YULDUZ_TEXT_COLOR_RESET);
#elif defined(YULDUZ_PLATFORM_UNIX)
        std::string color;
        switch (level) {
            case LogLevel::Fatal:
            case LogLevel::Error:
                color = YULDUZ_TEXT_COLOR_RED;
                break;
            case LogLevel::Warn:
                color = YULDUZ_TEXT_COLOR_YELLOW;
                break;
            case LogLevel::Debug:
                color = YULDUZ_TEXT_COLOR_GREEN;
                break;
            case LogLevel::Info:
                color = YULDUZ_TEXT_COLOR_BLUE;
                break;
            default:
                throw std::runtime_error("Unknown log level!");
        }

        std::cout << color
                  << makeLogString(level, function, location, message) << std::endl
                  << YULDUZ_TEXT_COLOR_RESET;
#else
#error Not supported platform
#endif
#endif
    }

    std::string makeLogString(LogLevel level, const std::string &function, const std::source_location &location, const std::string &message) {
        std::string currentTimeString = Timer::GetCurrentTimeStr();

        if (IsLogLevelVerbose(level)) {
            const std::uint32_t line = location.line();
            const std::uint32_t column = location.column();
            std::string file = location.file_name();
            return std::vformat("[{}]::[{}]::[{}:{}:{}]::[{}]: {}",
                                std::make_format_args(currentTimeString,
                                                      LogLevelStrs[level],
                                                      file,
                                                      line,
                                                      column,
                                                      function,
                                                      message));
        }

        return std::vformat("[{}]::[{}]: {}",
                            std::make_format_args(currentTimeString,
                                                  LogLevelStrs[level],
                                                  message));
    }

#if defined(YULDUZ_BUILD_TYPE_DEBUG)
    void Logger::SetLogLevel(LogLevel level) {
        g_Logger.m_Level = level;
    }

    void Logger::SetTraceEnable(bool trace) {
        g_Logger.m_Trace = trace;
    }

    void Logger::SetTraceFile(const std::string &file) {
        g_Logger.m_TracePath = file;
    }

    Logger::Logger() {
        LOG_INFO("Initializing Yulduz Logger");

        m_Level = LogLevel::Warn;
        m_Trace = false;
        m_TracePath = "Yulduz.txt";
    }

    Logger::~Logger() {
        LOG_INFO("Releasing Yulduz Logger");
        
        if (m_Trace && !m_TracePath.empty()) {
            std::ofstream file(m_TracePath);
            file << m_TraceContent.str();
        }
    }
#endif
}  // namespace Yulduz