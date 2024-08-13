#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <iomanip>
#include <source_location>
#include <vector>
#include <array>
#include <span>
#include <string>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <set>
#include <tuple>
#include <utility>
#include <chrono>
#include <memory>
#include <thread>
#include <format>
#include <type_traits>
#include <typeindex>
#include <typeinfo>

namespace fs = std::filesystem;

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <GLFW/glfw3.h>

#include <wgpu/wgpu_impl_glfw.h>
#include <wgpu/wgpu.h>

#if defined(_WIN32) || defined(_WIN64)
#define YULDUZ_PLATFORM_WINDOWS
#elif defined(__APPLE__) || defined(__MACH__)
#define YULDUZ_PLATFORM_OSX
#define YULDUZ_PLATFORM_UNIX
#elif defined(__linux__)
#define YULDUZ_PLATFORM_LINUX
#define YULDUZ_PLATFORM_UNIX
#else
#error Unsupported platform!
#endif

// YulduzCore configuration
#ifndef YULDUZ_SOURCE_DIR
#define YULDUZ_SOURCE_DIR ""
#endif

#ifndef YULDUZ_BUILD_TYPE_RELEASE
#define YULDUZ_BUILD_TYPE_DEBUG
#endif

#define YULDUZ_ENUM_CLASS_BITWISE_OPERATORS(Type, Other)                         \
    inline Type operator|(Type a, Type b) {                                      \
        return static_cast<Type>(static_cast<Other>(a) | static_cast<Other>(b)); \
    }                                                                            \
    inline Type operator|=(Type &a, Type b) {                                    \
        return a = a | b;                                                        \
    }                                                                            \
    inline Type operator&(Type a, Type b) {                                      \
        return static_cast<Type>(static_cast<Other>(a) & static_cast<Other>(b)); \
    }                                                                            \
    inline Type operator&=(Type &a, Type b) {                                    \
        return a = a & b;                                                        \
    }                                                                            \
    inline Type operator^(Type a, Type b) {                                      \
        return static_cast<Type>(static_cast<Other>(a) ^ static_cast<Other>(b)); \
    }                                                                            \
    inline Type operator^=(Type &a, Type b) {                                    \
        return a = a ^ b;                                                        \
    }                                                                            \
    inline Type operator~(Type a) {                                              \
        return static_cast<Type>(~static_cast<Other>(a));                        \
    }

#define YULDUZ_ENUM_UINT_BITWISE_OPERATORS(Type) YULDUZ_ENUM_CLASS_BITWISE_OPERATORS(Type, std::uint32_t)
