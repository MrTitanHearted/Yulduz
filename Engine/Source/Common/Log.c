#include <Yulduz/Common/Log.h>

YULDUZ_Logger *g_pApplicationLogger = nullptr;
YULDUZ_Logger *g_pEngineLogger      = nullptr;

const char *g_LogLevelNames[] = {
    "FATAL",
    "ERROR",
    "WARN",
    "INFO",
    "DEBUG",
    "TRACE",
};

bool YULDUZ_InitializeLogs(YULDUZ_Logger loggers[2]) {
    g_pEngineLogger      = &loggers[0];
    g_pApplicationLogger = &loggers[1];
    return true;
}

void YULDUZ_ReleaseLogs() {
    g_pEngineLogger      = nullptr;
    g_pApplicationLogger = nullptr;
}

void YULDUZ_Log(
    YULDUZ_Logger  *logger,
    YULDUZ_LogLevel log_level,
    uint64_t        line_number,
    const char     *file_path,
    const char     *file_name,
    const char     *function_name,
    const char     *fmt,
    ...) {
    SDL_Time timestamp = {0};
    SDL_GetCurrentTime(&timestamp);

    if (logger->MinLogLevel < log_level)
        return;

    YULDUZ_LogEntry entry = {};
    entry.FilePath        = file_path;
    entry.FileName        = file_name;
    entry.FunctionName    = function_name;
    entry.LineNumber      = line_number;
    entry.Timestamp       = timestamp;
    entry.Format          = fmt;
    entry.LogLevel        = log_level;

    va_start(entry.FormatArguments, fmt);
    logger->LogPFN(logger, &entry);
    va_end(entry.FormatArguments);
}

const char *YULDUZ_GetLogLevelName(YULDUZ_LogLevel log_level) {
    return g_LogLevelNames[log_level];
}
