#include <Yulduz/Engine/EntityRegistry.h>

bool YULDUZ_EnsureSparseCapacityInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity new_entity);
bool YULDUZ_EnsureFreeListCapacityInEntityRegistry(YULDUZ_EntityRegistry *registry);

bool YULDUZ_InitializeEntityRegistry(YULDUZ_EntityRegistry *registry, uint32_t initial_capacity) {
    SDL_zerop(registry);

    registry->SparseCapacity = initial_capacity;
    registry->Sparse         = SDL_malloc(sizeof(YULDUZ_EntityRecord) * initial_capacity);
    SDL_memset(registry->Sparse, 0x00, sizeof(YULDUZ_EntityRecord) * initial_capacity);

    registry->FreeListCapacity = initial_capacity;
    registry->FreeListCount    = 0;
    registry->FreeList         = SDL_malloc(sizeof(YULDUZ_Entity) * initial_capacity);

    registry->NextEntity = 0;

    return true;
}

void YULDUZ_ReleaseEntityRegistry(YULDUZ_EntityRegistry *registry) {
    SDL_free(registry->Sparse);
    SDL_free(registry->FreeList);

    SDL_zerop(registry);
}

bool YULDUZ_CreateEntityInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity *entity) {
    if (!YULDUZ_EnsureSparseCapacityInEntityRegistry(registry, registry->NextEntity)) {
        return false;
    }

    YULDUZ_Entity new_entity;
    if (registry->FreeListCount > 0) {
        new_entity = registry->FreeList[registry->FreeListCount - 1];
        registry->FreeListCount--;
    } else {
        new_entity = registry->NextEntity;
        registry->NextEntity++;
    }

    registry->Sparse[new_entity] = (YULDUZ_EntityRecord){
        .ArchetypeType  = YULDUZ_INVALID_ARCHETYPE_TYPE,
        .ArchetypeIndex = YULDUZ_INVALID_ARCHETYPE_INDEX,
        .Alive          = true,
    };

    *entity = new_entity;

    return true;
}

bool YULDUZ_DestroyEntityInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity) {
    if (entity >= registry->SparseCapacity ||
        !registry->Sparse[entity].Alive) {
        SDL_SetError("[YULDUZ]: Entity %u doesn't exist or destroyed", entity);
        return false;
    }

    uint32_t free_list_index = registry->FreeListCount;
    registry->FreeListCount++;

    if (!YULDUZ_EnsureFreeListCapacityInEntityRegistry(registry)) {
        registry->FreeListCount--;
        return false;
    }

    registry->FreeList[free_list_index] = entity;

    registry->Sparse[entity] = YULDUZ_NULL_ENTITY_RECORD;

    return true;
}

bool YULDUZ_GetEntityRecordsInEntityRegistry(
    YULDUZ_EntityRegistry *registry, const YULDUZ_Entity *entities, YULDUZ_EntityRecord *records, uint32_t count) {
    bool found_all = true;

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_Entity entity = entities[i];

        YULDUZ_EntityRecord *record = &records[i];

        if (entity >= registry->SparseCapacity ||
            !registry->Sparse[entity].Alive) {
            *record   = YULDUZ_NULL_ENTITY_RECORD;
            found_all = false;
            continue;
        }

        *record = registry->Sparse[entity];
    }

    return found_all;
}

bool YULDUZ_SetEntityRecordsInEntityRegistry(
    YULDUZ_EntityRegistry *registry, const YULDUZ_Entity *entities, const YULDUZ_EntityRecord *records, uint32_t count) {
    bool found_all = true;

    for (uint32_t i = 0; i < count; i++) {
        YULDUZ_Entity entity = entities[i];

        if (entity >= registry->SparseCapacity ||
            !registry->Sparse[entity].Alive) {
            found_all = false;
            continue;
        }

        registry->Sparse[entity]       = records[i];
        registry->Sparse[entity].Alive = true;
    }

    return found_all;
}

bool YULDUZ_EnsureSparseCapacityInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity new_entity) {
    if (new_entity < registry->SparseCapacity) {
        return true;
    }

    uint32_t old_capacity = registry->SparseCapacity;
    uint32_t new_capacity = old_capacity * 2;
    while (new_capacity <= new_entity)
        new_capacity *= 2;

    YULDUZ_EntityRecord *new_sparse = SDL_realloc(registry->Sparse, sizeof(YULDUZ_EntityRecord) * new_capacity);
    if (nullptr == new_sparse) {
        return false;
    }

    SDL_memset(&new_sparse[old_capacity], 0x00, sizeof(YULDUZ_EntityRecord) * (new_capacity - old_capacity));

    registry->SparseCapacity = new_capacity;
    registry->Sparse         = new_sparse;

    return true;
}

bool YULDUZ_EnsureFreeListCapacityInEntityRegistry(YULDUZ_EntityRegistry *registry) {
    if (registry->FreeListCount < registry->FreeListCapacity) {
        return true;
    }

    uint32_t old_capacity = registry->FreeListCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_Entity *new_free_list = SDL_realloc(registry->FreeList, sizeof(YULDUZ_Entity) * new_capacity);
    if (nullptr == new_free_list) {
        return false;
    }

    registry->FreeListCapacity = new_capacity;
    registry->FreeList         = new_free_list;

    return true;
}
