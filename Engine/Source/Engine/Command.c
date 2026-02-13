#include <Yulduz/Engine/Command.h>

bool YULDUZ_ExecuteCommandInCommandBuffer(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Command *command);
bool YULDUZ_PushCommandInCommandBuffer(YULDUZ_CommandBuffer *command_buffer, const YULDUZ_Command *command);

uint32_t YULDUZ_PushArenaDataInCommandBuffer(
    YULDUZ_CommandBuffer *command_buffer, uint32_t size, YULDUZ_NULLABLE const void *data);
const void *YULDUZ_GetArenaDataInCommandBuffer(YULDUZ_CommandBuffer *command_buffer, uint32_t arena_data);

bool YULDUZ_EnsureDenseCapacityInCommandBuffer(YULDUZ_CommandBuffer *command_buffer);
bool YULDUZ_EnsureArenaCapacityInCommandBuffer(YULDUZ_CommandBuffer *command_buffer, uint32_t required_size);

bool YULDUZ_InitializeCommandBuffer(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_ECSRegistry *registry, uint32_t initial_capacity) {
    SDL_zerop(command_buffer);

    command_buffer->Registry = registry;

    command_buffer->DenseCapacity = initial_capacity;
    command_buffer->DenseCount    = 0;
    command_buffer->Dense         = SDL_malloc(sizeof(YULDUZ_Command) * initial_capacity);

    command_buffer->ArenaCapacity = initial_capacity;
    command_buffer->ArenaCount    = 0;
    command_buffer->ArenaData     = SDL_malloc(sizeof(uint8_t) * initial_capacity);

    return true;
}

void YULDUZ_ReleaseCommandBuffer(YULDUZ_CommandBuffer *command_buffer) {
    SDL_free(command_buffer->Dense);
    SDL_free(command_buffer->ArenaData);
    SDL_zerop(command_buffer);
}

bool YULDUZ_RegisterType(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_TypeDescription description, YULDUZ_NULLABLE YULDUZ_Type *type) {
    return YULDUZ_RegisterTypeInECSRegistry(command_buffer->Registry, description, type);
}

bool YULDUZ_CreateEntity(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity *entity) {
    return YULDUZ_CreateEntityInECSRegistry(command_buffer->Registry, entity);
}

bool YULDUZ_HasTag(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, const char *tag_name) {
    YULDUZ_Type tag_type;
    YULDUZ_GetTypeInECSRegistry(command_buffer->Registry, tag_name, &tag_type);
    return YULDUZ_HasTagWithTypeInECSRegistry(command_buffer->Registry, entity, tag_type);
}

// Sets existing component data (immediate, no archetype change)
// Returns false if entity doesn't have this component
bool YULDUZ_SetComponent(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity,
    const char *component_name, YULDUZ_NULLABLE const void *component_data) {
    YULDUZ_Type component_type;
    if (!YULDUZ_GetTypeInECSRegistry(command_buffer->Registry, component_name, &component_type)) {
        return false;
    }
    return YULDUZ_SetComponentWithTypeInECSRegistry(command_buffer->Registry, entity, component_type, component_data);
}

bool YULDUZ_GetComponent(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity,
    const char *component_name, void *component_data) {
    YULDUZ_Type component_type;
    if (!YULDUZ_GetTypeInECSRegistry(command_buffer->Registry, component_name, &component_type)) {
        return false;
    }
    return YULDUZ_GetComponentWithTypeInECSRegistry(command_buffer->Registry, entity, component_type, component_data);
}

bool YULDUZ_HasTagWithType(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, YULDUZ_Type tag_type) {
    return YULDUZ_HasTagWithTypeInECSRegistry(command_buffer->Registry, entity, tag_type);
}

bool YULDUZ_SetComponentWithType(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity,
    YULDUZ_Type component_type, YULDUZ_NULLABLE const void *component_data) {
    return YULDUZ_SetComponentWithTypeInECSRegistry(command_buffer->Registry, entity, component_type, component_data);
}

bool YULDUZ_GetComponentWithType(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity,
    YULDUZ_Type component_type, void *component_data) {
    return YULDUZ_GetComponentWithTypeInECSRegistry(command_buffer->Registry, entity, component_type, component_data);
}

// blow operations are deferred
bool YULDUZ_DestroyEntity(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity) {
    YULDUZ_Command command = {0};
    command.Type           = YULDUZ_CommandType_DestroyEntity;
    command.Entity         = entity;
    return YULDUZ_PushCommandInCommandBuffer(command_buffer, &command);
}

bool YULDUZ_AddTag(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, const char *tag_name) {
    YULDUZ_Type tag_type;
    if (!YULDUZ_GetTypeInECSRegistry(command_buffer->Registry, tag_name, &tag_type)) {
        return false;
    }

    YULDUZ_Command command = {0};
    command.Type           = YULDUZ_CommandType_AddTag;
    command.Entity         = entity;
    command.AddTag.TagType = tag_type;
    return YULDUZ_PushCommandInCommandBuffer(command_buffer, &command);
}

bool YULDUZ_RemoveTag(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, const char *tag_name) {
    YULDUZ_Type tag_type;
    if (!YULDUZ_GetTypeInECSRegistry(command_buffer->Registry, tag_name, &tag_type)) {
        return false;
    }

    YULDUZ_Command command    = {0};
    command.Type              = YULDUZ_CommandType_RemoveTag;
    command.Entity            = entity;
    command.RemoveTag.TagType = tag_type;
    return YULDUZ_PushCommandInCommandBuffer(command_buffer, &command);
}

bool YULDUZ_AddComponent(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity,
    const char *component_name, YULDUZ_NULLABLE const void *component_data) {
    YULDUZ_TypeInfo component_type_info;
    if (!YULDUZ_GetTypeInfoInECSRegistry(command_buffer->Registry, component_name, &component_type_info)) {
        return false;
    }
    YULDUZ_Command command             = {0};
    command.Type                       = YULDUZ_CommandType_AddComponent;
    command.Entity                     = entity;
    command.AddComponent.ComponentType = component_type_info.Type;
    command.AddComponent.ComponentData = YULDUZ_PushArenaDataInCommandBuffer(
        command_buffer, component_type_info.Size, component_data);
    return YULDUZ_PushCommandInCommandBuffer(command_buffer, &command);
}

bool YULDUZ_RemoveComponent(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, const char *component_name) {
    YULDUZ_Type component_type;
    if (!YULDUZ_GetTypeInECSRegistry(command_buffer->Registry, component_name, &component_type)) {
        return false;
    }

    YULDUZ_Command command                = {0};
    command.Type                          = YULDUZ_CommandType_RemoveComponent;
    command.Entity                        = entity;
    command.RemoveComponent.ComponentType = component_type;
    return YULDUZ_PushCommandInCommandBuffer(command_buffer, &command);
}

bool YULDUZ_AddTagWithType(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, YULDUZ_Type tag_type) {
    YULDUZ_Command command = {0};
    command.Type           = YULDUZ_CommandType_AddTag;
    command.Entity         = entity;
    command.AddTag.TagType = tag_type;
    return YULDUZ_PushCommandInCommandBuffer(command_buffer, &command);
}

bool YULDUZ_RemoveTagWithType(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, YULDUZ_Type tag_type) {
    YULDUZ_Command command    = {0};
    command.Type              = YULDUZ_CommandType_RemoveTag;
    command.Entity            = entity;
    command.RemoveTag.TagType = tag_type;
    return YULDUZ_PushCommandInCommandBuffer(command_buffer, &command);
}

YULDUZ_API bool YULDUZ_AddComponentWithType(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity,
    YULDUZ_Type component_type, YULDUZ_NULLABLE const void *component_data) {
    YULDUZ_TypeDescription component_type_description;
    if (!YULDUZ_GetTypeDescriptionInECSRegistry(command_buffer->Registry, component_type, &component_type_description)) {
        return false;
    }
    YULDUZ_Command command             = {0};
    command.Type                       = YULDUZ_CommandType_AddComponent;
    command.Entity                     = entity;
    command.AddComponent.ComponentType = component_type;
    command.AddComponent.ComponentData = YULDUZ_PushArenaDataInCommandBuffer(
        command_buffer, component_type_description.Size, component_data);
    return YULDUZ_PushCommandInCommandBuffer(command_buffer, &command);
}

bool YULDUZ_RemoveComponentWithType(
    YULDUZ_CommandBuffer *command_buffer, YULDUZ_Entity entity, YULDUZ_Type component_type) {
    YULDUZ_Command command                = {0};
    command.Type                          = YULDUZ_CommandType_RemoveComponent;
    command.Entity                        = entity;
    command.RemoveComponent.ComponentType = component_type;
    return YULDUZ_PushCommandInCommandBuffer(command_buffer, &command);
}

bool YULDUZ_FlushCommandBuffer(YULDUZ_CommandBuffer *command_buffer) {
    uint32_t command_count = command_buffer->DenseCount;
    for (uint32_t i = 0; i < command_count; i++) {
        YULDUZ_ExecuteCommandInCommandBuffer(command_buffer, &command_buffer->Dense[i]);
    }

    command_buffer->DenseCount = 0;
    command_buffer->ArenaCount = 0;

    return true;
}

bool YULDUZ_ExecuteCommandInCommandBuffer(YULDUZ_CommandBuffer *command_buffer, YULDUZ_Command *command) {
    YULDUZ_ECSRegistry *registry = command_buffer->Registry;

    YULDUZ_Entity entity = command->Entity;

    switch (command->Type) {
        case YULDUZ_CommandType_DestroyEntity: {
            return YULDUZ_DestroyEntityInECSRegistry(registry, entity);
        }
        case YULDUZ_CommandType_AddTag: {
            return YULDUZ_AddTagWithTypeInECSRegistry(registry, entity, command->AddTag.TagType);
        }
        case YULDUZ_CommandType_RemoveTag: {
            return YULDUZ_RemoveTagWithTypeInECSRegistry(registry, entity, command->RemoveTag.TagType);
        }
        case YULDUZ_CommandType_AddComponent: {
            return YULDUZ_AddComponentWithTypeInECSRegistry(
                registry, entity, command->AddComponent.ComponentType,
                YULDUZ_GetArenaDataInCommandBuffer(command_buffer, command->AddComponent.ComponentData));
        }
        case YULDUZ_CommandType_RemoveComponent: {
            return YULDUZ_RemoveComponentWithTypeInECSRegistry(registry, entity, command->RemoveComponent.ComponentType);
        }
    }
    return false;
}

bool YULDUZ_PushCommandInCommandBuffer(YULDUZ_CommandBuffer *command_buffer, const YULDUZ_Command *command) {
    if (!YULDUZ_EnsureDenseCapacityInCommandBuffer(command_buffer)) {
        return false;
    }

    uint32_t dense_index = command_buffer->DenseCount;
    command_buffer->DenseCount++;
    SDL_memcpy(&command_buffer->Dense[dense_index], command, sizeof(YULDUZ_Command));
    return true;
}

uint32_t YULDUZ_PushArenaDataInCommandBuffer(YULDUZ_CommandBuffer *command_buffer, uint32_t size, const void *data) {
    if (nullptr != data) {
        return YULDUZ_INVALID_ARENA_POINTER_IN_COMMAND_BUFFER;
    }

    if (!YULDUZ_EnsureArenaCapacityInCommandBuffer(command_buffer, size)) {
        return YULDUZ_INVALID_ARENA_POINTER_IN_COMMAND_BUFFER;
    }

    uint32_t byte_index = command_buffer->ArenaCount;
    command_buffer->ArenaCount += size;
    SDL_memcpy(&command_buffer->ArenaData[byte_index], data, size);
    return byte_index;
}

const void *YULDUZ_GetArenaDataInCommandBuffer(YULDUZ_CommandBuffer *command_buffer, uint32_t arena_data) {
    if (YULDUZ_INVALID_ARENA_POINTER_IN_COMMAND_BUFFER == arena_data) {
        return nullptr;
    }
    return &command_buffer->ArenaData[arena_data];
}

bool YULDUZ_EnsureDenseCapacityInCommandBuffer(YULDUZ_CommandBuffer *command_buffer) {
    if (command_buffer->DenseCount < command_buffer->DenseCapacity) {
        return true;
    }

    uint32_t old_capacity = command_buffer->DenseCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_Command *new_dense = SDL_realloc(command_buffer->Dense, sizeof(YULDUZ_Command) * new_capacity);
    if (nullptr == new_dense) {
        return false;
    }

    command_buffer->DenseCapacity = new_capacity;
    command_buffer->Dense         = new_dense;

    return true;
}

bool YULDUZ_EnsureArenaCapacityInCommandBuffer(YULDUZ_CommandBuffer *command_buffer, uint32_t required_size) {
    uint32_t required_count = command_buffer->ArenaCount + required_size;
    if (required_count < command_buffer->DenseCapacity) {
        return true;
    }

    uint32_t old_capacity = command_buffer->DenseCapacity;
    uint32_t new_capacity = old_capacity * 2;
    while (new_capacity <= required_count)
        new_capacity = old_capacity * 2;

    uint8_t *new_arena = SDL_realloc(command_buffer->ArenaData, sizeof(uint8_t) * new_capacity);
    if (nullptr == new_arena) {
        return false;
    }

    command_buffer->ArenaCapacity = new_capacity;
    command_buffer->ArenaData     = new_arena;

    return true;
}
