#include <Yulduz/Common/Assert.h>

YULDUZ_AssertPFN g_pfnAssert = nullptr;

void *g_UserData = nullptr;

bool YULDUZ_InitializeAsserts(
    YULDUZ_AssertPFN assert_pfn,
    void            *user_data) {
    if (nullptr == assert_pfn) {
        SDL_SetError("assert_pfn is nullptr");
        return false;
    }
    g_pfnAssert = assert_pfn;
    g_UserData  = user_data;
    return true;
}

void YULDUZ_ReleaseAsserts() {
    g_UserData  = nullptr;
    g_pfnAssert = nullptr;
}

void YULDUZ_Assert(
    bool        condition,
    const char *condition_string,
    uint64_t    line_number,
    const char *file_path,
    const char *file_name,
    const char *function_name,
    const char *fmt,
    ...) {
    SDL_Time timestamp = {};
    SDL_GetCurrentTime(&timestamp);

    if (condition)
        return;

    YULDUZ_AssertEntry entry = {};
    entry.LineNumber         = line_number;
    entry.Timestamp          = timestamp;
    entry.FunctionName       = function_name;
    entry.Condition          = condition_string;
    entry.FilePath           = file_path;
    entry.FileName           = file_name;
    entry.Format             = fmt;

    va_start(entry.FormatArguments, fmt);
    g_pfnAssert(g_UserData, &entry);
    va_end(entry.FormatArguments);

    exit(-1);
}