#include <Yulduz/Signal.h>
#include <Yulduz/Memory.h>

bool YULDUZ_SignalHasListener(const YULDUZ_Signal *signal, YULDUZ_SignalListener listener) {
    if (nullptr == signal)
        return false;
    if (listener >= signal->SparseCount)
        return false;

    Uint32 dense_index = signal->SparseIndices[listener];
    return dense_index < signal->DenseCount && signal->DenseIndices[dense_index] == listener;
}

bool YULDUZ_SignalAddListener(YULDUZ_Signal *signal, YULDUZ_SignalPFN callback_pfn, YULDUZ_SignalListener *listener) {
    if (nullptr == signal || nullptr == listener)
        return false;
    if (signal->DenseCount >= signal->Capacity)
        return false;

    YULDUZ_SignalListener id;

    if (signal->FreeListCount > 0) {
        signal->FreeListCount--;
        id = signal->FreeList[signal->FreeListCount];
    } else {
        id = signal->SparseCount;
        signal->SparseCount++;
    }

    Uint32 dense_index = signal->DenseCount;

    signal->SparseIndices[id]              = dense_index;
    signal->DenseIndices[dense_index]      = id;
    signal->DenseCallbackPFNs[dense_index] = callback_pfn;

    signal->DenseCount++;

    *listener = id;

    return true;
}

bool YULDUZ_SignalRemoveListener(YULDUZ_Signal *signal, YULDUZ_SignalListener listener) {
    if (!YULDUZ_SignalHasListener(signal, listener))
        return false;

    Uint32 dense_index      = signal->SparseIndices[listener];
    Uint32 last_dense_index = signal->DenseCount - 1;
    Uint32 last_listener    = signal->DenseIndices[last_dense_index];

    signal->DenseCallbackPFNs[dense_index] = signal->DenseCallbackPFNs[last_dense_index];
    signal->DenseIndices[dense_index]      = last_listener;
    signal->SparseIndices[last_listener]   = dense_index;

    signal->DenseCount--;

    signal->FreeList[signal->FreeListCount] = listener;
    signal->FreeListCount++;

    return true;
}

void YULDUZ_SignalEmit(const YULDUZ_Signal *signal, void *signal_data) {
    Uint32 listener_count = signal->DenseCount;
    for (Uint32 listener_index = 0; listener_index < listener_count; listener_index++) {
        signal->DenseCallbackPFNs[listener_index](signal->UserData, signal_data);
    }
}

YULDUZ_SignalMemoryLayout YULDUZ_SignalGetMemoryLayout(Uint32 capacity) {
    Uint64 offset = 0;

    Uint64 dense_callback_pfns_offset = offset;
    offset += sizeof(YULDUZ_SignalPFN) * capacity;

    Uint64 free_list_offset = offset;
    offset += sizeof(YULDUZ_SignalListener) * capacity;

    Uint64 sparse_offset = offset;
    offset += sizeof(Uint32) * capacity;

    Uint64 dense_indices_offset = offset;
    offset += sizeof(Uint32) * capacity;

    return (YULDUZ_SignalMemoryLayout){
        .SparseOffset            = sparse_offset,
        .DenseCallbackPFNsOffset = dense_callback_pfns_offset,
        .DenseIndicesOffset      = dense_indices_offset,
        .FreeListOffset          = free_list_offset,
        .TotalSize               = offset,
    };
}

Uint64 YULDUZ_SignalGetRequiredMemorySize(Uint32 capacity) {
    YULDUZ_SignalMemoryLayout layout = YULDUZ_SignalGetMemoryLayout(capacity);
    return layout.TotalSize;
}

YULDUZ_Signal YULDUZ_SignalMake(void *user_data, Uint8 *memory, Uint32 capacity) {
    SDL_assert(YULDUZ_MemoryIsAligned(memory, YULDUZ_SIGNAL_MEMORY_ALIGNMENT) && "Memory is not properly aligned!");

    YULDUZ_SignalMemoryLayout layout = YULDUZ_SignalGetMemoryLayout(capacity);

    YULDUZ_Signal signal = {0};
    SDL_zero(signal);

    signal.UserData = user_data;
    signal.Capacity = capacity;

    signal.SparseIndices     = (void *)&memory[layout.SparseOffset];
    signal.DenseCallbackPFNs = (void *)&memory[layout.DenseCallbackPFNsOffset];
    signal.DenseIndices      = (void *)&memory[layout.DenseIndicesOffset];
    signal.FreeList          = (void *)&memory[layout.FreeListOffset];

    return signal;
}