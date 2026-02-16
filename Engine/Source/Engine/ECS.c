#include <Yulduz/Engine/ECS.h>

bool YULDUZ_GenerateArchetypeTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry,
    const YULDUZ_Type *component_types, uint32_t component_count,
    const YULDUZ_Type *tag_types, uint32_t tag_count,
    YULDUZ_ArchetypeType *archetype_type);

void YULDUZ_EnsureDenseCapacityInECSRegistry(YULDUZ_ECSRegistry *registry);

bool YULDUZ_InitializeECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_ECSRegistryInitializeInfo info) {
    SDL_zerop(registry);

    uint32_t initial_type_capacity           = info.InitialTypeCapacity;
    uint32_t initial_entity_capacity         = info.InitialEntityCapacity;
    uint32_t initial_archetype_capacity      = info.InitialArchetypeCapacity;
    uint32_t initial_archetype_type_capacity = info.InitialArchetypeTypeCapacity;

    if (!YULDUZ_InitializeTypeRegistry(&registry->TypeRegistry, initial_type_capacity)) {
        return false;
    }
    if (!YULDUZ_InitializeEntityRegistry(&registry->EntityRegistry, initial_entity_capacity)) {
        return false;
    }

    registry->DenseCapacity = initial_archetype_capacity;
    registry->DenseCount    = 1;
    registry->Dense         = SDL_malloc(sizeof(YULDUZ_Archetype) * initial_archetype_type_capacity);

    registry->InitialArchetypeCapacity = initial_archetype_capacity;

    registry->NullArchetypeType = 0;

    YULDUZ_Archetype *new_archetype = &registry->Dense[registry->NullArchetypeType];

    return YULDUZ_InitializeArchetype(new_archetype, nullptr, 0, nullptr, 0, initial_archetype_capacity);
}

void YULDUZ_ReleaseECSRegistry(YULDUZ_ECSRegistry *registry) {
    for (uint32_t i = 0; i < registry->DenseCount; i++) {
        YULDUZ_ReleaseArchetype(&registry->Dense[i]);
    }
    SDL_free(registry->Dense);

    YULDUZ_ReleaseEntityRegistry(&registry->EntityRegistry);
    YULDUZ_ReleaseTypeRegistry(&registry->TypeRegistry);

    SDL_zerop(registry);
}

bool YULDUZ_RegisterTypeInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_TypeDescription description, YULDUZ_Type *type) {
    return YULDUZ_RegisterTypesInTypeRegistry(&registry->TypeRegistry, &description, type, 1);
}

bool YULDUZ_GetTypeInECSRegistry(const YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_Type *type) {
    return YULDUZ_GetTypesInTypeRegistry(&registry->TypeRegistry, &name, type, 1);
}

bool YULDUZ_GetTypeInfoInECSRegistry(const YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_TypeInfo *info) {
    return YULDUZ_GetTypeInfosInTypeRegistry(&registry->TypeRegistry, &name, info, 1);
}

bool YULDUZ_GetTypeDescriptionInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Type type, YULDUZ_TypeDescription *description) {
    return YULDUZ_GetTypeDescriptionsInTypeRegistry(&registry->TypeRegistry, &type, description, 1);
}

bool YULDUZ_CreateEntityInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity *entity) {
    YULDUZ_EntityRecord record = (YULDUZ_EntityRecord){.ArchetypeType = registry->NullArchetypeType};
    if (!YULDUZ_AddInArchetype(
            &registry->Dense[registry->NullArchetypeType], YULDUZ_INVALID_ENTITY, nullptr, &record.ArchetypeIndex)) {
        return false;
    }
    if (!YULDUZ_CreateEntityInEntityRegistry(
            &registry->EntityRegistry, record.ArchetypeType, record.ArchetypeIndex, entity)) {
        YULDUZ_Entity moved_entity;
        YULDUZ_RemoveInArchetype(&registry->Dense[record.ArchetypeType], record.ArchetypeIndex, &moved_entity);
        if (YULDUZ_INVALID_ENTITY != moved_entity) {
            YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &moved_entity, &record, 1);
        }
        return false;
    }
    YULDUZ_SetEntityInArchetype(&registry->Dense[registry->NullArchetypeType], *entity, record.ArchetypeIndex);
    return true;
}

bool YULDUZ_DestroyEntityInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity) {
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    YULDUZ_Entity moved_entity;
    if (!YULDUZ_RemoveInArchetype(&registry->Dense[record.ArchetypeType], record.ArchetypeIndex, &moved_entity)) {
        return false;
    }
    if (YULDUZ_INVALID_ENTITY != moved_entity) {
        YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &moved_entity, &record, 1);
    }
    return YULDUZ_DestroyEntityInEntityRegistry(&registry->EntityRegistry, entity);
}

bool YULDUZ_HasTagInECSRegistry(const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *tag_name) {
    YULDUZ_EntityRecord record = {0};

    YULDUZ_Type tag_type;
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    if (!YULDUZ_GetTypesInTypeRegistry(&registry->TypeRegistry, &tag_name, &tag_type, 1)) {
        return false;
    }
    return nullptr != YULDUZ_QueryTagInArchetype(&registry->Dense[record.ArchetypeType], tag_type);
}

bool YULDUZ_SetComponentInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *component_name, const void *component_data) {
    YULDUZ_EntityRecord record = {0};

    YULDUZ_Type component_type;
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    if (!YULDUZ_GetTypesInTypeRegistry(&registry->TypeRegistry, &component_name, &component_type, 1)) {
        return false;
    }
    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(&registry->Dense[record.ArchetypeType], component_type);
    if (nullptr == store) {
        return false;
    }
    SDL_memcpy(YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex), component_data, store->TypeSize);
    return true;
}

bool YULDUZ_GetComponentInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *component_name, void *component_data) {
    YULDUZ_EntityRecord record = {0};

    YULDUZ_Type component_type;
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    if (!YULDUZ_GetTypesInTypeRegistry(&registry->TypeRegistry, &component_name, &component_type, 1)) {
        return false;
    }
    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(&registry->Dense[record.ArchetypeType], component_type);
    if (nullptr == store) {
        return false;
    }
    SDL_memcpy(component_data, YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex), store->TypeSize);
    return true;
}

bool YULDUZ_HasTagWithTypeInECSRegistry(const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type tag_type) {
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    return nullptr != YULDUZ_QueryTagInArchetype(&registry->Dense[record.ArchetypeType], tag_type);
}

bool YULDUZ_SetComponentWithTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type component_type, const void *component_data) {
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(&registry->Dense[record.ArchetypeType], component_type);
    if (nullptr == store) {
        return false;
    }
    SDL_memcpy(YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex), component_data, store->TypeSize);
    return true;
}

bool YULDUZ_GetComponentWithTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type component_type, void *component_data) {
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(&registry->Dense[record.ArchetypeType], component_type);
    if (nullptr == store) {
        return false;
    }
    SDL_memcpy(component_data, YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex), store->TypeSize);
    return true;
}

bool YULDUZ_AddTagInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *tag_name) {
    YULDUZ_Type tag_type;
    if (!YULDUZ_GetTypesInTypeRegistry(&registry->TypeRegistry, &tag_name, &tag_type, 1)) {
        return false;
    }
    return YULDUZ_AddTagWithTypeInECSRegistry(registry, entity, tag_type);
}

bool YULDUZ_RemoveTagInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *tag_name) {
    YULDUZ_Type tag_type;
    if (!YULDUZ_GetTypesInTypeRegistry(&registry->TypeRegistry, &tag_name, &tag_type, 1)) {
        return false;
    }
    return YULDUZ_RemoveTagWithTypeInECSRegistry(registry, entity, tag_type);
}

bool YULDUZ_AddComponentInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *component_name, const void *component_data) {
    YULDUZ_Type component_type;
    if (!YULDUZ_GetTypesInTypeRegistry(&registry->TypeRegistry, &component_name, &component_type, 1)) {
        return false;
    }
    return YULDUZ_AddComponentWithTypeInECSRegistry(registry, entity, component_type, component_data);
}

bool YULDUZ_RemoveComponentInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *component_name) {
    YULDUZ_Type component_type;
    if (!YULDUZ_GetTypesInTypeRegistry(&registry->TypeRegistry, &component_name, &component_type, 1)) {
        return false;
    }
    return YULDUZ_RemoveComponentWithTypeInECSRegistry(registry, entity, component_type);
}

bool YULDUZ_AddTagWithTypeInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type tag_type) {
    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &src_record, 1)) {
        return false;
    }

    YULDUZ_Archetype *src_archetype = &registry->Dense[src_record.ArchetypeType];

    if (nullptr != YULDUZ_QueryTagInArchetype(src_archetype, tag_type)) {
        return true;
    }

    uint32_t new_tag_count = src_archetype->TagCount + 1;

    YULDUZ_Type *new_tags = SDL_stack_alloc(YULDUZ_Type, new_tag_count);
    SDL_memcpy(new_tags, src_archetype->Tags, sizeof(YULDUZ_Type) * src_archetype->TagCount);
    new_tags[src_archetype->TagCount] = tag_type;
    YULDUZ_SDL_SortTypes(new_tags, new_tag_count);

    YULDUZ_Type *components = SDL_stack_alloc(YULDUZ_Type, src_archetype->StoreCount);
    for (uint32_t i = 0; i < src_archetype->StoreCount; i++) {
        components[i] = src_archetype->Stores[i].Type;
    }

    YULDUZ_QueryInfo query = {0};
    SDL_memset(&query, 0x00, sizeof(YULDUZ_QueryInfo));
    query.WithComponentTypes = components;
    query.WithComponentCount = src_archetype->StoreCount;
    query.WithTagTypes       = new_tags;
    query.WithTagCount       = new_tag_count;

    YULDUZ_ArchetypeType dst_archetype_type;
    if (!YULDUZ_GenerateArchetypeTypeInECSRegistry(
            registry,
            components, src_archetype->StoreCount,
            new_tags, new_tag_count,
            &dst_archetype_type)) {
        SDL_stack_free(components);
        SDL_stack_free(new_tags);
        return false;
    }

    YULDUZ_Archetype *dst_archetype = &registry->Dense[dst_archetype_type];

    YULDUZ_Entity src_moved_entity;

    YULDUZ_ArchetypeIndex dst_archetype_index;
    if (!YULDUZ_MoveEntityInArchetype(
            src_archetype, dst_archetype, nullptr, 0, src_record.ArchetypeIndex, &dst_archetype_index, &src_moved_entity)) {
        SDL_stack_free(components);
        SDL_stack_free(new_tags);
        return false;
    }

    if (YULDUZ_INVALID_ENTITY != src_moved_entity) {
        YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &src_moved_entity, &src_record, 1);
    }

    YULDUZ_EntityRecord dst_record = {0};
    dst_record.ArchetypeType       = dst_archetype_type;
    dst_record.ArchetypeIndex      = dst_archetype_index;
    if (!YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &dst_record, 1)) {
        SDL_stack_free(components);
        SDL_stack_free(new_tags);
        return false;
    }

    SDL_stack_free(components);
    SDL_stack_free(new_tags);
    return true;
}

bool YULDUZ_RemoveTagWithTypeInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type tag_type) {
    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &src_record, 1)) {
        return false;
    }

    YULDUZ_Archetype *src_archetype = &registry->Dense[src_record.ArchetypeType];

    if (nullptr == YULDUZ_QueryTagInArchetype(src_archetype, tag_type)) {
        return true;
    }

    uint32_t new_tag_count = src_archetype->TagCount - 1;

    YULDUZ_Type *new_tags = SDL_stack_alloc(YULDUZ_Type, new_tag_count);
    for (uint32_t i = 0, j = 0; i < src_archetype->TagCount; i++) {
        if (src_archetype->Tags[i] == tag_type) {
            continue;
        }
        new_tags[j] = src_archetype->Tags[i];
        j++;
    }

    YULDUZ_Type *components = SDL_stack_alloc(YULDUZ_Type, src_archetype->StoreCount);
    for (uint32_t i = 0; i < src_archetype->StoreCount; i++) {
        components[i] = src_archetype->Stores[i].Type;
    }

    YULDUZ_ArchetypeType dst_archetype_type;
    if (!YULDUZ_GenerateArchetypeTypeInECSRegistry(
            registry,
            components, src_archetype->StoreCount,
            new_tags, new_tag_count,
            &dst_archetype_type)) {
        SDL_stack_free(components);
        SDL_stack_free(new_tags);
        return false;
    }

    YULDUZ_Archetype *dst_archetype = &registry->Dense[dst_archetype_type];

    YULDUZ_Entity src_moved_entity;

    YULDUZ_ArchetypeIndex dst_archetype_index;
    if (!YULDUZ_MoveEntityInArchetype(
            src_archetype, dst_archetype, nullptr, 0, src_record.ArchetypeIndex, &dst_archetype_index, &src_moved_entity)) {
        SDL_stack_free(components);
        SDL_stack_free(new_tags);
        return false;
    }

    if (YULDUZ_INVALID_ENTITY != src_moved_entity) {
        YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &src_moved_entity, &src_record, 1);
    }

    YULDUZ_EntityRecord dst_record = {0};
    dst_record.ArchetypeType       = dst_archetype_type;
    dst_record.ArchetypeIndex      = dst_archetype_index;
    if (!YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &dst_record, 1)) {
        SDL_stack_free(components);
        SDL_stack_free(new_tags);
        return false;
    }

    SDL_stack_free(components);
    SDL_stack_free(new_tags);
    return true;
}

bool YULDUZ_AddComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type component_type, const void *component_data) {
    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &src_record, 1)) {
        return false;
    }

    YULDUZ_Archetype *src_archetype = &registry->Dense[src_record.ArchetypeType];

    if (nullptr != YULDUZ_QueryStoreInArchetype(src_archetype, component_type)) {
        return true;
    }

    uint32_t new_component_count = src_archetype->StoreCount + 1;

    YULDUZ_Type *new_components = SDL_stack_alloc(YULDUZ_Type, new_component_count);
    for (uint32_t i = 0; i < src_archetype->StoreCount; i++) {
        new_components[i] = src_archetype->Stores[i].Type;
    }
    new_components[src_archetype->StoreCount] = component_type;
    YULDUZ_SDL_SortTypes(new_components, new_component_count);

    YULDUZ_ArchetypeType dst_archetype_type;
    if (!YULDUZ_GenerateArchetypeTypeInECSRegistry(
            registry,
            new_components, new_component_count,
            src_archetype->Tags, src_archetype->TagCount,
            &dst_archetype_type)) {
        SDL_stack_free(new_components);
        return false;
    }

    YULDUZ_Archetype *dst_archetype = &registry->Dense[dst_archetype_type];

    YULDUZ_Entity src_moved_entity;

    YULDUZ_TypeDataInfo component_data_info = (YULDUZ_TypeDataInfo){
        .Type = component_type,
        .Data = component_data,
    };

    YULDUZ_ArchetypeIndex dst_archetype_index;
    if (!YULDUZ_MoveEntityInArchetype(
            src_archetype, dst_archetype, &component_data_info, 1, src_record.ArchetypeIndex, &dst_archetype_index, &src_moved_entity)) {
        SDL_stack_free(new_components);
        return false;
    }

    if (YULDUZ_INVALID_ENTITY != src_moved_entity) {
        YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &src_moved_entity, &src_record, 1);
    }

    YULDUZ_EntityRecord dst_record = {0};
    dst_record.ArchetypeType       = dst_archetype_type;
    dst_record.ArchetypeIndex      = dst_archetype_index;
    if (!YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &dst_record, 1)) {
        SDL_stack_free(new_components);
        return false;
    }

    SDL_stack_free(new_components);
    return true;
}

bool YULDUZ_RemoveComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type component_type) {
    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &src_record, 1)) {
        return false;
    }

    YULDUZ_Archetype *src_archetype = &registry->Dense[src_record.ArchetypeType];

    if (nullptr == YULDUZ_QueryStoreInArchetype(src_archetype, component_type)) {
        return true;
    }

    uint32_t new_component_count = src_archetype->StoreCount - 1;

    YULDUZ_Type *new_components = SDL_stack_alloc(YULDUZ_Type, new_component_count);
    for (uint32_t i = 0, j = 0; i < src_archetype->StoreCount; i++) {
        if (src_archetype->Stores[i].Type == component_type) {
            continue;
        }
        new_components[j] = src_archetype->Stores[i].Type;
        j++;
    }

    YULDUZ_ArchetypeType dst_archetype_type;
    if (!YULDUZ_GenerateArchetypeTypeInECSRegistry(
            registry,
            new_components, new_component_count,
            src_archetype->Tags, src_archetype->TagCount,
            &dst_archetype_type)) {
        SDL_stack_free(new_components);
        return false;
    }

    YULDUZ_Archetype *dst_archetype = &registry->Dense[dst_archetype_type];

    YULDUZ_Entity src_moved_entity;

    YULDUZ_ArchetypeIndex dst_archetype_index;
    if (!YULDUZ_MoveEntityInArchetype(
            src_archetype, dst_archetype, nullptr, 0, src_record.ArchetypeIndex, &dst_archetype_index, &src_moved_entity)) {
        SDL_stack_free(new_components);
        return false;
    }

    if (YULDUZ_INVALID_ENTITY != src_moved_entity) {
        YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &src_moved_entity, &src_record, 1);
    }

    YULDUZ_EntityRecord dst_record = {0};
    dst_record.ArchetypeType       = dst_archetype_type;
    dst_record.ArchetypeIndex      = dst_archetype_index;
    if (!YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &dst_record, 1)) {
        SDL_stack_free(new_components);
        return false;
    }

    SDL_stack_free(new_components);
    return true;
}

bool YULDUZ_GetArchetypeCountInECSRegistry(const YULDUZ_ECSRegistry *registry, uint32_t *archetype_count) {
    *archetype_count = registry->DenseCount;
    return true;
}

YULDUZ_Archetype *YULDUZ_GetArchetypeInECSRegistry(const YULDUZ_ECSRegistry *registry, YULDUZ_ArchetypeType archetype_type) {
    return &registry->Dense[archetype_type];
}

YULDUZ_ComponentStore *YULDUZ_QueryArchetypeStoreInECSRegistry(
    const YULDUZ_ECSRegistry *registry, const YULDUZ_Archetype *archetype, const char *component_name) {
    YULDUZ_Type component_type;
    if (!YULDUZ_GetTypesInTypeRegistry(&registry->TypeRegistry, &component_name, &component_type, 1)) {
        return nullptr;
    }
    return YULDUZ_QueryStoreInArchetype(archetype, component_type);
}

bool YULDUZ_GenerateArchetypeTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry,
    const YULDUZ_Type *component_types, uint32_t component_count,
    const YULDUZ_Type *tag_types, uint32_t tag_count,
    YULDUZ_ArchetypeType *archetype_type) {
    for (YULDUZ_ArchetypeType i = 0; i < registry->DenseCount; i++) {
        YULDUZ_Archetype *archetype = &registry->Dense[i];
        if (archetype->TagCount != tag_count) {
            continue;
        }
        if (archetype->StoreCount != component_count) {
            continue;
        }
        bool found_tags = true;
        for (uint32_t j = 0; j < tag_count; j++) {
            if (archetype->Tags[j] != tag_types[j]) {
                found_tags = false;
                break;
            }
        }
        if (!found_tags) {
            continue;
        }
        bool found_components = true;
        for (uint32_t j = 0; j < component_count; j++) {
            if (archetype->Stores[j].Type != component_types[j]) {
                found_components = false;
                break;
            }
        }
        if (!found_components) {
            continue;
        }
        *archetype_type = i;
        return true;
    }

    YULDUZ_TypeInfo *component_type_infos = nullptr;

    if (component_count > 0) {
        YULDUZ_TypeDescription *component_type_descriptions =
            SDL_stack_alloc(YULDUZ_TypeDescription, component_count);

        component_type_infos = SDL_stack_alloc(YULDUZ_TypeInfo, component_count);

        if (!YULDUZ_GetTypeDescriptionsInTypeRegistry(
                &registry->TypeRegistry, component_types, component_type_descriptions, component_count)) {
            SDL_stack_free(component_type_infos);
            SDL_stack_free(component_type_descriptions);
            return false;
        }

        for (uint32_t i = 0; i < component_count; i++) {
            component_type_infos[i].Type      = component_types[i];
            component_type_infos[i].Size      = component_type_descriptions[i].Size;
            component_type_infos[i].Alignment = component_type_descriptions[i].Alignment;
        }

        SDL_stack_free(component_type_descriptions);
    }

    YULDUZ_EnsureDenseCapacityInECSRegistry(registry);

    YULDUZ_ArchetypeType new_archetype_type = registry->DenseCount;
    registry->DenseCount++;

    YULDUZ_Archetype *new_archetype = &registry->Dense[new_archetype_type];
    if (!YULDUZ_InitializeArchetype(
            new_archetype,
            component_type_infos, component_count,
            tag_types, tag_count,
            registry->InitialArchetypeCapacity)) {
        if (component_count > 0) {
            SDL_stack_free(component_type_infos);
        }
        return false;
    }
    *archetype_type = new_archetype_type;

    if (component_count > 0) {
        SDL_stack_free(component_type_infos);
    }

    return true;
}

void YULDUZ_EnsureDenseCapacityInECSRegistry(YULDUZ_ECSRegistry *registry) {
    if (registry->DenseCount < registry->DenseCapacity) {
        return;
    }

    uint32_t old_capacity = registry->DenseCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_Archetype *new_dense = SDL_realloc(registry->Dense, sizeof(YULDUZ_Archetype) * new_capacity);

    registry->DenseCapacity = new_capacity;
    registry->Dense         = new_dense;
}