#pragma once

#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <cstdarg>
#include <cstdio>

#if !defined(YULDUZ_PROJECT_ROOT_DIR)
    #define YULDUZ_PROJECT_ROOT_DIR "/"
#endif

#define YULDUZ_RELATIVE_FILE_ (&__FILE__[std::strlen(YULDUZ_PROJECT_ROOT_DIR) + 1])

#if defined(_WIN32) || defined(_WIN64)
    #define YULDUZ_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define YULDUZ_PLATFORM_UNIX
    #define YULDUZ_PLATFORM_LINUX
    #if defined(__ANDROID__)
        #define YULDUZ_PLATFORM_ANDROID
    #endif
#elif defined(__APPLE__) && defined(__MACH__)
    #include <TargetConditionals.h>

    #define YULDUZ_PLATFORM_APPLE
    #define YULDUZ_PLATFORM_UNIX

    #if TARGET_OS_MAC
        #define YULDUZ_PLATFORM_MACOS
    #elif TARGET_OS_IPHONE
        #define YULDUZ_PLATFORM_IOS
    #else
        #error Unsupported Apple platform
    #endif
// #elif defined(__EMSCRIPTEN__)
//     #define YULDUZ_PLATFORM_WEB
//     #define YULDUZ_PLATFORM_UNIX
#else
    #error Unsupported platform!
#endif

#if !defined(YULDUZ_BUILD_TYPE_RELEASE) && !defined(YULDUZ_BUILD_TYPE_DEBUG) && !defined(YULDUZ_BUILD_TYPE_RELWITHDEBINFO)
    #define YULDUZ_BUILD_TYPE_DEBUG
#endif

#if defined(YULDUZ_BUILD_TYPE_DEBUG) || defined(YULDUZ_BUILD_TYPE_RELWITHDEBINFO)
    #ifndef YULDUZ_IF_DEBUG
        #define YULDUZ_IF_DEBUG(statement) statement
    #endif
#else
    #ifndef YULDUZ_IF_DEBUG
        #define YULDUZ_IF_DEBUG(statement)
    #endif
#endif
