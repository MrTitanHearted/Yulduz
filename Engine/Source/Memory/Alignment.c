#include <Yulduz/Memory/Alignment.h>

Uint64 YULDUZ_AddressAlignUp(Uint64 offset, Uint64 alignment) {
    Uint64 mask = alignment - 1;
    return (offset + mask) & ~mask;
}

Uint8 *YULDUZ_MemoryAlignUp(const Uint8 *memory, Uint64 alignment) {
    Uint64 mask    = alignment - 1;
    Uint64 offset  = (Uint64)memory;
    Uint64 aligned = (offset + mask) & ~mask;
    return (Uint8 *)aligned;
}

bool YULDUZ_AddressIsAligned(Uint64 offset, Uint64 alignment) {
    Uint64 mask = alignment - 1;
    return (offset & mask) == 0;
}

bool YULDUZ_MemoryIsAligned(const Uint8 *memory, Uint64 alignment) {
    Uint64 offset = (Uint64)memory;
    Uint64 mask   = alignment - 1;
    return (offset & mask) == 0;
}