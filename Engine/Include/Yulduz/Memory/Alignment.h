#pragma once

#include <Yulduz/PCH.h>

YULDUZ_API Uint64 YULDUZ_AddressAlignUp(Uint64 offset, Uint64 alignment);
YULDUZ_API Uint8 *YULDUZ_MemoryAlignUp(const Uint8 *memory, Uint64 alignment);

YULDUZ_API bool YULDUZ_AddressIsAligned(Uint64 offset, Uint64 alignment);
YULDUZ_API bool YULDUZ_MemoryIsAligned(const Uint8 *memory, Uint64 alignment);
