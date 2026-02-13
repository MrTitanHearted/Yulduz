#include <Yulduz/Engine/ComponentStore.h>

bool YULDUZ_InitializeComponentStore(
    YULDUZ_ComponentStore *store, YULDUZ_TypeInfo type_description, uint32_t initial_capacity) {
    SDL_zerop(store);

    store->Type          = type_description.Type;
    store->TypeSize      = type_description.Size;
    store->TypeAlignment = type_description.Alignment;
    store->Dense         = SDL_aligned_alloc(type_description.Alignment, type_description.Size * initial_capacity);

    return true;
}

void YULDUZ_ReleaseComponentStore(YULDUZ_ComponentStore *store) {
    SDL_aligned_free(store->Dense);

    SDL_zerop(store);
}

bool YULDUZ_ReallocateComponentStore(YULDUZ_ComponentStore *store, uint32_t old_capacity, uint32_t new_capacity) {
    uint8_t *old_dense = store->Dense;
    uint8_t *new_dense = SDL_aligned_alloc(store->TypeAlignment, store->TypeSize * new_capacity);
    SDL_memcpy(new_dense, old_dense, store->TypeSize * old_capacity);
    SDL_aligned_free(old_dense);
    store->Dense = new_dense;
    return true;
}

void YULDUZ_CopyBackComponentInComponentStore(
    YULDUZ_ComponentStore *store, YULDUZ_ComponentIndex index, YULDUZ_ComponentIndex back_index) {
    uint32_t byte_index      = store->TypeSize * index;
    uint32_t byte_back_index = store->TypeSize * back_index;
    SDL_memcpy(&store->Dense[byte_index], &store->Dense[byte_back_index], store->TypeSize);
}

void *YULDUZ_GetComponentInComponentStore(YULDUZ_ComponentStore *store, YULDUZ_ComponentIndex index) {
    return &store->Dense[store->TypeSize * index];
}
