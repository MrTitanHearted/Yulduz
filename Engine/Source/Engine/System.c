#include <Yulduz/Engine/System.h>

void YULDUZ_EnsureDenseCapacityInSystem(YULDUZ_System *system);

bool YULDUZ_InitializeSystem(
    YULDUZ_System *system, const char *name, const YULDUZ_Query *query, YULDUZ_SystemPFN system_pfn) {
    SDL_zerop(system);

    system->Name = SDL_strdup(name);
    if (!YULDUZ_CreateQueryInfo(&system->Query, query)) {
        return false;
    }
    system->SystemPFN = system_pfn;

    system->DenseCapacity = 1;
    system->DenseCount    = 0;
    system->Dense         = SDL_malloc(sizeof(YULDUZ_ArchetypeType));

    system->LastArchetypeCount = 0;
    return true;
}

void YULDUZ_ReleaseSystem(YULDUZ_System *system) {
    YULDUZ_DestroyQueryInfo(&system->Query);

    SDL_free(system->Name);
    SDL_free(system->Dense);

    SDL_zerop(system);
}

void YULDUZ_RunSystem(YULDUZ_System *system, const YULDUZ_ECSRegistry *registry, void *user_data) {
    uint32_t archetype_count;

    YULDUZ_GetArchetypeCountInECSRegistry(registry, &archetype_count);

    if (archetype_count > system->LastArchetypeCount) {
        for (YULDUZ_ArchetypeType archetype_type = system->LastArchetypeCount;
             archetype_type < archetype_count;
             archetype_type++) {
            if (!YULDUZ_ArchetypeSupportsQueryInfo(
                    &system->Query, YULDUZ_GetArchetypeInECSRegistry(registry, archetype_type))) {
                continue;
            }

            YULDUZ_EnsureDenseCapacityInSystem(system);

            uint32_t index = system->DenseCount;
            system->DenseCount++;
            system->Dense[index] = archetype_type;
        }

        system->LastArchetypeCount = archetype_count;
    }

    for (uint32_t i = 0; i < system->DenseCount; i++) {
        (system->SystemPFN)(YULDUZ_GetArchetypeInECSRegistry(registry, system->Dense[i]), &system->Query, user_data);
    }
}

void YULDUZ_EnsureDenseCapacityInSystem(YULDUZ_System *system) {
    if (system->DenseCount < system->DenseCapacity) {
        return;
    }

    uint32_t old_capacity = system->DenseCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_ArchetypeType *new_dense = SDL_realloc(system->Dense, sizeof(YULDUZ_ArchetypeType) * new_capacity);

    system->DenseCapacity = new_capacity;
    system->Dense         = new_dense;
}