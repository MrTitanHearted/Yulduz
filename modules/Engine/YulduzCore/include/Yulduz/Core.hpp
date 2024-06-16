#pragma once

#include <YulduzCore/YulduzPCH.hpp>
#include <YulduzCore/Timer.hpp>
#include <YulduzCore/Logger.hpp>
#include <YulduzCore/Events.hpp>

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

#define YULDUZ_ENUM_UINT_BITWISE_OPERATORS(Type) YULDUZ_ENUM_CLASS_BITWISE_OPERATORS(Type, uint32_t)

namespace Yulduz {
    void TestYulduzCore();
}  // namespace Yulduz
