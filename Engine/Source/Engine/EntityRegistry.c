#include <Yulduz/Engine/EntityRegistry.h>

static void YULDUZ_EnsureSparseCapacityInEntityRegistry(YULDUZ_EntityRegistry *registry);
static void YULDUZ_EnsureFreeListCapacityInEntityRegistry(YULDUZ_EntityRegistry *registry);

bool YULDUZ_InitializeEntityRegistry(YULDUZ_EntityRegistry *registry, uint32_t initial_capacity) {
    SDL_zerop(registry);

    registry->SparseCapacity    = initial_capacity;
    registry->SparseCount       = 0;
    registry->Sparse            = SDL_malloc(sizeof(YULDUZ_EntityRecord) * initial_capacity);
    registry->SparseGenerations = SDL_malloc(sizeof(uint32_t) * initial_capacity);
    SDL_memset(registry->Sparse, 0xFF, sizeof(YULDUZ_EntityRecord) * initial_capacity);
    SDL_memset(registry->SparseGenerations, 0xFF, sizeof(uint32_t) * initial_capacity);

    registry->FreeListCapacity = initial_capacity;
    registry->FreeListCount    = 0;
    registry->FreeList         = SDL_malloc(sizeof(YULDUZ_Entity) * initial_capacity);

    registry->EntityCount = 0;

    return true;
}

void YULDUZ_ReleaseEntityRegistry(YULDUZ_EntityRegistry *registry) {
    SDL_free(registry->Sparse);
    SDL_free(registry->FreeList);

    SDL_zerop(registry);
}

bool YULDUZ_CreateEntityInEntityRegistry(
    YULDUZ_EntityRegistry *registry, YULDUZ_EntityRecord record, YULDUZ_Entity *entity) {
    uint32_t entity_index;
    if (registry->FreeListCount > 0) {
        entity_index = registry->FreeList[registry->FreeListCount - 1];
        registry->FreeListCount--;
    } else {
        YULDUZ_EnsureSparseCapacityInEntityRegistry(registry);
        entity_index = registry->SparseCount;
        registry->SparseCount++;
        registry->SparseGenerations[entity_index] = 0;
    }

    registry->Sparse[entity_index] = record;

    *entity = ((uint64_t)registry->SparseGenerations[entity_index] << 32) | entity_index;

    registry->EntityCount++;

    return true;
}

bool YULDUZ_DestroyEntityInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity) {
    uint32_t entity_index      = (uint32_t)entity;
    uint32_t entity_generation = (uint32_t)(entity >> 32);

    if (entity_index >= registry->SparseCapacity ||
        entity_generation != registry->SparseGenerations[entity_index] ||
        YULDUZ_INVALID_ARCHETYPE_TYPE == registry->Sparse[entity_index].ArchetypeType ||
        YULDUZ_INVALID_ARCHETYPE_INDEX == registry->Sparse[entity_index].ArchetypeIndex) {
        return false;
    }

    YULDUZ_EnsureFreeListCapacityInEntityRegistry(registry);

    uint32_t free_list_index = registry->FreeListCount;
    registry->FreeListCount++;

    registry->FreeList[free_list_index] = entity_index;

    registry->Sparse[entity_index] = YULDUZ_INVALID_ENTITY_RECORD;
    if (0xFFFFFFFF == registry->SparseGenerations[entity_index])
        registry->SparseGenerations[entity_index] = 0;
    else
        registry->SparseGenerations[entity_index]++;

    registry->EntityCount--;

    return true;
}

void YULDUZ_DestroyEntityUnsafeInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity) {
    uint32_t entity_index = (uint32_t)entity;

    YULDUZ_EnsureFreeListCapacityInEntityRegistry(registry);

    uint32_t free_list_index = registry->FreeListCount;
    registry->FreeListCount++;

    registry->FreeList[free_list_index] = entity_index;

    registry->Sparse[entity_index] = YULDUZ_INVALID_ENTITY_RECORD;
    if (0xFFFFFFFF == registry->SparseGenerations[entity_index])
        registry->SparseGenerations[entity_index] = 0;
    else
        registry->SparseGenerations[entity_index]++;

    registry->EntityCount--;
}

bool YULDUZ_HasEntityInEntityRegistry(YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity) {
    uint32_t entity_index      = (uint32_t)entity;
    uint32_t entity_generation = (uint32_t)(entity >> 32);

    if (entity_index >= registry->SparseCapacity ||
        entity_generation != registry->SparseGenerations[entity_index] ||
        YULDUZ_INVALID_ARCHETYPE_TYPE == registry->Sparse[entity_index].ArchetypeType ||
        YULDUZ_INVALID_ARCHETYPE_INDEX == registry->Sparse[entity_index].ArchetypeIndex) {
        return false;
    }

    return true;
}

uint32_t YULDUZ_GetEntityCountInEntityRegistry(const YULDUZ_EntityRegistry *registry) {
    return registry->EntityCount;
}

bool YULDUZ_GetEntityRecordInEntityRegistry(
    const YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity, YULDUZ_EntityRecord *record) {
    uint32_t entity_index      = (uint32_t)entity;
    uint32_t entity_generation = (uint32_t)(entity >> 32);

    if (entity_index >= registry->SparseCapacity ||
        entity_generation != registry->SparseGenerations[entity_index] ||
        YULDUZ_INVALID_ARCHETYPE_TYPE == registry->Sparse[entity_index].ArchetypeType ||
        YULDUZ_INVALID_ARCHETYPE_INDEX == registry->Sparse[entity_index].ArchetypeIndex) {
        *record = YULDUZ_INVALID_ENTITY_RECORD;
        return false;
    }

    *record = registry->Sparse[entity_index];
    return true;
}

bool YULDUZ_SetEntityRecordInEntityRegistry(
    const YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity, YULDUZ_EntityRecord record) {
    uint32_t entity_index      = (uint32_t)entity;
    uint32_t entity_generation = (uint32_t)(entity >> 32);

    if (entity_index >= registry->SparseCapacity ||
        entity_generation != registry->SparseGenerations[entity_index] ||
        YULDUZ_INVALID_ARCHETYPE_TYPE == registry->Sparse[entity_index].ArchetypeType ||
        YULDUZ_INVALID_ARCHETYPE_INDEX == registry->Sparse[entity_index].ArchetypeIndex) {
        return false;
    }

    registry->Sparse[entity_index] = record;
    return true;
}

void YULDUZ_GetEntityRecordUnsafeInEntityRegistry(
    const YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity, YULDUZ_EntityRecord *record) {
    *record = registry->Sparse[(uint32_t)entity];
}

void YULDUZ_SetEntityRecordUnsafeInEntityRegistry(
    const YULDUZ_EntityRegistry *registry, YULDUZ_Entity entity, YULDUZ_EntityRecord record) {
    registry->Sparse[(uint32_t)entity] = record;
}

void YULDUZ_EnsureSparseCapacityInEntityRegistry(YULDUZ_EntityRegistry *registry) {
    if (registry->SparseCount < registry->SparseCapacity) {
        return;
    }

    uint32_t old_capacity = registry->SparseCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_EntityRecord *new_sparse = SDL_realloc(registry->Sparse, sizeof(YULDUZ_EntityRecord) * new_capacity);

    uint32_t *new_sparse_generations = SDL_realloc(registry->SparseGenerations, sizeof(uint32_t) * new_capacity);

    SDL_memset(&new_sparse[old_capacity], 0xFF, sizeof(YULDUZ_EntityRecord) * (new_capacity - old_capacity));
    SDL_memset(&new_sparse_generations[old_capacity], 0xFF, sizeof(uint32_t) * (new_capacity - old_capacity));

    registry->SparseCapacity    = new_capacity;
    registry->Sparse            = new_sparse;
    registry->SparseGenerations = new_sparse_generations;
}

void YULDUZ_EnsureFreeListCapacityInEntityRegistry(YULDUZ_EntityRegistry *registry) {
    if (registry->FreeListCount < registry->FreeListCapacity) {
        return;
    }

    uint32_t old_capacity = registry->FreeListCapacity;
    uint32_t new_capacity = old_capacity * 2;

    uint32_t *new_free_list = SDL_realloc(registry->FreeList, sizeof(YULDUZ_Entity) * new_capacity);

    registry->FreeListCapacity = new_capacity;
    registry->FreeList         = new_free_list;
}
