#pragma once

#include <Yulduz/Common/PCH.h>

typedef struct YULDUZ_AssertEntry YULDUZ_AssertEntry;

typedef void (*YULDUZ_AssertPFN)(void *user_data, YULDUZ_AssertEntry *entry);

struct YULDUZ_AssertEntry {
    uint64_t LineNumber;
    SDL_Time Timestamp;

    const char *FunctionName;
    const char *Condition;
    const char *FilePath;
    const char *FileName;
    const char *Format;

    va_list FormatArguments;
};

YULDUZ_API bool YULDUZ_InitializeAsserts(
    YULDUZ_AssertPFN assert_pfn,
    void            *user_data);
YULDUZ_API void YULDUZ_ReleaseAsserts();

YULDUZ_API void YULDUZ_Assert(
    bool        condition,
    const char *condition_string,
    uint64_t    line_number,
    const char *file_path,
    const char *file_name,
    const char *function_name,
    const char *fmt,
    ...);

#ifndef YULDUZ_ASSERT
    #define YULDUZ_ASSERT(condition, fmt, ...) YULDUZ_Assert((condition), #condition, __LINE__, __FILE__, __FILE_NAME__, __FUNCTION__, fmt, ##__VA_ARGS__)
#endif

#ifndef YULDUZ_BUILD_TYPE_RELEASE
    #ifndef DYULDUZ_ASSERT
        #define DYULDUZ_ASSERT(condition, fmt, ...) YULDUZ_ASSERT(condition, fmt, ##__VA_ARGS__)
    #endif
#else
    #ifndef DYULDUZ_ASSERT
        #define DYULDUZ_ASSERT(condition, fmt, ...) ((void)0)
    #endif
#endif