#pragma once

#include <Yulduz/Engine/Archetype.h>
#include <Yulduz/Engine/ComponentStore.h>
#include <Yulduz/Engine/ComponentType.h>
#include <Yulduz/Engine/Entity.h>
#include <Yulduz/Engine/EntityRegistry.h>
#include <Yulduz/Engine/TagType.h>

typedef struct YULDUZ_ECSRegistry YULDUZ_ECSRegistry;
typedef struct YULDUZ_ECSRegistryInitializeInfo
    YULDUZ_ECSRegistryInitializeInfo;

typedef struct YULDUZ_ArchetypeTagEdge YULDUZ_ArchetypeTagEdge;
typedef struct YULDUZ_ArchetypeTagTransitions
    YULDUZ_ArchetypeTagTransitions;

typedef struct YULDUZ_ArchetypeComponentEdge YULDUZ_ArchetypeComponentEdge;
typedef struct YULDUZ_ArchetypeComponentTransitions
    YULDUZ_ArchetypeComponentTransitions;

struct YULDUZ_ECSRegistry {
    YULDUZ_ComponentTypeRegistry *ComponentTypeRegistry;
    YULDUZ_TagTypeRegistry       *TagTypeRegistry;
    YULDUZ_EntityRegistry        *EntityRegistry;

    YULDUZ_Archetype *Dense;

    YULDUZ_ArchetypeTagTransitions *DenseTagTransitions;

    YULDUZ_ArchetypeComponentTransitions *DenseComponentTransitions;

    uint32_t DenseCapacity;
    uint32_t DenseCount;

    YULDUZ_ArchetypeType NullArchetypeType;

    uint32_t InitialArchetypeCapacity;
};

struct YULDUZ_ECSRegistryInitializeInfo {
    YULDUZ_ComponentTypeRegistry *ComponentTypeRegistry;
    YULDUZ_TagTypeRegistry       *TagTypeRegistry;
    YULDUZ_EntityRegistry        *EntityRegistry;

    uint32_t InitialArchetypeCapacity;
    uint32_t InitialArchetypeTypeCapacity;
};

struct YULDUZ_ArchetypeTagEdge {
    YULDUZ_TagType       TagType;
    YULDUZ_ArchetypeType ArchetypeType;
};

struct YULDUZ_ArchetypeTagTransitions {
    YULDUZ_ArchetypeTagEdge *AddEdges;
    YULDUZ_ArchetypeTagEdge *RemoveEdges;

    uint32_t AddEdgeCapacity;
    uint32_t AddEdgeCount;
    uint32_t RemoveEdgeCapacity;
    uint32_t RemoveEdgeCount;
};

struct YULDUZ_ArchetypeComponentEdge {
    YULDUZ_ComponentType ComponentType;
    YULDUZ_ArchetypeType ArchetypeType;
};

struct YULDUZ_ArchetypeComponentTransitions {
    YULDUZ_ArchetypeComponentEdge *AddEdges;
    YULDUZ_ArchetypeComponentEdge *RemoveEdges;

    uint32_t AddEdgeCapacity;
    uint32_t AddEdgeCount;
    uint32_t RemoveEdgeCapacity;
    uint32_t RemoveEdgeCount;
};

YULDUZ_API bool YULDUZ_InitializeECSRegistry(
    YULDUZ_ECSRegistry *registry, const YULDUZ_ECSRegistryInitializeInfo *info);
YULDUZ_API void YULDUZ_ReleaseECSRegistry(YULDUZ_ECSRegistry *registry);

YULDUZ_API bool YULDUZ_RegisterTagTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_NULLABLE YULDUZ_TagType *type);

YULDUZ_API bool YULDUZ_RegisterComponentTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, const YULDUZ_ComponentTypeDescription *description,
    YULDUZ_NULLABLE YULDUZ_ComponentType *type);

YULDUZ_API bool YULDUZ_GetTagTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_TagType *type);
YULDUZ_API bool YULDUZ_GetTagTypeNameInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_TagType type, char **name);

YULDUZ_API bool YULDUZ_GetComponentTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_ComponentType *type);
YULDUZ_API bool YULDUZ_GetComponentTypeDescriptionInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_ComponentType type, YULDUZ_ComponentTypeDescription *description);

YULDUZ_API bool YULDUZ_CreateEntityInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity *entity);
YULDUZ_API bool YULDUZ_DestroyEntityInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity);

YULDUZ_API bool YULDUZ_CloneEntityInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity src_entity, YULDUZ_Entity *dst_entity);

YULDUZ_API uint32_t YULDUZ_GetEntityCountInECSRegistry(const YULDUZ_ECSRegistry *registry);

YULDUZ_API bool YULDUZ_HasTagWithTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_TagType tag_type);

// UNSAFE! USE WITH CAUTION! returns a mapping unstable pointer to the component type. It assumes, component type exists
// It will cause segmentation faults if it doesn't
YULDUZ_API void *YULDUZ_GetComponentWithTypeUnsafeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_ComponentType component_type);

YULDUZ_API bool YULDUZ_GetComponentWithTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    YULDUZ_ComponentType component_type, YULDUZ_NULLABLE void *component_data);
YULDUZ_API bool YULDUZ_SetComponentWithTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    YULDUZ_ComponentType component_type, const void *component_data);

YULDUZ_API bool YULDUZ_AddTagWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_TagType tag_type);
YULDUZ_API bool YULDUZ_RemoveTagWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_TagType tag_type);

YULDUZ_API bool YULDUZ_AddComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    YULDUZ_ComponentType component_type, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_RemoveComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_TagType component_type);

YULDUZ_API bool YULDUZ_GetEntityRecordInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_EntityRecord *record);

// UNSAFE! Use with precautions! (entity must exist!, may cause segmentation faults if not)
YULDUZ_API void YULDUZ_GetEntityRecordUnsafeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_EntityRecord *record);

YULDUZ_API bool YULDUZ_GetArchetypeCountInECSRegistry(
    const YULDUZ_ECSRegistry *registry, uint32_t *archetype_count);

// Unsafe index! Unstable Pointer!
YULDUZ_API YULDUZ_Archetype *YULDUZ_GetArchetypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_ArchetypeType archetype_type);

YULDUZ_API YULDUZ_ComponentStore *YULDUZ_QueryArchetypeStoreInECSRegistry(
    const YULDUZ_ECSRegistry *registry, const YULDUZ_Archetype *archetype, const char *component_name);