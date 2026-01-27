#include <Yulduz/Common/API.hpp>

YULDUZ_API void* operator new[](const size_t size,
                     const char* /*name*/,
                     int /*flags*/,
                     unsigned /*debugFlags*/,
                     const char* /*file*/,
                     int /*line*/) {
    return ::operator new[](size);
}

YULDUZ_API void* operator new[](const size_t size,
                     const size_t alignment,
                     const size_t /*alignmentOffset*/,
                     const char* /*name*/,
                     int /*flags*/,
                     unsigned /*debugFlags*/,
                     const char* /*file*/,
                     int /*line*/) {
    return ::operator new[](size, static_cast<std::align_val_t>(alignment));
}