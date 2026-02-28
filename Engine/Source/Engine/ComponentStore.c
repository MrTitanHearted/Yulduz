#include <Yulduz/Engine/ComponentStore.h>

bool YULDUZ_InitializeComponentStore(
    YULDUZ_ComponentStore *store, YULDUZ_ComponentTypeInfo type_info, uint32_t initial_capacity) {
    SDL_zerop(store);

    store->TypeAlignment = type_info.Alignment;
    store->TypeSize      = type_info.Size;
    store->Type          = type_info.Type;
    store->Dense         = SDL_aligned_alloc(type_info.Alignment, type_info.Size * initial_capacity);

    return true;
}

void YULDUZ_ReleaseComponentStore(YULDUZ_ComponentStore *store) {
    SDL_aligned_free(store->Dense);

    SDL_zerop(store);
}

void YULDUZ_ReallocateComponentStore(YULDUZ_ComponentStore *store, uint32_t old_capacity, uint32_t new_capacity) {
    uint8_t *old_dense = store->Dense;
    uint8_t *new_dense = SDL_aligned_alloc(store->TypeAlignment, store->TypeSize * new_capacity);
    SDL_memcpy(new_dense, old_dense, store->TypeSize * old_capacity);
    SDL_aligned_free(old_dense);
    store->Dense = new_dense;
}

void YULDUZ_CopyComponentInComponentStore(
    const YULDUZ_ComponentStore *store, YULDUZ_ComponentIndex src_index, YULDUZ_ComponentIndex dst_index) {
    uint32_t src_byte_index = store->TypeSize * src_index;
    uint32_t dst_byte_index = store->TypeSize * dst_index;
    SDL_memcpy(&store->Dense[src_byte_index], &store->Dense[dst_byte_index], store->TypeSize);
}

void *YULDUZ_GetComponentInComponentStore(const YULDUZ_ComponentStore *store, YULDUZ_ComponentIndex index) {
    return &store->Dense[store->TypeSize * index];
}
