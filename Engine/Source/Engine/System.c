#include <Yulduz/Engine/System.h>

bool YULDUZ_InitializeSystem(
    YULDUZ_System *system, const char *name, const YULDUZ_Query *query, YULDUZ_SystemPFN system_pfn) {
    SDL_zerop(system);

    system->Name = SDL_strdup(name);
    if (!YULDUZ_CreateQueryInfo(query, &system->Query)) {
        return false;
    }
    system->SystemPFN = system_pfn;
    return true;
}

void YULDUZ_ReleaseSystem(YULDUZ_System *system) {
    YULDUZ_DestroyQueryInfo(&system->Query);

    SDL_free(system->Name);

    SDL_zerop(system);
}

void YULDUZ_RunSystem(YULDUZ_System *system, YULDUZ_ECSRegistry *registry, void *user_data) {
    uint32_t archetype_count;

    YULDUZ_GetArchetypeCountInECSRegistry(registry, &archetype_count);

    for (uint32_t i = 0; i < archetype_count; i++) {
        YULDUZ_Archetype *archetype = YULDUZ_GetArchetypeInECSRegistry(registry, i);
        if (YULDUZ_ArchetypeSupportsQueryInfo(&system->Query, archetype)) {
            (system->SystemPFN)(archetype, &system->Query, user_data);
        }
    }
}