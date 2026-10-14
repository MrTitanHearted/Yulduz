#pragma once

#include <Yulduz/PCH.h>

typedef void (*YULDUZ_SignalPFN)(void *user_data, void *signal_data);

typedef Uint32 YULDUZ_SignalListener;

typedef struct YULDUZ_SignalMemoryLayout YULDUZ_SignalMemoryLayout;
typedef struct YULDUZ_Signal             YULDUZ_Signal;

struct YULDUZ_SignalMemoryLayout {
    Uint64 SparseOffset;
    Uint64 DenseCallbackPFNsOffset;
    Uint64 DenseIndicesOffset;
    Uint64 FreeListOffset;
    Uint64 TotalSize;
};

struct YULDUZ_Signal {
    void *UserData;

    Uint32 *SparseIndices;

    YULDUZ_SignalPFN *DenseCallbackPFNs;
    Uint32           *DenseIndices;

    YULDUZ_SignalListener *FreeList;

    Uint32 Capacity;

    Uint32 SparseCount;
    Uint32 DenseCount;
    Uint32 FreeListCount;
};

constexpr Uint32 YULDUZ_SIGNAL_MEMORY_ALIGNMENT = alignof(YULDUZ_SignalPFN);

constexpr YULDUZ_SignalListener YULDUZ_INVALID_SIGNAL_LISTENER = 0xFFFFFFFF;

YULDUZ_API bool YULDUZ_SignalHasListener(const YULDUZ_Signal *signal, YULDUZ_SignalListener listener);

YULDUZ_API bool YULDUZ_SignalAddListener(YULDUZ_Signal *signal, YULDUZ_SignalPFN callback_pfn, YULDUZ_SignalListener *listener);
YULDUZ_API bool YULDUZ_SignalRemoveListener(YULDUZ_Signal *signal, YULDUZ_SignalListener listener);

YULDUZ_API void YULDUZ_SignalEmit(const YULDUZ_Signal *signal, void *signal_data);

YULDUZ_API YULDUZ_SignalMemoryLayout YULDUZ_SignalGetMemoryLayout(Uint32 capacity);

YULDUZ_API Uint64 YULDUZ_SignalGetRequiredMemorySize(Uint32 capacity);
// memory must be algined up to YULDUZ_SIGNAL_MEMORY_ALIGNMENT and
// have at least YULDUZ_SignalGetRequiredMemorySize(capacity) bytes!!!
YULDUZ_API YULDUZ_Signal YULDUZ_SignalMake(void *user_data, Uint8 *memory, Uint32 capacity);
