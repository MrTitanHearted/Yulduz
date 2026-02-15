#include <Yulduz/Engine/EntityRegistry.h>

void YULDUZ_EnsureSparseCapacityInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity new_entity);
void YULDUZ_EnsureFreeListCapacityInEntityRegistry(YULDUZ_EntityRegistry *registry);

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

bool YULDUZ_CreateEntityInEntityRegistry(
    YULDUZ_EntityRegistry *registry,
    YULDUZ_ArchetypeType archetype_type, YULDUZ_ArchetypeIndex archetype_index,
    YULDUZ_Entity *entity) {
    YULDUZ_EnsureSparseCapacityInEntityRegistry(registry, registry->NextEntity);

    YULDUZ_Entity new_entity;
    if (registry->FreeListCount > 0) {
        new_entity = registry->FreeList[registry->FreeListCount - 1];
        registry->FreeListCount--;
    } else {
        new_entity = registry->NextEntity;
        registry->NextEntity++;
    }

    registry->Sparse[new_entity] = (YULDUZ_EntityRecord){
        .ArchetypeType  = archetype_type,
        .ArchetypeIndex = archetype_index,
    };

    *entity = new_entity;

    return true;
}

bool YULDUZ_DestroyEntityInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity) {
    if (entity >= registry->SparseCapacity ||
        YULDUZ_INVALID_ARCHETYPE_TYPE == registry->Sparse[entity].ArchetypeType ||
        YULDUZ_INVALID_ARCHETYPE_INDEX == registry->Sparse[entity].ArchetypeIndex) {
        return false;
    }

    YULDUZ_EnsureFreeListCapacityInEntityRegistry(registry);

    uint32_t free_list_index = registry->FreeListCount;
    registry->FreeListCount++;

    registry->FreeList[free_list_index] = entity;

    registry->Sparse[entity] = YULDUZ_INVALID_ENTITY_RECORD;

    return true;
}

bool YULDUZ_GetEntityRecordsInEntityRegistry(
    const YULDUZ_EntityRegistry *registry, const YULDUZ_Entity *entities, YULDUZ_EntityRecord *records, uint32_t count) {
    bool found_all = true;

    for (uint32_t i = 0; i < count; i++) {
        if (entities[i] >= registry->SparseCapacity ||
            YULDUZ_INVALID_ARCHETYPE_TYPE == registry->Sparse[entities[i]].ArchetypeType ||
            YULDUZ_INVALID_ARCHETYPE_INDEX == registry->Sparse[entities[i]].ArchetypeIndex) {
            records[i] = YULDUZ_INVALID_ENTITY_RECORD;
            found_all  = false;
            continue;
        }

        records[i] = registry->Sparse[entities[i]];
    }

    return found_all;
}

bool YULDUZ_SetEntityRecordsInEntityRegistry(
    const YULDUZ_EntityRegistry *registry, const YULDUZ_Entity *entities, const YULDUZ_EntityRecord *records, uint32_t count) {
    bool found_all = true;

    for (uint32_t i = 0; i < count; i++) {
        if (entities[i] >= registry->SparseCapacity ||
            YULDUZ_INVALID_ARCHETYPE_TYPE == registry->Sparse[entities[i]].ArchetypeType ||
            YULDUZ_INVALID_ARCHETYPE_INDEX == registry->Sparse[entities[i]].ArchetypeIndex) {
            found_all = false;
            continue;
        }

        registry->Sparse[entities[i]] = records[i];
    }

    return found_all;
}

void YULDUZ_EnsureSparseCapacityInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity new_entity) {
    if (new_entity < registry->SparseCapacity) {
        return;
    }

    uint32_t old_capacity = registry->SparseCapacity;
    uint32_t new_capacity = old_capacity * 2;
    while (new_capacity <= new_entity)
        new_capacity *= 2;

    YULDUZ_EntityRecord *new_sparse = SDL_realloc(registry->Sparse, sizeof(YULDUZ_EntityRecord) * new_capacity);

    SDL_memset(&new_sparse[old_capacity], 0xFF, sizeof(YULDUZ_EntityRecord) * (new_capacity - old_capacity));

    registry->SparseCapacity = new_capacity;
    registry->Sparse         = new_sparse;
}

void YULDUZ_EnsureFreeListCapacityInEntityRegistry(YULDUZ_EntityRegistry *registry) {
    if (registry->FreeListCount < registry->FreeListCapacity) {
        return;
    }

    uint32_t old_capacity = registry->FreeListCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_Entity *new_free_list = SDL_realloc(registry->FreeList, sizeof(YULDUZ_Entity) * new_capacity);

    registry->FreeListCapacity = new_capacity;
    registry->FreeList         = new_free_list;
}
