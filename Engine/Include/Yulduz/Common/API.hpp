#pragma once

#include <Yulduz/Common/PCH.hpp>

#if defined(YULDUZ_STATIC)
    #define YULDUZ_API
#else
    #if defined(YULDUZ_PLATFORM_WINDOWS)
        #if defined(YULDUZ_BUILD)
            #define YULDUZ_API __declspec(dllexport)
        #else
            #define YULDUZ_API __declspec(dllimport)
        #endif
    #else
        #define YULDUZ_API __attribute__((visibility("default")))
    #endif
#endif