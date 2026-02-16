#include <Yulduz/Engine/ECS.h>

bool YULDUZ_MoveEntityInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_ArchetypeType dst_archetype_type,
    YULDUZ_NULLABLE const YULDUZ_TypeDataInfo *component_data, uint32_t component_data_count);

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

    registry->DenseCapacity    = initial_archetype_capacity;
    registry->DenseCount       = 1;
    registry->Dense            = SDL_malloc(sizeof(YULDUZ_Archetype) * initial_archetype_type_capacity);
    registry->DenseTransitions = SDL_malloc(sizeof(YULDUZ_ArchetypeTransitions) * initial_archetype_type_capacity);

    registry->InitialArchetypeCapacity = initial_archetype_capacity;

    registry->NullArchetypeType = 0;

    YULDUZ_Archetype *null_archetype = &registry->Dense[registry->NullArchetypeType];

    if (!YULDUZ_InitializeArchetype(null_archetype, nullptr, 0, nullptr, 0, initial_archetype_capacity)) {
        return false;
    }

    YULDUZ_ArchetypeTransitions *null_archetype_transitions = &registry->DenseTransitions[0];

    null_archetype_transitions->AddEdgeCapacity = 1;
    null_archetype_transitions->AddEdgeCount    = 0;
    null_archetype_transitions->AddEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeEdge));

    null_archetype_transitions->RemoveEdgeCapacity = 1;
    null_archetype_transitions->RemoveEdgeCount    = 0;
    null_archetype_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeEdge));

    return true;
}

void YULDUZ_ReleaseECSRegistry(YULDUZ_ECSRegistry *registry) {
    for (uint32_t i = 0; i < registry->DenseCount; i++) {
        YULDUZ_ReleaseArchetype(&registry->Dense[i]);

        SDL_free(registry->DenseTransitions[i].RemoveEdges);
        SDL_free(registry->DenseTransitions[i].AddEdges);
    }
    SDL_free(registry->Dense);
    SDL_free(registry->DenseTransitions);

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

bool YULDUZ_SetComponentInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *component_name, const void *component_data) {
    YULDUZ_EntityRecord record = {0};

    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    YULDUZ_TypeInfo component_type_info = {0};
    if (!YULDUZ_GetTypeInfosInTypeRegistry(&registry->TypeRegistry, &component_name, &component_type_info, 1)) {
        return false;
    }

    if (0 == component_type_info.Size) {
        return nullptr != YULDUZ_QueryTagInArchetype(&registry->Dense[record.ArchetypeType], component_type_info.Type);
    }

    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(
        &registry->Dense[record.ArchetypeType], component_type_info.Type);
    if (nullptr == store) {
        return false;
    }
    SDL_memcpy(YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex), component_data, store->TypeSize);
    return true;
}

bool YULDUZ_GetComponentInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, const char *component_name, void *component_data) {
    YULDUZ_EntityRecord record = {0};

    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    YULDUZ_TypeInfo component_type_info = {0};
    if (!YULDUZ_GetTypeInfosInTypeRegistry(&registry->TypeRegistry, &component_name, &component_type_info, 1)) {
        return false;
    }

    if (0 == component_type_info.Size) {
        return nullptr != YULDUZ_QueryTagInArchetype(&registry->Dense[record.ArchetypeType], component_type_info.Type);
    }

    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(
        &registry->Dense[record.ArchetypeType], component_type_info.Type);
    if (nullptr == store) {
        return false;
    }
    SDL_memcpy(component_data, YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex), store->TypeSize);
    return true;
}

bool YULDUZ_SetComponentWithTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type component_type, const void *component_data) {
    YULDUZ_EntityRecord record = {0};

    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &record, 1)) {
        return false;
    }
    YULDUZ_TypeDescription component_type_description = {0};
    if (!YULDUZ_GetTypeDescriptionsInTypeRegistry(&registry->TypeRegistry, &component_type, &component_type_description, 1)) {
        return false;
    }

    if (0 == component_type_description.Size) {
        return nullptr != YULDUZ_QueryTagInArchetype(&registry->Dense[record.ArchetypeType], component_type);
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
    YULDUZ_TypeDescription component_type_description = {0};
    if (!YULDUZ_GetTypeDescriptionsInTypeRegistry(&registry->TypeRegistry, &component_type, &component_type_description, 1)) {
        return false;
    }

    if (0 == component_type_description.Size) {
        return nullptr != YULDUZ_QueryTagInArchetype(&registry->Dense[record.ArchetypeType], component_type);
    }

    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(&registry->Dense[record.ArchetypeType], component_type);
    if (nullptr == store) {
        return false;
    }
    SDL_memcpy(component_data, YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex), store->TypeSize);
    return true;
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

bool YULDUZ_AddComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type component_type, const void *component_data) {
    YULDUZ_TypeDescription component_type_description = {0};
    if (!YULDUZ_GetTypeDescriptionsInTypeRegistry(
            &registry->TypeRegistry, &component_type, &component_type_description, 1)) {
        return false;
    }

    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &src_record, 1)) {
        return false;
    }

    YULDUZ_EnsureDenseCapacityInECSRegistry(registry);

    YULDUZ_Archetype *src_archetype = &registry->Dense[src_record.ArchetypeType];

    bool is_component_tag = 0 == component_type_description.Size;

    if (is_component_tag) {
        if (nullptr != YULDUZ_QueryTagInArchetype(src_archetype, component_type)) {
            return true;
        }
    } else {
        if (nullptr != YULDUZ_QueryStoreInArchetype(src_archetype, component_type)) {
            return true;
        }
    }

    YULDUZ_ArchetypeType dst_archetype_type = YULDUZ_INVALID_ARCHETYPE_TYPE;

    YULDUZ_ArchetypeTransitions *src_archetype_transitions = &registry->DenseTransitions[src_record.ArchetypeType];

    uint32_t              add_edge_count = src_archetype_transitions->AddEdgeCount;
    YULDUZ_ArchetypeEdge *add_edge_start = src_archetype_transitions->AddEdges;
    YULDUZ_ArchetypeEdge *add_edge       = SDL_bsearch(
        &component_type, add_edge_start, add_edge_count, sizeof(YULDUZ_ArchetypeEdge), YULDUZ_SDL_CompareTypes);
    if (nullptr != add_edge) {
        dst_archetype_type = add_edge->ArchetypeType;
    } else {
        uint32_t component_count = src_archetype->StoreCount + 1;
        uint32_t tag_count       = src_archetype->TagCount + 1;

        YULDUZ_TypeInfo *component_types = SDL_stack_alloc(YULDUZ_TypeInfo, component_count);
        YULDUZ_Type     *tag_types       = SDL_stack_alloc(YULDUZ_Type, tag_count);

        for (uint32_t i = 0; i < component_count - 1; i++) {
            component_types[i].Type      = src_archetype->Stores[i].Type;
            component_types[i].Size      = src_archetype->Stores[i].TypeSize;
            component_types[i].Alignment = src_archetype->Stores[i].TypeAlignment;
        }

        SDL_memcpy(tag_types, src_archetype->Tags, sizeof(YULDUZ_Type) * (tag_count - 1));

        if (is_component_tag) {
            component_count--;
            tag_types[tag_count - 1] = component_type;
            YULDUZ_SDL_SortTypes(tag_types, tag_count);
        } else {
            tag_count--;
            component_types[component_count - 1].Type      = component_type;
            component_types[component_count - 1].Size      = component_type_description.Size;
            component_types[component_count - 1].Alignment = component_type_description.Alignment;
            YULDUZ_SDL_SortTypeInfos(component_types, component_count);
        }

        dst_archetype_type = registry->DenseCount;
        registry->DenseCount++;

        if (!YULDUZ_InitializeArchetype(
                &registry->Dense[dst_archetype_type],
                component_types, component_count,
                tag_types, tag_count,
                registry->InitialArchetypeCapacity)) {
            SDL_stack_free(tag_types);
            SDL_stack_free(component_types);
            return false;
        }

        YULDUZ_ArchetypeTransitions *dst_archetype_transitions = &registry->DenseTransitions[dst_archetype_type];

        dst_archetype_transitions->AddEdgeCapacity = 1;
        dst_archetype_transitions->AddEdgeCount    = 0;
        dst_archetype_transitions->AddEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeEdge));

        dst_archetype_transitions->RemoveEdgeCapacity = 1;
        dst_archetype_transitions->RemoveEdgeCount    = 1;
        dst_archetype_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeEdge));

        dst_archetype_transitions->RemoveEdges[0].Type          = component_type;
        dst_archetype_transitions->RemoveEdges[0].ArchetypeType = src_record.ArchetypeType;

        if (src_archetype_transitions->AddEdgeCount >= src_archetype_transitions->AddEdgeCapacity) {
            src_archetype_transitions->AddEdgeCapacity *= 2;
            src_archetype_transitions->AddEdges = SDL_realloc(
                src_archetype_transitions->AddEdges,
                sizeof(YULDUZ_ArchetypeEdge) * src_archetype_transitions->AddEdgeCapacity);
        }

        uint32_t add_edge_index = 0;
        while (add_edge_index < src_archetype_transitions->AddEdgeCount &&
               component_type > src_archetype_transitions->AddEdges[add_edge_index].Type) {
            add_edge_index++;
        }

        if (add_edge_index < src_archetype_transitions->AddEdgeCount) {
            SDL_memmove(
                &src_archetype_transitions->AddEdges[add_edge_index + 1],
                &src_archetype_transitions->AddEdges[add_edge_index],
                sizeof(YULDUZ_ArchetypeEdge) * (src_archetype_transitions->AddEdgeCount - add_edge_index));
        }

        src_archetype_transitions->AddEdgeCount++;
        src_archetype_transitions->AddEdges[add_edge_index].Type          = component_type;
        src_archetype_transitions->AddEdges[add_edge_index].ArchetypeType = dst_archetype_type;

        SDL_stack_free(tag_types);
        SDL_stack_free(component_types);
    }

    YULDUZ_TypeDataInfo component_data_info = (YULDUZ_TypeDataInfo){
        .Type = component_type,
        .Data = component_data,
    };

    return YULDUZ_MoveEntityInECSRegistry(registry, entity, dst_archetype_type, &component_data_info, 1);
}

bool YULDUZ_RemoveComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_Type component_type) {
    YULDUZ_TypeDescription component_type_description = {0};
    if (!YULDUZ_GetTypeDescriptionsInTypeRegistry(
            &registry->TypeRegistry, &component_type, &component_type_description, 1)) {
        return false;
    }

    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &src_record, 1)) {
        return false;
    }

    YULDUZ_EnsureDenseCapacityInECSRegistry(registry);

    YULDUZ_Archetype *src_archetype = &registry->Dense[src_record.ArchetypeType];

    bool is_component_tag = 0 == component_type_description.Size;

    if (is_component_tag) {
        if (nullptr == YULDUZ_QueryTagInArchetype(src_archetype, component_type)) {
            return true;
        }
    } else {
        if (nullptr == YULDUZ_QueryStoreInArchetype(src_archetype, component_type)) {
            return true;
        }
    }

    YULDUZ_ArchetypeType dst_archetype_type = YULDUZ_INVALID_ARCHETYPE_TYPE;

    YULDUZ_ArchetypeTransitions *src_archetype_transitions = &registry->DenseTransitions[src_record.ArchetypeType];

    uint32_t              remove_edge_count = src_archetype_transitions->RemoveEdgeCount;
    YULDUZ_ArchetypeEdge *remove_edge_start = src_archetype_transitions->RemoveEdges;
    YULDUZ_ArchetypeEdge *remove_edge       = SDL_bsearch(
        &component_type, remove_edge_start, remove_edge_count, sizeof(YULDUZ_ArchetypeEdge), YULDUZ_SDL_CompareTypes);
    if (nullptr != remove_edge) {
        dst_archetype_type = remove_edge->ArchetypeType;
    } else {
        uint32_t component_count = src_archetype->StoreCount;
        uint32_t tag_count       = src_archetype->TagCount;

        YULDUZ_TypeInfo *component_types = SDL_stack_alloc(YULDUZ_TypeInfo, component_count + 1);
        YULDUZ_Type     *tag_types       = SDL_stack_alloc(YULDUZ_Type, tag_count + 1);

        if (is_component_tag) {
            for (uint32_t i = 0, j = 0; i < tag_count; i++) {
                if (component_type == src_archetype->Tags[i]) {
                    continue;
                }
                tag_types[j] = src_archetype->Tags[i];
                j++;
            }
            tag_count--;
            for (uint32_t i = 0; i < component_count; i++) {
                component_types[i].Type      = src_archetype->Stores[i].Type;
                component_types[i].Size      = src_archetype->Stores[i].TypeSize;
                component_types[i].Alignment = src_archetype->Stores[i].TypeAlignment;
            }
        } else {
            for (uint32_t i = 0, j = 0; i < component_count; i++) {
                if (component_type == src_archetype->Stores[i].Type) {
                    continue;
                }
                component_types[j].Type      = src_archetype->Stores[i].Type;
                component_types[j].Size      = src_archetype->Stores[i].TypeSize;
                component_types[j].Alignment = src_archetype->Stores[i].TypeAlignment;
                j++;
            }
            component_count--;
            SDL_memcpy(tag_types, src_archetype->Tags, sizeof(YULDUZ_Type) * tag_count);
        }

        dst_archetype_type = registry->DenseCount;
        registry->DenseCount++;

        if (!YULDUZ_InitializeArchetype(
                &registry->Dense[dst_archetype_type],
                component_types, component_count,
                tag_types, tag_count,
                registry->InitialArchetypeCapacity)) {
            SDL_stack_free(tag_types);
            SDL_stack_free(component_types);
            return false;
        }

        YULDUZ_ArchetypeTransitions *dst_archetype_transitions = &registry->DenseTransitions[dst_archetype_type];

        dst_archetype_transitions->AddEdgeCapacity = 1;
        dst_archetype_transitions->AddEdgeCount    = 1;
        dst_archetype_transitions->AddEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeEdge));

        dst_archetype_transitions->RemoveEdgeCapacity = 1;
        dst_archetype_transitions->RemoveEdgeCount    = 0;
        dst_archetype_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeEdge));

        dst_archetype_transitions->AddEdges[0].Type          = component_type;
        dst_archetype_transitions->AddEdges[0].ArchetypeType = src_record.ArchetypeType;

        if (src_archetype_transitions->RemoveEdgeCount >= src_archetype_transitions->RemoveEdgeCapacity) {
            src_archetype_transitions->RemoveEdgeCapacity *= 2;
            src_archetype_transitions->RemoveEdges = SDL_realloc(
                src_archetype_transitions->RemoveEdges,
                sizeof(YULDUZ_ArchetypeEdge) * src_archetype_transitions->RemoveEdgeCapacity);
        }

        uint32_t remove_edge_index = 0;
        while (remove_edge_index < src_archetype_transitions->RemoveEdgeCount &&
               component_type > src_archetype_transitions->RemoveEdges[remove_edge_index].Type) {
            remove_edge_index++;
        }

        if (remove_edge_index < src_archetype_transitions->RemoveEdgeCount) {
            SDL_memmove(
                &src_archetype_transitions->RemoveEdges[remove_edge_index + 1],
                &src_archetype_transitions->RemoveEdges[remove_edge_index],
                sizeof(YULDUZ_ArchetypeEdge) * (src_archetype_transitions->RemoveEdgeCount - remove_edge_index));
        }

        src_archetype_transitions->RemoveEdgeCount++;
        src_archetype_transitions->RemoveEdges[remove_edge_index].Type          = component_type;
        src_archetype_transitions->RemoveEdges[remove_edge_index].ArchetypeType = dst_archetype_type;

        SDL_stack_free(tag_types);
        SDL_stack_free(component_types);
    }

    return YULDUZ_MoveEntityInECSRegistry(registry, entity, dst_archetype_type, nullptr, 0);
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

bool YULDUZ_MoveEntityInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_ArchetypeType dst_archetype_type,
    YULDUZ_NULLABLE const YULDUZ_TypeDataInfo *component_data, uint32_t component_data_count) {
    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &src_record, 1)) {
        return false;
    }

    YULDUZ_Archetype *src_archetype = &registry->Dense[src_record.ArchetypeType];
    YULDUZ_Archetype *dst_archetype = &registry->Dense[dst_archetype_type];

    YULDUZ_Entity         src_moved_entity    = YULDUZ_INVALID_ENTITY;
    YULDUZ_ArchetypeIndex dst_archetype_index = YULDUZ_INVALID_ARCHETYPE_INDEX;
    if (!YULDUZ_MoveEntityInArchetype(
            src_archetype, dst_archetype,
            component_data, component_data_count, src_record.ArchetypeIndex,
            &dst_archetype_index, &src_moved_entity)) {
        return false;
    }

    if (YULDUZ_INVALID_ENTITY != src_moved_entity) {
        YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &src_moved_entity, &src_record, 1);
    }

    YULDUZ_EntityRecord dst_record = {0};
    dst_record.ArchetypeType       = dst_archetype_type;
    dst_record.ArchetypeIndex      = dst_archetype_index;
    return YULDUZ_SetEntityRecordsInEntityRegistry(&registry->EntityRegistry, &entity, &dst_record, 1);
}

void YULDUZ_EnsureDenseCapacityInECSRegistry(YULDUZ_ECSRegistry *registry) {
    if (registry->DenseCount < registry->DenseCapacity) {
        return;
    }

    uint32_t old_capacity = registry->DenseCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_Archetype            *new_dense             = SDL_realloc(registry->Dense, sizeof(YULDUZ_Archetype) * new_capacity);
    YULDUZ_ArchetypeTransitions *new_dense_transitions = SDL_realloc(registry->DenseTransitions, sizeof(YULDUZ_ArchetypeTransitions) * new_capacity);

    registry->DenseCapacity    = new_capacity;
    registry->Dense            = new_dense;
    registry->DenseTransitions = new_dense_transitions;
}