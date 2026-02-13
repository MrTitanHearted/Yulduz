#pragma once

#include <Yulduz/Engine/ECS.h>

typedef enum YULDUZ_CommandType YULDUZ_CommandType;

typedef struct YULDUZ_Command       YULDUZ_Command;
typedef struct YULDUZ_CommandBuffer YULDUZ_CommandBuffer;

enum YULDUZ_CommandType {
    YULDUZ_CommandType_DestroyEntity,
    YULDUZ_CommandType_AddTag,
    YULDUZ_CommandType_RemoveTag,
    YULDUZ_CommandType_AddComponent,
    YULDUZ_CommandType_RemoveComponent,
};

struct YULDUZ_Command {
    YULDUZ_CommandType Type;
    YULDUZ_Entity      Entity;

    union {
        struct {
        } DestroyEntity;

        struct {
            YULDUZ_Type TagType;
        } AddTag;

        struct {
            YULDUZ_Type TagType;
        } RemoveTag;

        struct {
            YULDUZ_Type ComponentType;
            uint32_t    ComponentData;  // Arena Index!
        } AddComponent;

        struct {
            YULDUZ_Type ComponentType;
        } RemoveComponent;
    };
};

struct YULDUZ_CommandBuffer {
    YULDUZ_ECSRegistry *Registry;

    YULDUZ_Command *Dense;

    uint8_t *ArenaData;

    uint32_t DenseCapacity;
    uint32_t DenseCount;

    uint32_t ArenaCapacity;
    uint32_t ArenaCount;
};

constexpr uint32_t YULDUZ_INVALID_ARENA_POINTER_IN_COMMAND_BUFFER = 0xFFFFFFFF;

YULDUZ_API bool YULDUZ_InitializeCommandBuffer(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_ECSRegistry *registry, uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseCommandBuffer(YULDUZ_CommandBuffer *command_buffer);

YULDUZ_API bool YULDUZ_RegisterType(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_TypeDescription description, YULDUZ_NULLABLE YULDUZ_Type *type);

YULDUZ_API bool YULDUZ_CreateEntity(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity *entity);

YULDUZ_API bool YULDUZ_HasTag(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, const char *tag_name);

// Sets existing component data (immediate, no archetype change)
// Returns false if entity doesn't have this component
YULDUZ_API bool YULDUZ_SetComponent(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity,
    const char *component_name, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_GetComponent(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity,
    const char *component_name, void *component_data);

YULDUZ_API bool YULDUZ_HasTagWithType(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, YULDUZ_Type tag_type);

YULDUZ_API bool YULDUZ_SetComponentWithType(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity,
    YULDUZ_Type component_type, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_GetComponentWithType(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity,
    YULDUZ_Type component_type, void *component_data);

// blow operations are deferred
YULDUZ_API bool YULDUZ_DestroyEntity(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity);

YULDUZ_API bool YULDUZ_AddTag(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, const char *tag_name);
YULDUZ_API bool YULDUZ_RemoveTag(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, const char *tag_name);

YULDUZ_API bool YULDUZ_AddComponent(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity,
    const char *component_name, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_RemoveComponent(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, const char *component_name);

YULDUZ_API bool YULDUZ_AddTagWithType(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, YULDUZ_Type tag_type);
YULDUZ_API bool YULDUZ_RemoveTagWithType(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, YULDUZ_Type tag_type);

YULDUZ_API bool YULDUZ_AddComponentWithType(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity,
    YULDUZ_Type component_type, YULDUZ_NULLABLE const void *component_data);
YULDUZ_API bool YULDUZ_RemoveComponentWithType(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, YULDUZ_Type component_type);

// Don't directly call this if you don't know what you are doing
YULDUZ_API bool YULDUZ_FlushCommandBuffer(YULDUZ_CommandBuffer *command_buffer);