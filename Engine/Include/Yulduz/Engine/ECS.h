#pragma once

#include <Yulduz/Engine/Archetype.h>
#include <Yulduz/Engine/ComponentStore.h>
#include <Yulduz/Engine/Type.h>
#include <Yulduz/Engine/Entity.h>
#include <Yulduz/Engine/EntityRegistry.h>

typedef struct YULDUZ_ECSRegistry YULDUZ_ECSRegistry;
typedef struct YULDUZ_ECSRegistryInitializeInfo
    YULDUZ_ECSRegistryInitializeInfo;

struct YULDUZ_ECSRegistry {
    YULDUZ_EntityRegistry EntityRegistry;
    YULDUZ_TypeRegistry   TypeRegistry;

    YULDUZ_Archetype *Dense;

    YULDUZ_ArchetypeType NullArchetypeType;

    uint32_t InitialArchetypeCapacity;

    uint32_t DenseCapacity;
    uint32_t DenseCount;
};

struct YULDUZ_ECSRegistryInitializeInfo {
    uint32_t InitialEntityCapacity;
    uint32_t InitialComponentCapacity;
    uint32_t InitialArchetypeCapacity;
};

YULDUZ_API bool YULDUZ_InitializeECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_NULLABLE const YULDUZ_ECSRegistryInitializeInfo *info);
YULDUZ_API void YULDUZ_ReleaseECSRegistry(YULDUZ_ECSRegistry *registry);

YULDUZ_API bool YULDUZ_RegisterTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_TypeDescription description, YULDUZ_NULLABLE YULDUZ_Type *type);

YULDUZ_API bool YULDUZ_GetTypeInECSRegistry(YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_Type *type);
YULDUZ_API bool YULDUZ_GetTypeInfoInECSRegistry(YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_TypeInfo *info);
YULDUZ_API bool YULDUZ_GetTypeDescriptionInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Type type, YULDUZ_TypeDescription *description);

YULDUZ_API bool YULDUZ_CreateEntityInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity *entity);
YULDUZ_API bool YULDUZ_DestroyEntityInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity);

YULDUZ_API bool YULDUZ_HasTagInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *tag_name);

// Sets existing component data (immediate, no archetype change)
// Returns false if entity doesn't have this component
YULDUZ_API bool YULDUZ_SetComponentInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    const char *component_name, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_GetComponentInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    const char *component_name, void *component_data);

YULDUZ_API bool YULDUZ_HasTagWithTypeInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type tag_type);

YULDUZ_API bool YULDUZ_SetComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    YULDUZ_Type component_type, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_GetComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    YULDUZ_Type component_type, void *component_data);

YULDUZ_API bool YULDUZ_AddTagInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *tag_name);
YULDUZ_API bool YULDUZ_RemoveTagInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *tag_name);

YULDUZ_API bool YULDUZ_AddComponentInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    const char *component_name, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_RemoveComponentInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *component_name);

YULDUZ_API bool YULDUZ_AddTagWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type tag_type);
YULDUZ_API bool YULDUZ_RemoveTagWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type tag_type);

YULDUZ_API bool YULDUZ_AddComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    YULDUZ_Type component_type, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_RemoveComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type component_type);
