#pragma once

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdbit.h>
#include <stdatomic.h>
#include <time.h>

#if !defined(YULDUZ_PROJECT_ROOT_DIR)
    #define YULDUZ_PROJECT_ROOT_DIR "/"
#endif

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

#if defined(YULDUZ_ENGINE_STATIC)
    #define YULDUZ_API
#else
    #if defined(YULDUZ_PLATFORM_WINDOWS)
        #if defined(YULDUZ_ENGINE_BUILD)
            #define YULDUZ_API __declspec(dllexport)
        #else
            #define YULDUZ_API __declspec(dllimport)
        #endif
    #else
        #define YULDUZ_API __attribute__((visibility("default")))
    #endif
#endif

#if defined(YULDUZ_BUILD_TYPE_DEBUG) || defined(YULDUZ_BUILD_TYPE_RELWITHDEBINFO)
    #ifndef YULDUZ_IF_DEBUG
        #define YULDUZ_IF_DEBUG(statement) statement
    #endif
#else
    #ifndef YULDUZ_IF_DEBUG
        #define YULDUZ_IF_DEBUG(statement) ((void)0);
    #endif
#endif

#ifndef YULDUZ_NULLABLE
// Pointers that can be nullptr or NULL
    #define YULDUZ_NULLABLE
#endif