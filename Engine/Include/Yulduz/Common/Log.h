#pragma once

#include <Yulduz/Common/PCH.h>

typedef enum YULDUZ_LogLevel YULDUZ_LogLevel;

typedef struct YULDUZ_Logger   YULDUZ_Logger;
typedef struct YULDUZ_LoggerMT YULDUZ_LoggerMT;
typedef struct YULDUZ_LogEntry YULDUZ_LogEntry;

typedef void (*YULDUZ_LogPFN)(YULDUZ_Logger *logger, YULDUZ_LogEntry *entry);

enum YULDUZ_LogLevel {
    YULDUZ_LogLevel_Fatal   = 0,
    YULDUZ_LogLevel_Error   = 1,
    YULDUZ_LogLevel_Warning = 2,
    YULDUZ_LogLevel_Info    = 3,
    YULDUZ_LogLevel_Debug   = 4,
    YULDUZ_LogLevel_Trace   = 5,
};

struct YULDUZ_Logger {
    const char *Name;

    void *UserData;

    YULDUZ_LogPFN LogPFN;

    YULDUZ_LogLevel MinLogLevel;
};

struct YULDUZ_LogEntry {
    const char *FilePath;
    const char *FileName;
    const char *FunctionName;
    const char *Format;

    va_list FormatArguments;

    uint64_t LineNumber;
    SDL_Time Timestamp;

    YULDUZ_LogLevel LogLevel;
};

YULDUZ_API bool YULDUZ_InitializeLogs(YULDUZ_Logger *engine_logger, YULDUZ_Logger *application_logger);
YULDUZ_API void YULDUZ_ReleaseLogs();

YULDUZ_API void YULDUZ_Log(
    YULDUZ_Logger  *logger,
    YULDUZ_LogLevel log_level,
    uint64_t        line_number,
    const char     *file_path,
    const char     *file_name,
    const char     *function_name,
    const char     *fmt,
    ...);

YULDUZ_API const char *YULDUZ_GetLogLevelName(YULDUZ_LogLevel log_level);

YULDUZ_API extern YULDUZ_Logger *g_pApplicationLogger;
YULDUZ_API extern YULDUZ_Logger *g_pEngineLogger;

#ifndef YULDUZ_LOGGER_CALL
    #define YULDUZ_LOGGER_CALL(logger, log_level, fmt, ...) YULDUZ_Log(logger, log_level, __LINE__, __FILE__, __FILE_NAME__, __FUNCTION__, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOGGER_FATAL
    #define YULDUZ_LOGGER_FATAL(logger, fmt, ...) YULDUZ_LOGGER_CALL(logger, YULDUZ_LogLevel_Fatal, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOGGER_ERROR
    #define YULDUZ_LOGGER_ERROR(logger, fmt, ...) YULDUZ_LOGGER_CALL(logger, YULDUZ_LogLevel_Error, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOGGER_WARN
    #define YULDUZ_LOGGER_WARN(logger, fmt, ...) YULDUZ_LOGGER_CALL(logger, YULDUZ_LogLevel_Warning, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOGGER_INFO
    #define YULDUZ_LOGGER_INFO(logger, fmt, ...) YULDUZ_LOGGER_CALL(logger, YULDUZ_LogLevel_Info, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOGGER_DEBUG
    #define YULDUZ_LOGGER_DEBUG(logger, fmt, ...) YULDUZ_LOGGER_CALL(logger, YULDUZ_LogLevel_Debug, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOGGER_TRACE
    #define YULDUZ_LOGGER_TRACE(logger, fmt, ...) YULDUZ_LOGGER_CALL(logger, YULDUZ_LogLevel_Trace, fmt, ##__VA_ARGS__)
#endif

#ifndef YULDUZ_LOG_CALL
    #define YULDUZ_LOG_CALL(log_level, fmt, ...) YULDUZ_LOGGER_CALL(g_pApplicationLogger, log_level, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_FATAL
    #define YULDUZ_LOG_FATAL(fmt, ...) YULDUZ_LOG_CALL(YULDUZ_LogLevel_Fatal, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ERROR
    #define YULDUZ_LOG_ERROR(fmt, ...) YULDUZ_LOG_CALL(YULDUZ_LogLevel_Error, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_WARN
    #define YULDUZ_LOG_WARN(fmt, ...) YULDUZ_LOG_CALL(YULDUZ_LogLevel_Warning, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_INFO
    #define YULDUZ_LOG_INFO(fmt, ...) YULDUZ_LOG_CALL(YULDUZ_LogLevel_Info, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_DEBUG
    #define YULDUZ_LOG_DEBUG(fmt, ...) YULDUZ_LOG_CALL(YULDUZ_LogLevel_Debug, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_TRACE
    #define YULDUZ_LOG_TRACE(fmt, ...) YULDUZ_LOG_CALL(YULDUZ_LogLevel_Trace, fmt, ##__VA_ARGS__)
#endif

#ifndef YULDUZ_BUILD_TYPE_RELEASE
    #ifndef DYULDUZ_LOG_CALL
        #define DYULDUZ_LOG_CALL(log_level, fmt, ...) YULDUZ_LOG_CALL(log_level, fmt, ##__VA_ARGS__)
    #endif
    #ifndef DYULDUZ_LOG_FATAL
        #define DYULDUZ_LOG_FATAL(fmt, ...) DYULDUZ_LOG_CALL(YULDUZ_LogLevel_Fatal, fmt, ##__VA_ARGS__)
    #endif
    #ifndef DYULDUZ_LOG_ERROR
        #define DYULDUZ_LOG_ERROR(fmt, ...) DYULDUZ_LOG_CALL(YULDUZ_LogLevel_Error, fmt, ##__VA_ARGS__)
    #endif
    #ifndef DYULDUZ_LOG_WARN
        #define DYULDUZ_LOG_WARN(fmt, ...) DYULDUZ_LOG_CALL(YULDUZ_LogLevel_Warning, fmt, ##__VA_ARGS__)
    #endif
    #ifndef DYULDUZ_LOG_INFO
        #define DYULDUZ_LOG_INFO(fmt, ...) DYULDUZ_LOG_CALL(YULDUZ_LogLevel_Info, fmt, ##__VA_ARGS__)
    #endif
    #ifndef DYULDUZ_LOG_DEBUG
        #define DYULDUZ_LOG_DEBUG(fmt, ...) DYULDUZ_LOG_CALL(YULDUZ_LogLevel_Debug, fmt, ##__VA_ARGS__)
    #endif
    #ifndef DYULDUZ_LOG_TRACE
        #define DYULDUZ_LOG_TRACE(fmt, ...) DYULDUZ_LOG_CALL(YULDUZ_LogLevel_Trace, fmt, ##__VA_ARGS__)
    #endif
#else
    #ifndef DYULDUZ_LOG_CALL
        #define DYULDUZ_LOG_CALL(log_level, fmt, ...) ((void)0)
    #endif
    #ifndef DYULDUZ_LOG_FATAL
        #define DYULDUZ_LOG_FATAL(fmt, ...) ((void)0)
    #endif
    #ifndef DYULDUZ_LOG_ERROR
        #define DYULDUZ_LOG_ERROR(fmt, ...) ((void)0)
    #endif
    #ifndef DYULDUZ_LOG_WARN
        #define DYULDUZ_LOG_WARN(fmt, ...) ((void)0)
    #endif
    #ifndef DYULDUZ_LOG_INFO
        #define DYULDUZ_LOG_INFO(fmt, ...) ((void)0)
    #endif
    #ifndef DYULDUZ_LOG_DEBUG
        #define DYULDUZ_LOG_DEBUG(fmt, ...) ((void)0)
    #endif
#endif

#ifndef YULDUZ_LOG_ENGINE_CALL
    #define YULDUZ_LOG_ENGINE_CALL(log_level, fmt, ...) YULDUZ_LOGGER_CALL(g_pEngineLogger, log_level, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ENGINE_FATAL
    #define YULDUZ_LOG_ENGINE_FATAL(fmt, ...) YULDUZ_LOG_ENGINE_CALL(YULDUZ_LogLevel_Fatal, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ENGINE_ERROR
    #define YULDUZ_LOG_ENGINE_ERROR(fmt, ...) YULDUZ_LOG_ENGINE_CALL(YULDUZ_LogLevel_Error, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ENGINE_WARN
    #define YULDUZ_LOG_ENGINE_WARN(fmt, ...) YULDUZ_LOG_ENGINE_CALL(YULDUZ_LogLevel_Warning, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ENGINE_INFO
    #define YULDUZ_LOG_ENGINE_INFO(fmt, ...) YULDUZ_LOG_ENGINE_CALL(YULDUZ_LogLevel_Info, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ENGINE_DEBUG
    #define YULDUZ_LOG_ENGINE_DEBUG(fmt, ...) YULDUZ_LOG_ENGINE_CALL(YULDUZ_LogLevel_Debug, fmt, ##__VA_ARGS__)
#endif
#ifndef YULDUZ_LOG_ENGINE_TRACE
    #define YULDUZ_LOG_ENGINE_TRACE(fmt, ...) YULDUZ_LOG_ENGINE_CALL(YULDUZ_LogLevel_Trace, fmt, ##__VA_ARGS__)
#endif

#ifndef YULDUZ_BUILD_TYPE_RELEASE
    #ifndef DYULDUZ_LOG_ENGINE_CALL
        #define DYULDUZ_LOG_ENGINE_CALL(log_level, fmt, ...) YULDUZ_LOG_ENGINE_CALL(log_level, fmt, ##__VA_ARGS__)
    #endif
    #ifndef DYULDUZ_LOG_ENGINE_FATAL
        #define DYULDUZ_LOG_ENGINE_FATAL(fmt, ...) DYULDUZ_LOG_ENGINE_CALL(YULDUZ_LogLevel_Fatal, fmt, ##__VA_ARGS__)
    #endif
    #ifndef DYULDUZ_LOG_ENGINE_ERROR
        #define DYULDUZ_LOG_ENGINE_ERROR(fmt, ...) DYULDUZ_LOG_ENGINE_CALL(YULDUZ_LogLevel_Error, fmt, ##__VA_ARGS__)
    #endif
    #ifndef DYULDUZ_LOG_ENGINE_WARN
        #define DYULDUZ_LOG_ENGINE_WARN(fmt, ...) DYULDUZ_LOG_ENGINE_CALL(YULDUZ_LogLevel_Warning, fmt, ##__VA_ARGS__)
    #endif
    #ifndef DYULDUZ_LOG_ENGINE_INFO
        #define DYULDUZ_LOG_ENGINE_INFO(fmt, ...) DYULDUZ_LOG_ENGINE_CALL(YULDUZ_LogLevel_Info, fmt, ##__VA_ARGS__)
    #endif
    #ifndef DYULDUZ_LOG_ENGINE_DEBUG
        #define DYULDUZ_LOG_ENGINE_DEBUG(fmt, ...) DYULDUZ_LOG_ENGINE_CALL(YULDUZ_LogLevel_Debug, fmt, ##__VA_ARGS__)
    #endif
    #ifndef DYULDUZ_LOG_ENGINE_TRACE
        #define DYULDUZ_LOG_ENGINE_TRACE(fmt, ...) DYULDUZ_LOG_ENGINE_CALL(YULDUZ_LogLevel_Trace, fmt, ##__VA_ARGS__)
    #endif
#else
    #ifndef DYULDUZ_LOG_ENGINE_CALL
        #define DYULDUZ_LOG_ENGINE_CALL(log_level, fmt, ...) ((void)0)
    #endif
    #ifndef DYULDUZ_LOG_ENGINE_FATAL
        #define DYULDUZ_LOG_ENGINE_FATAL(fmt, ...) ((void)0)
    #endif
    #ifndef DYULDUZ_LOG_ENGINE_ERROR
        #define DYULDUZ_LOG_ENGINE_ERROR(fmt, ...) ((void)0)
    #endif
    #ifndef DYULDUZ_LOG_ENGINE_WARN
        #define DYULDUZ_LOG_ENGINE_WARN(fmt, ...) ((void)0)
    #endif
    #ifndef DYULDUZ_LOG_ENGINE_INFO
        #define DYULDUZ_LOG_ENGINE_INFO(fmt, ...) ((void)0)
    #endif
    #ifndef DYULDUZ_LOG_ENGINE_DEBUG
        #define DYULDUZ_LOG_ENGINE_DEBUG(fmt, ...) ((void)0)
    #endif
#endif