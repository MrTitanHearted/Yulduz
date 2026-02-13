#include <Yulduz/Engine/ECS.h>

YULDUZ_Archetype *YULDUZ_GetArchetypeInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_ArchetypeType archetype_type);

bool YULDUZ_GetArchetypeTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry,
    const YULDUZ_Type *component_types, uint32_t component_count,
    const YULDUZ_Type *tag_types, uint32_t tag_count,
    YULDUZ_ArchetypeType *archetype_type);

bool YULDUZ_EnsureDenseCapacityInECSRegistry(YULDUZ_ECSRegistry *registry);

bool YULDUZ_InitializeECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_NULLABLE const YULDUZ_ECSRegistryInitializeInfo *info) {
    SDL_zerop(registry);
    uint32_t initial_entity_capacity    = 64;
    uint32_t initial_component_capacity = 64;
    uint32_t initial_archetype_capacity = 64;

    if (nullptr != info) {
        initial_entity_capacity    = info->InitialEntityCapacity;
        initial_component_capacity = info->InitialComponentCapacity;
        initial_archetype_capacity = info->InitialArchetypeCapacity;
    }

    if (!YULDUZ_InitializeEntityRegistry(&registry->EntityRegistry, initial_entity_capacity)) {
        return false;
    }
    if (!YULDUZ_InitializeTypeRegistry(&registry->TypeRegistry, initial_component_capacity)) {
        return false;
    }

    registry->DenseCapacity = initial_archetype_capacity;
    registry->DenseCount    = 0;
    registry->Dense         = SDL_malloc(sizeof(YULDUZ_Archetype) * initial_archetype_capacity);

    registry->InitialArchetypeCapacity = initial_archetype_capacity;

    if (!YULDUZ_GetArchetypeTypeInECSRegistry(registry, nullptr, 0, nullptr, 0, &registry->NullArchetypeType)) {
        return false;
    }

    return true;
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

bool YULDUZ_GetTypeInECSRegistry(YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_Type *type) {
    return YULDUZ_GetTypesInTypeRegistry(&registry->TypeRegistry, &name, type, 1);
}

bool YULDUZ_GetTypeInfoInECSRegistry(YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_TypeInfo *info) {
    return YULDUZ_GetTypeInfosInTypeRegistry(&registry->TypeRegistry, &name, info, 1);
}

bool YULDUZ_GetTypeDescriptionInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Type type, YULDUZ_TypeDescription *description) {
    return YULDUZ_GetTypeDescriptionsInTypeRegistry(&registry->TypeRegistry, &type, description, 1);
}

bool YULDUZ_CreateEntityInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity *entity) {
    if (!YULDUZ_CreateEntityInEntityRegistry(&registry->EntityRegistry, entity))
        return false;
    YULDUZ_EntityRecord record = (YULDUZ_EntityRecord){.ArchetypeType = registry->NullArchetypeType};
    if (!YULDUZ_AddInArchetype(&registry->Dense[registry->NullArchetypeType], *entity, nullptr, &record.ArchetypeIndex)) {
        YULDUZ_DestroyEntityInEntityRegistry(&registry->EntityRegistry, *entity);
        return false;
    }
    return YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, entity, &record, 1);
}

bool YULDUZ_DestroyEntityInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity) {
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    if (!YULDUZ_RemoveInArchetype(&registry->Dense[record.ArchetypeType], record.ArchetypeIndex)) {
        return false;
    }
    return YULDUZ_DestroyEntityInEntityRegistry(&registry->EntityRegistry, entity);
}

bool YULDUZ_HasTagInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *tag_name) {
    YULDUZ_EntityRecord record = {0};
    YULDUZ_Type         tag_type;
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    if (!YULDUZ_GetTypesInTypeRegistry(&registry->TypeRegistry, &tag_name, &tag_type, 1)) {
        return false;
    }
    return nullptr != YULDUZ_QueryTagInArchetype(&registry->Dense[record.ArchetypeType], tag_type);
}

bool YULDUZ_SetComponentInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *component_name, const void *component_data) {
    YULDUZ_EntityRecord record = {0};
    YULDUZ_Type         component_type;
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
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *component_name, void *component_data) {
    YULDUZ_EntityRecord record = {0};
    YULDUZ_Type         component_type;
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

bool YULDUZ_HasTagWithTypeInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type tag_type) {
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    return nullptr != YULDUZ_QueryTagInArchetype(&registry->Dense[record.ArchetypeType], tag_type);
}

bool YULDUZ_SetComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type component_type, const void *component_data) {
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
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type component_type, void *component_data) {
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
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }

    YULDUZ_Archetype *src_archetype = &registry->Dense[record.ArchetypeType];

    if (nullptr != YULDUZ_QueryTagInArchetype(src_archetype, tag_type)) {
        return true;
    }

    YULDUZ_Type *new_tags = SDL_stack_alloc(YULDUZ_Type, src_archetype->TagCount + 1);
    SDL_memcpy(new_tags, src_archetype->Tags, sizeof(YULDUZ_Type) * src_archetype->TagCount);
    new_tags[src_archetype->TagCount] = tag_type;
    YULDUZ_SDL_SortTypes(new_tags, src_archetype->TagCount + 1);

    YULDUZ_Type *components = SDL_stack_alloc(YULDUZ_Type, src_archetype->StoreCount);
    for (uint32_t i = 0; i < src_archetype->StoreCount; i++) {
        components[i] = src_archetype->Stores[i].Type;
    }

    YULDUZ_ArchetypeType dst_archetype_type;
    if (!YULDUZ_GetArchetypeTypeInECSRegistry(
            registry,
            components, src_archetype->StoreCount,
            new_tags, src_archetype->TagCount + 1,
            &dst_archetype_type)) {
        SDL_stack_free(components);
        SDL_stack_free(new_tags);
        return false;
    }

    YULDUZ_Archetype *dst_archetype = &registry->Dense[dst_archetype_type];

    YULDUZ_ArchetypeIndex dst_archetype_index;
    if (!YULDUZ_MoveEntityInArchetype(
            src_archetype, dst_archetype, nullptr, 0, record.ArchetypeIndex, &dst_archetype_index)) {
        SDL_stack_free(components);
        SDL_stack_free(new_tags);
        return false;
    }

    if (record.ArchetypeIndex < src_archetype->DenseCount) {
        YULDUZ_Entity moved_entity = src_archetype->Dense[record.ArchetypeIndex];
        if (!YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &moved_entity, &record, 1)) {
            SDL_stack_free(components);
            SDL_stack_free(new_tags);
            return false;
        }
    }

    record.ArchetypeType  = dst_archetype_type;
    record.ArchetypeIndex = dst_archetype_index;
    if (!YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        SDL_stack_free(components);
        SDL_stack_free(new_tags);
        return false;
    }

    SDL_stack_free(components);
    SDL_stack_free(new_tags);

    return true;
}

bool YULDUZ_RemoveTagWithTypeInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type tag_type) {
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }

    YULDUZ_Archetype *src_archetype = &registry->Dense[record.ArchetypeType];

    if (nullptr == YULDUZ_QueryTagInArchetype(src_archetype, tag_type)) {
        return true;
    }

    YULDUZ_Type *new_tags = SDL_stack_alloc(YULDUZ_Type, src_archetype->TagCount - 1);
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
    if (!YULDUZ_GetArchetypeTypeInECSRegistry(
            registry,
            components, src_archetype->StoreCount,
            new_tags, src_archetype->TagCount - 1,
            &dst_archetype_type)) {
        SDL_stack_free(components);
        SDL_stack_free(new_tags);
        return false;
    }

    YULDUZ_Archetype *dst_archetype = &registry->Dense[dst_archetype_type];

    YULDUZ_ArchetypeIndex dst_archetype_index;
    if (!YULDUZ_MoveEntityInArchetype(
            src_archetype, dst_archetype, nullptr, 0, record.ArchetypeIndex, &dst_archetype_index)) {
        SDL_stack_free(components);
        SDL_stack_free(new_tags);
        return false;
    }

    if (record.ArchetypeIndex < src_archetype->DenseCount) {
        YULDUZ_Entity moved_entity = src_archetype->Dense[record.ArchetypeIndex];
        if (!YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &moved_entity, &record, 1)) {
            SDL_stack_free(components);
            SDL_stack_free(new_tags);
            return false;
        }
    }

    record.ArchetypeType  = dst_archetype_type;
    record.ArchetypeIndex = dst_archetype_index;
    if (!YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
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
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }

    YULDUZ_Archetype *src_archetype = &registry->Dense[record.ArchetypeType];

    if (nullptr != YULDUZ_QueryStoreInArchetype(src_archetype, component_type)) {
        return true;
    }

    YULDUZ_Type *new_components = SDL_stack_alloc(YULDUZ_Type, src_archetype->StoreCount + 1);
    for (uint32_t i = 0; i < src_archetype->StoreCount; i++) {
        new_components[i] = src_archetype->Stores[i].Type;
    }
    new_components[src_archetype->StoreCount] = component_type;
    YULDUZ_SDL_SortTypes(new_components, src_archetype->StoreCount + 1);

    YULDUZ_ArchetypeType dst_archetype_type;
    if (!YULDUZ_GetArchetypeTypeInECSRegistry(
            registry,
            new_components, src_archetype->StoreCount + 1,
            src_archetype->Tags, src_archetype->TagCount,
            &dst_archetype_type)) {
        SDL_stack_free(new_components);
        return false;
    }

    YULDUZ_Archetype *dst_archetype = &registry->Dense[dst_archetype_type];

    YULDUZ_DataInfo info = (YULDUZ_DataInfo){
        .Type = component_type,
        .Data = component_data,
    };

    YULDUZ_ArchetypeIndex dst_archetype_index;
    if (!YULDUZ_MoveEntityInArchetype(
            src_archetype, dst_archetype, &info, 1, record.ArchetypeIndex, &dst_archetype_index)) {
        SDL_stack_free(new_components);
        return false;
    }

    if (record.ArchetypeIndex < src_archetype->DenseCount) {
        YULDUZ_Entity moved_entity = src_archetype->Dense[record.ArchetypeIndex];
        if (!YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &moved_entity, &record, 1)) {
            SDL_stack_free(new_components);
            return false;
        }
    }

    record.ArchetypeType  = dst_archetype_type;
    record.ArchetypeIndex = dst_archetype_index;
    if (!YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        SDL_stack_free(new_components);
        return false;
    }

    SDL_stack_free(new_components);

    return true;
}

bool YULDUZ_RemoveComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type component_type) {
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }

    YULDUZ_Archetype *src_archetype = &registry->Dense[record.ArchetypeType];

    if (nullptr == YULDUZ_QueryStoreInArchetype(src_archetype, component_type)) {
        return true;
    }

    YULDUZ_Type *new_components = SDL_stack_alloc(YULDUZ_Type, src_archetype->StoreCount - 1);
    for (uint32_t i = 0, j = 0; i < src_archetype->StoreCount; i++) {
        if (src_archetype->Stores[i].Type == component_type) {
            continue;
        }
        new_components[j] = src_archetype->Stores[i].Type;
        j++;
    }

    YULDUZ_ArchetypeType dst_archetype_type;
    if (!YULDUZ_GetArchetypeTypeInECSRegistry(
            registry,
            new_components, src_archetype->StoreCount - 1,
            src_archetype->Tags, src_archetype->TagCount,
            &dst_archetype_type)) {
        SDL_stack_free(new_components);
        return false;
    }

    YULDUZ_Archetype *dst_archetype = &registry->Dense[dst_archetype_type];

    YULDUZ_ArchetypeIndex dst_archetype_index;
    if (!YULDUZ_MoveEntityInArchetype(
            src_archetype, dst_archetype, nullptr, 0, record.ArchetypeIndex, &dst_archetype_index)) {
        SDL_stack_free(new_components);
        return false;
    }

    if (record.ArchetypeIndex < src_archetype->DenseCount) {
        YULDUZ_Entity moved_entity = src_archetype->Dense[record.ArchetypeIndex];
        if (!YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &moved_entity, &record, 1)) {
            SDL_stack_free(new_components);
            return false;
        }
    }

    record.ArchetypeType  = dst_archetype_type;
    record.ArchetypeIndex = dst_archetype_index;
    if (!YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        SDL_stack_free(new_components);
        return false;
    }

    SDL_stack_free(new_components);

    return true;
}

bool YULDUZ_GetArchetypeTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry,
    const YULDUZ_Type *component_types, uint32_t component_count,
    const YULDUZ_Type *tag_types, uint32_t tag_count,
    YULDUZ_ArchetypeType *archetype_type) {
    for (YULDUZ_ArchetypeType i = 0; i < registry->DenseCount; i++) {
        YULDUZ_Archetype *archetype = &registry->Dense[i];

        if (archetype->StoreCount != component_count) {
            continue;
        }
        if (archetype->TagCount != tag_count) {
            continue;
        }

        bool component_match = true;
        for (uint32_t j = 0; j < archetype->StoreCount; j++) {
            if (archetype->Stores[j].Type != component_types[j]) {
                component_match = false;
                break;
            }
        }
        if (!component_match) {
            continue;
        }

        bool tag_match = true;
        for (uint32_t j = 0; j < archetype->TagCount; j++) {
            if (archetype->Tags[j] != tag_types[j]) {
                tag_match = false;
                break;
            }
        }
        if (!tag_match) {
            continue;
        }

        *archetype_type = i;
        return true;
    }

    if (!YULDUZ_EnsureDenseCapacityInECSRegistry(registry)) {
        return false;
    }

    YULDUZ_TypeDescription *component_type_descriptions = SDL_stack_alloc(YULDUZ_TypeDescription, component_count);
    YULDUZ_TypeInfo        *component_type_infos        = SDL_stack_alloc(YULDUZ_TypeInfo, component_count);

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

    YULDUZ_ArchetypeType new_archetype_type = registry->DenseCount;
    registry->DenseCount++;

    YULDUZ_Archetype *new_archetype = &registry->Dense[new_archetype_type];
    if (!YULDUZ_InitializeArchetype(
            new_archetype, component_type_infos, component_count, tag_types, tag_count, registry->InitialArchetypeCapacity)) {
        SDL_stack_free(component_type_infos);
        SDL_stack_free(component_type_descriptions);
        return false;
    }
    *archetype_type = new_archetype_type;
    SDL_stack_free(component_type_infos);
    SDL_stack_free(component_type_descriptions);
    return true;
}

bool YULDUZ_EnsureDenseCapacityInECSRegistry(YULDUZ_ECSRegistry *registry) {
    if (registry->DenseCount < registry->DenseCapacity) {
        return true;
    }

    uint32_t old_capacity = registry->DenseCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_Archetype *new_dense = SDL_realloc(registry->Dense, sizeof(YULDUZ_Archetype) * new_capacity);
    if (!new_dense) {
        return false;
    }

    registry->DenseCapacity = new_capacity;
    registry->Dense         = new_dense;

    return true;
}