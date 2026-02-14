#include <Yulduz/Engine/System.h>

bool YULDUZ_InitializeSystem(
    YULDUZ_System *system, const char *name, const YULDUZ_Query *query, YULDUZ_SystemPFN system_pfn) {
    SDL_zerop(system);

    system->Name = SDL_strdup(name);
    if (!YULDUZ_DeepCopyQuery(query, &system->Query)) {
        return false;
    }
    system->SystemPFN = system_pfn;
    return true;
}

void YULDUZ_ReleaseSystem(YULDUZ_System *system) {
    YULDUZ_ReleaseQuery(&system->Query);

    SDL_free(system->Name);

    SDL_zerop(system);
}

bool YULDUZ_RunSystem(YULDUZ_System *system, YULDUZ_ECSRegistry *registry, void *user_data) {
    uint32_t archetype_count;

    YULDUZ_ArchetypeType *archetype_types = YULDUZ_QueryArchetypesInECSRegistry(
        registry,
        system->Query.SortedRequiredComponentTypes, system->Query.RequiredComponentCount,
        system->Query.SortedRequiredTagTypes, system->Query.RequiredTagCount,
        &archetype_count);

    if (0 == archetype_count) {
        return true;
    }

    for (uint32_t i = 0; i < archetype_count; i++) {
        YULDUZ_Archetype *archetype = YULDUZ_GetArchetypeInECSRegistry(registry, archetype_types[i]);
        (system->SystemPFN)(archetype, &system->Query, user_data);
    }

    SDL_free(archetype_types);
    return true;
}