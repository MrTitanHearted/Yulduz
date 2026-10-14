#pragma once

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>

#include <Yulduz/Vendor.h>

#if !defined(YULDUZ_PROJECT_ROOT_DIRECTORY)
    #define YULDUZ_PROJECT_ROOT_DIRECTORY "/"
#endif

#define YULDUZ_PLATFORM_WINDOWS 0
#define YULDUZ_PLATFORM_LINUX 0
#define YULDUZ_PLATFORM_APPLE 0

#define YULDUZ_PLATFORM_STRING ""

#if defined(_WIN32) || defined(_WIN64)
    #define YULDUZ_PLATFORM_WINDOWS 1
    #define YULDUZ_PLATFORM_STRING "Windows"
#elif defined(__linux__)
    #undef YULDUZ_PLATFORM_LINUX
    #undef YULDUZ_PLATFORM_STRING
    #define YULDUZ_PLATFORM_LINUX 1
    #define YULDUZ_PLATFORM_STRING "Linux"
    #if defined(__ANDROID__)
        #error Unsupported Platform. Android is not supported yet
    #endif
#elif defined(__APPLE__) && defined(__MACH__)
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        #define YULDUZ_PLATFORM_APPLE 1
        #define YULDUZ_PLATFORM_STRING "Apple"
    #else
        #error Unsupported Apple platform
    #endif
#else
    #error Unsupported Platform
#endif

#if !defined(YULDUZ_BUILD_TYPE_DEBUG) && !defined(YULDUZ_BUILD_TYPE_RELWITHDEBINFO) && !defined(YULDUZ_BUILD_TYPE_RELEASE)
    #define YULDUZ_BUILD_TYPE_DEBUG
#endif

#if defined(YULDUZ_ENGINE_STATIC)
    #define YULDUZ_API
#else
    #if YULDUZ_PLATFORM_WINDOWS
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
        #define YULDUZ_IF_DEBUG(statement) ((void)0)
    #endif
#endif

// Pointers that can be nullptr or NULL
#define YULDUZ_NULLABLE

// Integer Handles
#define YULDUZ_HANDLE
// Integer Enums
#define YULDUZ_ENUM

#define YULDUZ_B 1
#define YULDUZ_KB (YULDUZ_B * 1024)
#define YULDUZ_MB (YULDUZ_KB * 1024)
#define YULDUZ_GB (YULDUZ_MB * 1024)