#pragma once

#include <Yulduz/Common.h>

typedef void (*YULDUZ_SignalPFN)(const void *event_data, void *user_data);

typedef uint64_t YULDUZ_SignalListener;

typedef struct YULDUZ_SignalListenerInfo YULDUZ_SignalListenerInfo;
typedef struct YULDUZ_Signal             YULDUZ_Signal;

struct YULDUZ_SignalListenerInfo {
    YULDUZ_SignalPFN CallbackPFN;

    void *UserData;

    uint32_t SparseIndex;
    uint32_t SparseGeneration;
};

struct YULDUZ_Signal {
    uint64_t *Sparse;

    YULDUZ_SignalListenerInfo *Dense;

    uint32_t *FreeList;

    uint32_t SparseCapacity;
    uint32_t SparseCount;

    uint32_t DenseCapacity;
    uint32_t DenseCount;

    uint32_t FreeListCapacity;
    uint32_t FreeListCount;
};

constexpr YULDUZ_SignalListener YULDUZ_INVALID_SIGNAL_LISTENER = 0xFFFFFFFFFFFFFFFF;

constexpr YULDUZ_SignalListenerInfo YULDUZ_NULL_SIGNAL_LISTENER_INFO = (YULDUZ_SignalListenerInfo){
    .CallbackPFN      = nullptr,
    .UserData         = nullptr,
    .SparseIndex      = 0xFFFFFFFF,
    .SparseGeneration = 0xFFFFFFFF,
};

YULDUZ_API bool YULDUZ_InitializeSignal(YULDUZ_Signal *signal, uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseSignal(YULDUZ_Signal *signal);

YULDUZ_API bool YULDUZ_HasListenerInSignal(const YULDUZ_Signal *signal, YULDUZ_SignalListener listener);

YULDUZ_API bool YULDUZ_AddListenerInSignal(
    YULDUZ_Signal *signal, YULDUZ_SignalPFN callback_pfn, void *user_data, YULDUZ_SignalListener *listener);
YULDUZ_API bool YULDUZ_RemoveListenerInSignal(YULDUZ_Signal *signal, YULDUZ_SignalListener listener);

YULDUZ_API void YULDUZ_EmitSignal(const YULDUZ_Signal *signal, const void *signal_data);