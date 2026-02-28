#include <Yulduz/Engine/Signal.h>

static void YULDUZ_EnsureSparseCapacityInSignal(YULDUZ_Signal *signal);
static void YULDUZ_EnsureDenseCapacityInSignal(YULDUZ_Signal *signal);
static void YULDUZ_EnsureFreeListCapacityInSignal(YULDUZ_Signal *signal);

bool YULDUZ_InitializeSignal(YULDUZ_Signal *signal, uint32_t initial_capacity) {
    SDL_zerop(signal);

    signal->SparseCapacity = initial_capacity;
    signal->SparseCount    = 0;
    signal->Sparse         = SDL_malloc(sizeof(uint64_t) * initial_capacity);
    SDL_memset(signal->Sparse, 0xFF, sizeof(uint64_t) * initial_capacity);

    signal->DenseCapacity = initial_capacity;
    signal->DenseCount    = 0;
    signal->Dense         = SDL_malloc(sizeof(YULDUZ_SignalListenerInfo) * initial_capacity);

    signal->FreeListCapacity = initial_capacity;
    signal->FreeListCount    = 0;
    signal->FreeList         = SDL_malloc(sizeof(uint32_t) * initial_capacity);

    return true;
}

void YULDUZ_ReleaseSignal(YULDUZ_Signal *signal) {
    SDL_free(signal->Sparse);
    SDL_free(signal->Dense);
    SDL_free(signal->FreeList);

    SDL_zerop(signal);
}

bool YULDUZ_HasListenerInSignal(const YULDUZ_Signal *signal, YULDUZ_SignalListener listener) {
    uint32_t sparse_generation = (uint32_t)(listener >> 32);
    uint32_t sparse_index      = (uint32_t)listener;

    if (sparse_index >= signal->SparseCount ||
        (uint32_t)signal->Sparse[sparse_index] >= signal->DenseCount ||
        sparse_generation != (uint32_t)(signal->Sparse[sparse_index] >> 32)) {
        return false;
    }

    return true;
}

bool YULDUZ_AddListenerInSignal(
    YULDUZ_Signal *signal, YULDUZ_SignalPFN callback_pfn, void *user_data, YULDUZ_SignalListener *listener) {
    uint32_t sparse_index;
    uint32_t sparse_generation;
    if (signal->FreeListCount > 0) {
        sparse_index = signal->FreeList[signal->FreeListCount - 1];
        signal->FreeListCount--;
        sparse_generation = (uint32_t)(signal->Sparse[sparse_index] >> 32);
    } else {
        YULDUZ_EnsureSparseCapacityInSignal(signal);
        sparse_index = signal->SparseCount;
        signal->SparseCount++;
        sparse_generation = 0x00000000;
    }

    YULDUZ_EnsureDenseCapacityInSignal(signal);

    uint32_t dense_index = signal->DenseCount;
    signal->DenseCount++;

    signal->Sparse[sparse_index] = ((uint64_t)sparse_generation << 32) | (uint64_t)dense_index;

    signal->Dense[dense_index] = (YULDUZ_SignalListenerInfo){
        .CallbackPFN      = callback_pfn,
        .UserData         = user_data,
        .SparseIndex      = sparse_index,
        .SparseGeneration = sparse_generation,
    };

    *listener = ((uint64_t)sparse_generation << 32) | (uint64_t)sparse_index;

    return true;
}

bool YULDUZ_RemoveListenerInSignal(YULDUZ_Signal *signal, YULDUZ_SignalListener listener) {
    uint32_t sparse_generation = (uint32_t)(listener >> 32);
    uint32_t sparse_index      = (uint32_t)listener;

    if (sparse_index >= signal->SparseCount ||
        (uint32_t)signal->Sparse[sparse_index] >= signal->DenseCount ||
        sparse_generation != (uint32_t)(signal->Sparse[sparse_index] >> 32)) {
        return false;
    }

    uint32_t dense_index      = (uint32_t)signal->Sparse[sparse_index];
    uint32_t dense_last_index = signal->DenseCount - 1;
    signal->DenseCount--;

    if (dense_index != dense_last_index) {
        uint32_t last_sparse_index = signal->Dense[dense_last_index].SparseIndex;
        signal->Sparse[last_sparse_index] =
            (0xFFFFFFFF00000000 & signal->Sparse[last_sparse_index]) | (uint64_t)dense_index;
        signal->Dense[dense_index] = signal->Dense[dense_last_index];
    }

    if (sparse_generation == 0xFFFFFFFF)
        sparse_generation = 0;
    else
        sparse_generation++;

    signal->Sparse[sparse_index] = ((uint64_t)sparse_generation << 32) | 0x00000000FFFFFFFF;

    YULDUZ_EnsureFreeListCapacityInSignal(signal);

    uint32_t free_list_index = signal->FreeListCount;
    signal->FreeListCount++;

    signal->FreeList[free_list_index] = sparse_index;

    return true;
}

void YULDUZ_EmitSignal(const YULDUZ_Signal *signal, const void *signal_data) {
    uint32_t dense_count = signal->DenseCount;
    for (uint32_t i = 0; i < dense_count; i++) {
        const YULDUZ_SignalListenerInfo *info = &signal->Dense[i];

        (*info->CallbackPFN)(signal_data, info->UserData);
    }
}

void YULDUZ_EnsureSparseCapacityInSignal(YULDUZ_Signal *signal) {
    if (signal->SparseCount < signal->SparseCapacity)
        return;

    uint32_t old_capacity = signal->SparseCapacity;
    uint32_t new_capacity = old_capacity * 2;

    uint64_t *new_sparse = SDL_realloc(signal->Sparse, sizeof(uint64_t) * new_capacity);

    SDL_memset(&new_sparse[old_capacity], 0xFF, sizeof(uint64_t) * (new_capacity - old_capacity));

    signal->SparseCapacity = new_capacity;
    signal->Sparse         = new_sparse;
}

void YULDUZ_EnsureDenseCapacityInSignal(YULDUZ_Signal *signal) {
    if (signal->DenseCount < signal->DenseCapacity)
        return;

    uint32_t old_capacity = signal->DenseCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_SignalListenerInfo *new_dense =
        SDL_realloc(signal->Dense, sizeof(YULDUZ_SignalListenerInfo) * new_capacity);

    signal->DenseCapacity = new_capacity;
    signal->Dense         = new_dense;
}

void YULDUZ_EnsureFreeListCapacityInSignal(YULDUZ_Signal *signal) {
    if (signal->FreeListCount < signal->FreeListCapacity)
        return;

    uint32_t old_capacity = signal->FreeListCapacity;
    uint32_t new_capacity = old_capacity * 2;

    uint32_t *new_free_list = SDL_realloc(signal->FreeList, sizeof(uint32_t) * new_capacity);

    signal->FreeListCapacity = new_capacity;
    signal->FreeList         = new_free_list;
}