#pragma once

#include <Yulduz/Common/Assert.h>
#include <Yulduz/Common/Defines.h>
#include <Yulduz/Common/Error.h>
#include <Yulduz/Common/Log.h>
#include <Yulduz/Common/PCH.h>

typedef struct YULDUZ_Context YULDUZ_Context;
typedef struct YULDUZ_ContextInitializeInfo
    YULDUZ_ContextInitializeInfo;

struct YULDUZ_Context {
    YULDUZ_Logger EngineLogger;
    YULDUZ_Logger ApplicationLogger;
};

struct YULDUZ_ContextInitializeInfo {
    YULDUZ_AssertPFN AssertPFN;
    void            *AssertUserData;

    YULDUZ_Logger EngineLogger;
    YULDUZ_Logger ApplicationLogger;
};

YULDUZ_API bool YULDUZ_InitializeContext(YULDUZ_Context *context, YULDUZ_ContextInitializeInfo initialize_info);
YULDUZ_API void YULDUZ_ReleaseContext(YULDUZ_Context *context);