#pragma once

#include <Yulduz/Engine/Archetype.h>
#include <Yulduz/Engine/ComponentStore.h>
#include <Yulduz/Engine/Type.h>
#include <Yulduz/Engine/Entity.h>
#include <Yulduz/Engine/EntityRegistry.h>

typedef struct YULDUZ_ECSRegistry YULDUZ_ECSRegistry;
typedef struct YULDUZ_ECSRegistryInitializeInfo
    YULDUZ_ECSRegistryInitializeInfo;

typedef struct YULDUZ_ArchetypeEdge YULDUZ_ArchetypeEdge;
typedef struct YULDUZ_ArchetypeTransitions
    YULDUZ_ArchetypeTransitions;

struct YULDUZ_ECSRegistry {
    YULDUZ_EntityRegistry EntityRegistry;
    YULDUZ_TypeRegistry   TypeRegistry;

    YULDUZ_Archetype *Dense;

    YULDUZ_ArchetypeTransitions *DenseTransitions;

    uint32_t DenseCapacity;
    uint32_t DenseCount;

    YULDUZ_ArchetypeType NullArchetypeType;

    uint32_t InitialArchetypeCapacity;
};

struct YULDUZ_ECSRegistryInitializeInfo {
    uint32_t InitialTypeCapacity;
    uint32_t InitialEntityCapacity;
    uint32_t InitialArchetypeCapacity;
    uint32_t InitialArchetypeTypeCapacity;
};

struct YULDUZ_ArchetypeEdge {
    YULDUZ_Type Type;

    YULDUZ_ArchetypeType ArchetypeType;
};

struct YULDUZ_ArchetypeTransitions {
    YULDUZ_ArchetypeEdge *AddEdges;
    YULDUZ_ArchetypeEdge *RemoveEdges;

    uint32_t AddEdgeCapacity;
    uint32_t AddEdgeCount;
    uint32_t RemoveEdgeCapacity;
    uint32_t RemoveEdgeCount;
};

YULDUZ_API bool YULDUZ_InitializeECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_ECSRegistryInitializeInfo info);
YULDUZ_API void YULDUZ_ReleaseECSRegistry(YULDUZ_ECSRegistry *registry);

YULDUZ_API bool YULDUZ_RegisterTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_TypeDescription description, YULDUZ_NULLABLE YULDUZ_Type *type);

YULDUZ_API bool YULDUZ_GetTypeInECSRegistry(const YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_Type *type);
YULDUZ_API bool YULDUZ_GetTypeInfoInECSRegistry(const YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_TypeInfo *info);
YULDUZ_API bool YULDUZ_GetTypeDescriptionInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Type type, YULDUZ_TypeDescription *description);

YULDUZ_API bool YULDUZ_CreateEntityInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity *entity);
YULDUZ_API bool YULDUZ_DestroyEntityInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity);

// Sets existing component data (immediate, no archetype change)
// Returns false if entity doesn't have this component
YULDUZ_API bool YULDUZ_SetComponentInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    const char *component_name, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_GetComponentInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    const char *component_name, void *component_data);

YULDUZ_API bool YULDUZ_SetComponentWithTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    YULDUZ_Type component_type, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_GetComponentWithTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    YULDUZ_Type component_type, void *component_data);

YULDUZ_API bool YULDUZ_AddComponentInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    const char *component_name, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_RemoveComponentInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *component_name);

YULDUZ_API bool YULDUZ_AddComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    YULDUZ_Type component_type, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_RemoveComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type component_type);

YULDUZ_API bool YULDUZ_GetArchetypeCountInECSRegistry(
    const YULDUZ_ECSRegistry *registry, uint32_t *archetype_count);

// Unsafe index!
YULDUZ_API YULDUZ_Archetype *YULDUZ_GetArchetypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_ArchetypeType archetype_type);

YULDUZ_API YULDUZ_ComponentStore *YULDUZ_QueryArchetypeStoreInECSRegistry(
    const YULDUZ_ECSRegistry *registry, const YULDUZ_Archetype *archetype, const char *component_name);