#include <YulduzCore/Logger.hpp>
#include <YulduzCore/Timer.hpp>

#ifdef YULDUZ_PLATFORM_WINDOWS
#pragma warning(disable : 5082)
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

namespace Yulduz {
    Logger *Logger::g_Logger = nullptr;

    const char *LogLevelStrs[]{
        "FATAL",
        "ERROR",
        "WARN",
        "DEBUG",
        "INFO",
    };

    void printLog(LogLevel level, const std::string &function, const std::string &message) {
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
        std::cout << makeLogStr(level, function, message) << std::endl;
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
                  << makeLogStr(level, function, message) << std::endl
                  << YULDUZ_TEXT_COLOR_RESET;
#else
#error Not supported platform
#endif
    }

    std::string makeLogStr(LogLevel level, const std::string &function, const std::string &message) {
        return std::vformat("[{}]::[{}]::[{}]: {}",
                            std::make_format_args(Timer::GetCurrentTimeStr(),
                                                  LogLevelStrs[level],
                                                  function,
                                                  message));
    }

    void Logger::Initialize(LogLevel level, bool trace, const std::string &tracePath) {
        if (g_Logger != nullptr) {
            YZWARN("Logger is already initialized");
            return;
        }

        g_Logger = new Logger(level, trace, tracePath);
    }

    void Logger::Shutdown() {
        if (g_Logger == nullptr) {
            LOG_WARN("Logger is not initialized. Logger should be initialized first!");
            return;
        }

        delete g_Logger;
        g_Logger = nullptr;
    }

    Logger::Logger(LogLevel level, bool trace, const std::string &tracePath) {
        LOG_DEBUG("Initializing Logger with Log Level '{}'", LogLevelStrs[level]);

        m_Level = level;
        m_Trace = trace;
        m_TracePath = tracePath;

        if (m_Trace && !m_TracePath.empty()) {
            m_TracePath = "Yulduz.txt";
        }
    }

    Logger::~Logger() {
        LOG_DEBUG("Releasing Logger");
        if (m_Trace && !m_TracePath.empty()) {
            std::ofstream file(m_TracePath);
            file << m_TraceContent.str();
        }
    }
}  // namespace Yulduz
