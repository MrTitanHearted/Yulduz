#include <Yulduz/Engine/ECS.h>

static bool YULDUZ_MoveEntityInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_ArchetypeType dst_archetype_type,
    YULDUZ_NULLABLE const YULDUZ_ComponentTypeDataInfo *component_data, uint32_t component_data_count);

static void YULDUZ_EnsureDenseCapacityInECSRegistry(YULDUZ_ECSRegistry *registry);

bool YULDUZ_InitializeECSRegistry(
    YULDUZ_ECSRegistry *registry, const YULDUZ_ECSRegistryInitializeInfo *info) {
    SDL_zerop(registry);

    uint32_t initial_archetype_capacity      = info->InitialArchetypeCapacity;
    uint32_t initial_archetype_type_capacity = info->InitialArchetypeTypeCapacity;

    registry->ComponentTypeRegistry = info->ComponentTypeRegistry;
    registry->TagTypeRegistry       = info->TagTypeRegistry;
    registry->EntityRegistry        = info->EntityRegistry;

    registry->DenseCapacity = initial_archetype_capacity;
    registry->DenseCount    = 1;
    registry->Dense         = SDL_malloc(sizeof(YULDUZ_Archetype) * initial_archetype_type_capacity);

    registry->DenseTagTransitions =
        SDL_malloc(sizeof(YULDUZ_ArchetypeTagTransitions) * initial_archetype_type_capacity);
    registry->DenseComponentTransitions =
        SDL_malloc(sizeof(YULDUZ_ArchetypeComponentTransitions) * initial_archetype_type_capacity);

    registry->InitialArchetypeCapacity = initial_archetype_capacity;

    registry->NullArchetypeType = 0;

    YULDUZ_Archetype *null_archetype = &registry->Dense[registry->NullArchetypeType];

    if (!YULDUZ_InitializeArchetype(null_archetype, nullptr, 0, nullptr, 0, initial_archetype_capacity)) {
        return false;
    }

    YULDUZ_ArchetypeTagTransitions *null_archetype_tag_transitions =
        &registry->DenseTagTransitions[registry->NullArchetypeType];
    null_archetype_tag_transitions->AddEdgeCapacity    = 1;
    null_archetype_tag_transitions->AddEdgeCount       = 0;
    null_archetype_tag_transitions->AddEdges           = SDL_malloc(sizeof(YULDUZ_ArchetypeTagEdge));
    null_archetype_tag_transitions->RemoveEdgeCapacity = 1;
    null_archetype_tag_transitions->RemoveEdgeCount    = 0;
    null_archetype_tag_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeTagEdge));

    YULDUZ_ArchetypeComponentTransitions *null_archetype_component_transitions =
        &registry->DenseComponentTransitions[registry->NullArchetypeType];
    null_archetype_component_transitions->AddEdgeCapacity    = 1;
    null_archetype_component_transitions->AddEdgeCount       = 0;
    null_archetype_component_transitions->AddEdges           = SDL_malloc(sizeof(YULDUZ_ArchetypeComponentEdge));
    null_archetype_component_transitions->RemoveEdgeCapacity = 1;
    null_archetype_component_transitions->RemoveEdgeCount    = 0;
    null_archetype_component_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeComponentEdge));

    return true;
}

void YULDUZ_ReleaseECSRegistry(YULDUZ_ECSRegistry *registry) {
    for (YULDUZ_ArchetypeType archetype_type = 0; archetype_type < registry->DenseCount; archetype_type++) {
        YULDUZ_Archetype *archetype = &registry->Dense[archetype_type];

        for (uint32_t store_index = 0; store_index < archetype->StoreCount; store_index++) {
            YULDUZ_ComponentStore *store = &archetype->Stores[store_index];

            YULDUZ_ComponentTypeDescription type_description = {0};
            YULDUZ_GetComponentTypeDescriptionUnsafeInComponentTypeRegistry(
                registry->ComponentTypeRegistry, store->Type, &type_description);
            if (nullptr == type_description.OnDestroyPFN) continue;

            for (YULDUZ_ArchetypeIndex component_index = 0; component_index < archetype->DenseCount; component_index++) {
                (*type_description.OnDestroyPFN)(
                    YULDUZ_GetComponentInComponentStore(store, component_index),
                    type_description.UserData);
            }
        }

        YULDUZ_ReleaseArchetype(archetype);

        SDL_free(registry->DenseTagTransitions[archetype_type].RemoveEdges);
        SDL_free(registry->DenseTagTransitions[archetype_type].AddEdges);
        SDL_free(registry->DenseComponentTransitions[archetype_type].RemoveEdges);
        SDL_free(registry->DenseComponentTransitions[archetype_type].AddEdges);
    }

    SDL_free(registry->Dense);
    SDL_free(registry->DenseTagTransitions);
    SDL_free(registry->DenseComponentTransitions);

    SDL_zerop(registry);
}

bool YULDUZ_RegisterTagTypeInECSRegistry(YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_TagType *type) {
    return YULDUZ_RegisterTagTypeInTagTypeRegistry(registry->TagTypeRegistry, name, type);
}

bool YULDUZ_RegisterComponentTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, const YULDUZ_ComponentTypeDescription *description,
    YULDUZ_NULLABLE YULDUZ_ComponentType *type) {
    return YULDUZ_RegisterComponentTypeInComponentTypeRegistry(registry->ComponentTypeRegistry, description, type);
}

bool YULDUZ_GetTagTypeInECSRegistry(const YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_TagType *type) {
    return YULDUZ_GetTagTypeInTagTypeRegistry(registry->TagTypeRegistry, name, type);
}

bool YULDUZ_GetTagTypeNameInECSRegistry(const YULDUZ_ECSRegistry *registry, YULDUZ_TagType type, char **name) {
    return YULDUZ_GetTagTypeNameInTagTypeRegistry(registry->TagTypeRegistry, type, name);
}

bool YULDUZ_GetComponentTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, const char *name, YULDUZ_ComponentType *type) {
    return YULDUZ_GetComponentTypeInComponentTypeRegistry(registry->ComponentTypeRegistry, name, type);
}

bool YULDUZ_GetComponentTypeDescriptionInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_ComponentType type, YULDUZ_ComponentTypeDescription *description) {
    return YULDUZ_GetComponentTypeDescriptionInComponentTypeRegistry(registry->ComponentTypeRegistry, type, description);
}

bool YULDUZ_CreateEntityInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity *entity) {
    YULDUZ_EntityRecord record = (YULDUZ_EntityRecord){.ArchetypeType = registry->NullArchetypeType};
    if (!YULDUZ_AddInArchetype(
            &registry->Dense[registry->NullArchetypeType], YULDUZ_INVALID_ENTITY, nullptr, 0, &record.ArchetypeIndex)) {
        return false;
    }
    YULDUZ_CreateEntityInEntityRegistry(registry->EntityRegistry, record, entity);
    YULDUZ_SetEntityUnsafeInArchetype(&registry->Dense[registry->NullArchetypeType], record.ArchetypeIndex, *entity);
    return true;
}

bool YULDUZ_DestroyEntityInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity) {
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordInEntityRegistry(registry->EntityRegistry, entity, &record)) {
        return false;
    }
    YULDUZ_Archetype *archetype = &registry->Dense[record.ArchetypeType];
    for (uint32_t i = 0; i < archetype->StoreCount; i++) {
        YULDUZ_ComponentStore *store = &archetype->Stores[i];

        YULDUZ_ComponentTypeDescription type_description = {0};
        YULDUZ_GetComponentTypeDescriptionUnsafeInComponentTypeRegistry(
            registry->ComponentTypeRegistry, store->Type, &type_description);
        if (nullptr != type_description.OnDestroyPFN) {
            (*type_description.OnDestroyPFN)(
                YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex),
                type_description.UserData);
        }
    }
    YULDUZ_Entity moved_entity;
    if (!YULDUZ_RemoveInArchetype(archetype, record.ArchetypeIndex, &moved_entity)) {
        return false;
    }
    if (YULDUZ_INVALID_ENTITY != moved_entity) {
        YULDUZ_SetEntityRecordUnsafeInEntityRegistry(registry->EntityRegistry, moved_entity, record);
    }
    YULDUZ_DestroyEntityUnsafeInEntityRegistry(registry->EntityRegistry, entity);
    return true;
}

bool YULDUZ_CloneEntityInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity src_entity, YULDUZ_Entity *dst_entity) {
    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordInEntityRegistry(registry->EntityRegistry, src_entity, &src_record)) {
        return false;
    }
    YULDUZ_Archetype *archetype = &registry->Dense[src_record.ArchetypeType];

    YULDUZ_ArchetypeIndex dst_index = YULDUZ_INVALID_ARCHETYPE_INDEX;
    if (!YULDUZ_AddInArchetype(archetype, YULDUZ_INVALID_ENTITY, nullptr, 0, &dst_index)) {
        return false;
    }
    for (uint32_t i = 0; i < archetype->StoreCount; i++) {
        YULDUZ_ComponentStore *store = &archetype->Stores[i];

        void *src_component = YULDUZ_GetComponentInComponentStore(store, src_record.ArchetypeIndex);
        void *dst_component = YULDUZ_GetComponentInComponentStore(store, dst_index);
        SDL_memcpy(dst_component, src_component, store->TypeSize);

        YULDUZ_ComponentTypeDescription type_description = {0};
        YULDUZ_GetComponentTypeDescriptionUnsafeInComponentTypeRegistry(
            registry->ComponentTypeRegistry, store->Type, &type_description);
        if (nullptr != type_description.OnClonePFN)
            (*type_description.OnClonePFN)(src_component, dst_component, type_description.UserData);
    }
    YULDUZ_EntityRecord dst_record = (YULDUZ_EntityRecord){
        .ArchetypeType  = src_record.ArchetypeType,
        .ArchetypeIndex = dst_index,
    };
    YULDUZ_CreateEntityInEntityRegistry(registry->EntityRegistry, dst_record, dst_entity);
    YULDUZ_SetEntityUnsafeInArchetype(archetype, dst_index, *dst_entity);
    return true;
}

uint32_t YULDUZ_GetEntityCountInECSRegistry(const YULDUZ_ECSRegistry *registry) {
    return YULDUZ_GetEntityCountInEntityRegistry(registry->EntityRegistry);
}

bool YULDUZ_HasTagWithTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_TagType tag_type) {
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordInEntityRegistry(registry->EntityRegistry, entity, &record)) {
        return false;
    }
    return nullptr != YULDUZ_QueryTagInArchetype(&registry->Dense[record.ArchetypeType], tag_type);
}

void *YULDUZ_GetComponentWithTypeUnsafeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_ComponentType component_type) {
    YULDUZ_EntityRecord record = {0};
    YULDUZ_GetEntityRecordUnsafeInEntityRegistry(registry->EntityRegistry, entity, &record);
    YULDUZ_ComponentTypeDescription component_type_description = {0};
    YULDUZ_GetComponentTypeDescriptionUnsafeInComponentTypeRegistry(
        registry->ComponentTypeRegistry, component_type, &component_type_description);
    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(
        &registry->Dense[record.ArchetypeType], component_type);
    return YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex);
}

bool YULDUZ_GetComponentWithTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    YULDUZ_ComponentType component_type, YULDUZ_NULLABLE void *component_data) {
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordInEntityRegistry(registry->EntityRegistry, entity, &record)) {
        return false;
    }
    YULDUZ_ComponentTypeDescription component_type_description = {0};
    if (!YULDUZ_GetComponentTypeDescriptionInComponentTypeRegistry(
            registry->ComponentTypeRegistry, component_type, &component_type_description)) {
        return false;
    }

    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(
        &registry->Dense[record.ArchetypeType], component_type);
    if (nullptr == store) {
        return false;
    }
    if (nullptr != component_data)
        SDL_memcpy(component_data, YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex), store->TypeSize);
    return true;
}

bool YULDUZ_SetComponentWithTypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    YULDUZ_ComponentType component_type, const void *component_data) {
    YULDUZ_EntityRecord record = {0};
    if (!YULDUZ_GetEntityRecordInEntityRegistry(registry->EntityRegistry, entity, &record)) {
        return false;
    }
    YULDUZ_ComponentTypeDescription component_type_description = {0};
    if (!YULDUZ_GetComponentTypeDescriptionInComponentTypeRegistry(
            registry->ComponentTypeRegistry, component_type, &component_type_description)) {
        return false;
    }

    YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(
        &registry->Dense[record.ArchetypeType], component_type);
    if (nullptr == store) {
        return false;
    }
    SDL_memcpy(YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex), component_data, store->TypeSize);
    return true;
}

bool YULDUZ_AddTagWithTypeInECSRegistry(YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_TagType tag_type) {
    char *tag_name = nullptr;
    if (!YULDUZ_GetTagTypeNameInTagTypeRegistry(registry->TagTypeRegistry, tag_type, &tag_name)) {
        return false;
    }

    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordInEntityRegistry(registry->EntityRegistry, entity, &src_record)) {
        return false;
    }

    YULDUZ_EnsureDenseCapacityInECSRegistry(registry);

    YULDUZ_Archetype *src_archetype = &registry->Dense[src_record.ArchetypeType];

    if (nullptr != YULDUZ_QueryTagInArchetype(src_archetype, tag_type)) {
        return true;
    }

    YULDUZ_ArchetypeType dst_archetype_type = YULDUZ_INVALID_ARCHETYPE_TYPE;

    YULDUZ_ArchetypeTagTransitions *src_tag_transitions = &registry->DenseTagTransitions[src_record.ArchetypeType];

    YULDUZ_ArchetypeTagEdge *add_edge = SDL_bsearch(
        &tag_type, src_tag_transitions->AddEdges, src_tag_transitions->AddEdgeCount,
        sizeof(YULDUZ_ArchetypeTagEdge), YULDUZ_SDL_CompareTagTypes);
    if (nullptr != add_edge) {
        dst_archetype_type = add_edge->ArchetypeType;
    } else {
        uint32_t component_count = src_archetype->StoreCount;
        uint32_t tag_count       = src_archetype->TagCount + 1;

        YULDUZ_ComponentTypeInfo *component_types = SDL_stack_alloc(YULDUZ_ComponentTypeInfo, component_count + 1);
        YULDUZ_TagType           *tag_types       = SDL_stack_alloc(YULDUZ_TagType, tag_count);

        for (uint32_t i = 0; i < component_count; i++) {
            component_types[i].Type      = src_archetype->Stores[i].Type;
            component_types[i].Size      = src_archetype->Stores[i].TypeSize;
            component_types[i].Alignment = src_archetype->Stores[i].TypeAlignment;
        }

        SDL_memcpy(tag_types, src_archetype->Tags, sizeof(YULDUZ_TagType) * (tag_count - 1));
        tag_types[tag_count - 1] = tag_type;
        YULDUZ_SDL_SortTagTypes(tag_types, tag_count);

        dst_archetype_type = registry->DenseCount;

        if (!YULDUZ_InitializeArchetype(
                &registry->Dense[dst_archetype_type],
                component_types, component_count,
                tag_types, tag_count,
                registry->InitialArchetypeCapacity)) {
            SDL_stack_free(tag_types);
            SDL_stack_free(component_types);
            return false;
        }
        registry->DenseCount++;

        YULDUZ_ArchetypeComponentTransitions *dst_component_transitions =
            &registry->DenseComponentTransitions[dst_archetype_type];
        dst_component_transitions->AddEdgeCapacity    = 1;
        dst_component_transitions->AddEdgeCount       = 0;
        dst_component_transitions->AddEdges           = SDL_malloc(sizeof(YULDUZ_ArchetypeComponentEdge));
        dst_component_transitions->RemoveEdgeCapacity = 1;
        dst_component_transitions->RemoveEdgeCount    = 0;
        dst_component_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeComponentEdge));

        YULDUZ_ArchetypeTagTransitions *dst_tag_transitions =
            &registry->DenseTagTransitions[dst_archetype_type];
        dst_tag_transitions->AddEdgeCapacity    = 1;
        dst_tag_transitions->AddEdgeCount       = 0;
        dst_tag_transitions->AddEdges           = SDL_malloc(sizeof(YULDUZ_ArchetypeTagEdge));
        dst_tag_transitions->RemoveEdgeCapacity = 1;
        dst_tag_transitions->RemoveEdgeCount    = 1;
        dst_tag_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeTagEdge));

        dst_tag_transitions->RemoveEdges[0].TagType       = tag_type;
        dst_tag_transitions->RemoveEdges[0].ArchetypeType = src_record.ArchetypeType;

        if (src_tag_transitions->AddEdgeCount >= src_tag_transitions->AddEdgeCapacity) {
            src_tag_transitions->AddEdgeCapacity *= 2;
            src_tag_transitions->AddEdges = SDL_realloc(
                src_tag_transitions->AddEdges,
                sizeof(YULDUZ_ArchetypeTagEdge) * src_tag_transitions->AddEdgeCapacity);
        }

        uint32_t add_edge_index = 0;
        while (add_edge_index < src_tag_transitions->AddEdgeCount &&
               tag_type > src_tag_transitions->AddEdges[add_edge_index].TagType) {
            add_edge_index++;
        }

        if (add_edge_index < src_tag_transitions->AddEdgeCount) {
            SDL_memmove(
                &src_tag_transitions->AddEdges[add_edge_index + 1],
                &src_tag_transitions->AddEdges[add_edge_index],
                sizeof(YULDUZ_ArchetypeTagEdge) * (src_tag_transitions->AddEdgeCount - add_edge_index));
        }

        src_tag_transitions->AddEdgeCount++;
        src_tag_transitions->AddEdges[add_edge_index].TagType       = tag_type;
        src_tag_transitions->AddEdges[add_edge_index].ArchetypeType = dst_archetype_type;

        SDL_stack_free(tag_types);
        SDL_stack_free(component_types);
    }

    return YULDUZ_MoveEntityInECSRegistry(registry, entity, dst_archetype_type, nullptr, 0);
}

bool YULDUZ_RemoveTagWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_TagType tag_type) {
    char *tag_name = nullptr;
    if (!YULDUZ_GetTagTypeNameInTagTypeRegistry(registry->TagTypeRegistry, tag_type, &tag_name)) {
        return false;
    }

    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordInEntityRegistry(registry->EntityRegistry, entity, &src_record)) {
        return false;
    }

    YULDUZ_EnsureDenseCapacityInECSRegistry(registry);

    YULDUZ_Archetype *src_archetype = &registry->Dense[src_record.ArchetypeType];

    if (nullptr == YULDUZ_QueryTagInArchetype(src_archetype, tag_type)) {
        return true;
    }

    YULDUZ_ArchetypeType dst_archetype_type = YULDUZ_INVALID_ARCHETYPE_TYPE;

    YULDUZ_ArchetypeTagTransitions *src_tag_transitions = &registry->DenseTagTransitions[src_record.ArchetypeType];

    YULDUZ_ArchetypeTagEdge *remove_tag_edge = SDL_bsearch(
        &tag_type, src_tag_transitions->RemoveEdges, src_tag_transitions->RemoveEdgeCount,
        sizeof(YULDUZ_ArchetypeTagEdge), YULDUZ_SDL_CompareTagTypes);
    if (nullptr != remove_tag_edge) {
        dst_archetype_type = remove_tag_edge->ArchetypeType;
    } else {
        uint32_t component_count = src_archetype->StoreCount;
        uint32_t tag_count       = src_archetype->TagCount - 1;

        YULDUZ_ComponentTypeInfo *component_types = SDL_stack_alloc(YULDUZ_ComponentTypeInfo, component_count + 1);
        YULDUZ_TagType           *tag_types       = SDL_stack_alloc(YULDUZ_TagType, tag_count + 1);

        for (uint32_t i = 0; i < component_count; i++) {
            component_types[i].Type      = src_archetype->Stores[i].Type;
            component_types[i].Size      = src_archetype->Stores[i].TypeSize;
            component_types[i].Alignment = src_archetype->Stores[i].TypeAlignment;
        }

        for (uint32_t i = 0, j = 0; i < tag_count + 1; i++) {
            if (tag_type == src_archetype->Tags[i]) continue;
            tag_types[j] = src_archetype->Tags[i];
            j++;
        }

        dst_archetype_type = registry->DenseCount;

        if (!YULDUZ_InitializeArchetype(
                &registry->Dense[dst_archetype_type],
                component_types, component_count,
                tag_types, tag_count,
                registry->InitialArchetypeCapacity)) {
            SDL_stack_free(tag_types);
            SDL_stack_free(component_types);
            return false;
        }
        registry->DenseCount++;

        YULDUZ_ArchetypeComponentTransitions *dst_component_transitions =
            &registry->DenseComponentTransitions[dst_archetype_type];
        dst_component_transitions->AddEdgeCapacity    = 1;
        dst_component_transitions->AddEdgeCount       = 0;
        dst_component_transitions->AddEdges           = SDL_malloc(sizeof(YULDUZ_ArchetypeComponentEdge));
        dst_component_transitions->RemoveEdgeCapacity = 1;
        dst_component_transitions->RemoveEdgeCount    = 0;
        dst_component_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeComponentEdge));

        YULDUZ_ArchetypeTagTransitions *dst_tag_transitions =
            &registry->DenseTagTransitions[dst_archetype_type];
        dst_tag_transitions->AddEdgeCapacity    = 1;
        dst_tag_transitions->AddEdgeCount       = 1;
        dst_tag_transitions->AddEdges           = SDL_malloc(sizeof(YULDUZ_ArchetypeTagEdge));
        dst_tag_transitions->RemoveEdgeCapacity = 1;
        dst_tag_transitions->RemoveEdgeCount    = 0;
        dst_tag_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeTagEdge));

        dst_tag_transitions->AddEdges[0].TagType       = tag_type;
        dst_tag_transitions->AddEdges[0].ArchetypeType = src_record.ArchetypeType;

        if (src_tag_transitions->RemoveEdgeCount >= src_tag_transitions->RemoveEdgeCapacity) {
            src_tag_transitions->RemoveEdgeCapacity *= 2;
            src_tag_transitions->RemoveEdges = SDL_realloc(
                src_tag_transitions->RemoveEdges,
                sizeof(YULDUZ_ArchetypeTagEdge) * src_tag_transitions->RemoveEdgeCapacity);
        }

        uint32_t remove_edge_index = 0;
        while (remove_edge_index < src_tag_transitions->RemoveEdgeCount &&
               tag_type > src_tag_transitions->RemoveEdges[remove_edge_index].TagType) {
            remove_edge_index++;
        }

        if (remove_edge_index < src_tag_transitions->RemoveEdgeCount) {
            SDL_memmove(
                &src_tag_transitions->RemoveEdges[remove_edge_index + 1],
                &src_tag_transitions->RemoveEdges[remove_edge_index],
                sizeof(YULDUZ_ArchetypeTagEdge) * (src_tag_transitions->RemoveEdgeCount - remove_edge_index));
        }

        src_tag_transitions->RemoveEdgeCount++;
        src_tag_transitions->RemoveEdges[remove_edge_index].TagType       = tag_type;
        src_tag_transitions->RemoveEdges[remove_edge_index].ArchetypeType = dst_archetype_type;

        SDL_stack_free(tag_types);
        SDL_stack_free(component_types);
    }

    return YULDUZ_MoveEntityInECSRegistry(registry, entity, dst_archetype_type, nullptr, 0);
}

bool YULDUZ_AddComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity,
    YULDUZ_ComponentType component_type, YULDUZ_NULLABLE const void *component_data) {
    YULDUZ_ComponentTypeDescription component_type_description = {0};
    if (!YULDUZ_GetComponentTypeDescriptionInComponentTypeRegistry(
            registry->ComponentTypeRegistry, component_type, &component_type_description)) {
        return false;
    }

    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordInEntityRegistry(registry->EntityRegistry, entity, &src_record)) {
        return false;
    }

    YULDUZ_EnsureDenseCapacityInECSRegistry(registry);

    YULDUZ_Archetype *src_archetype = &registry->Dense[src_record.ArchetypeType];

    if (nullptr != YULDUZ_QueryStoreInArchetype(src_archetype, component_type)) {
        return true;
    }

    YULDUZ_ArchetypeType dst_archetype_type = YULDUZ_INVALID_ARCHETYPE_TYPE;

    YULDUZ_ArchetypeComponentTransitions *src_component_transitions =
        &registry->DenseComponentTransitions[src_record.ArchetypeType];

    YULDUZ_ArchetypeComponentEdge *add_component_edge = SDL_bsearch(
        &component_type, src_component_transitions->AddEdges, src_component_transitions->AddEdgeCount,
        sizeof(YULDUZ_ArchetypeComponentEdge), YULDUZ_SDL_CompareComponentTypes);
    if (nullptr != add_component_edge) {
        dst_archetype_type = add_component_edge->ArchetypeType;
    } else {
        uint32_t component_count = src_archetype->StoreCount + 1;
        uint32_t tag_count       = src_archetype->TagCount;

        YULDUZ_ComponentTypeInfo *component_types = SDL_stack_alloc(YULDUZ_ComponentTypeInfo, component_count);
        YULDUZ_TagType           *tag_types       = SDL_stack_alloc(YULDUZ_TagType, tag_count + 1);

        for (uint32_t i = 0; i < component_count - 1; i++) {
            component_types[i].Type      = src_archetype->Stores[i].Type;
            component_types[i].Size      = src_archetype->Stores[i].TypeSize;
            component_types[i].Alignment = src_archetype->Stores[i].TypeAlignment;
        }
        component_types[component_count - 1].Type      = component_type;
        component_types[component_count - 1].Size      = component_type_description.Size;
        component_types[component_count - 1].Alignment = component_type_description.Alignment;
        SDL_qsort(component_types, component_count, sizeof(YULDUZ_ComponentTypeInfo), YULDUZ_SDL_CompareComponentTypes);

        SDL_memcpy(tag_types, src_archetype->Tags, sizeof(YULDUZ_TagType) * tag_count);

        dst_archetype_type = registry->DenseCount;

        if (!YULDUZ_InitializeArchetype(
                &registry->Dense[dst_archetype_type],
                component_types, component_count,
                tag_types, tag_count,
                registry->InitialArchetypeCapacity)) {
            SDL_stack_free(tag_types);
            SDL_stack_free(component_types);
            return false;
        }
        registry->DenseCount++;

        YULDUZ_ArchetypeTagTransitions *dst_tag_transitions =
            &registry->DenseTagTransitions[dst_archetype_type];
        dst_tag_transitions->AddEdgeCapacity    = 1;
        dst_tag_transitions->AddEdgeCount       = 0;
        dst_tag_transitions->AddEdges           = SDL_malloc(sizeof(YULDUZ_ArchetypeTagEdge));
        dst_tag_transitions->RemoveEdgeCapacity = 1;
        dst_tag_transitions->RemoveEdgeCount    = 0;
        dst_tag_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeTagEdge));

        YULDUZ_ArchetypeComponentTransitions *dst_component_transitions =
            &registry->DenseComponentTransitions[dst_archetype_type];
        dst_component_transitions->AddEdgeCapacity    = 1;
        dst_component_transitions->AddEdgeCount       = 0;
        dst_component_transitions->AddEdges           = SDL_malloc(sizeof(YULDUZ_ArchetypeComponentEdge));
        dst_component_transitions->RemoveEdgeCapacity = 1;
        dst_component_transitions->RemoveEdgeCount    = 1;
        dst_component_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeComponentEdge));

        dst_component_transitions->RemoveEdges[0].ComponentType = component_type;
        dst_component_transitions->RemoveEdges[0].ArchetypeType = src_record.ArchetypeType;

        if (src_component_transitions->AddEdgeCount >= src_component_transitions->AddEdgeCapacity) {
            src_component_transitions->AddEdgeCapacity *= 2;
            src_component_transitions->AddEdges = SDL_realloc(
                src_component_transitions->AddEdges,
                sizeof(YULDUZ_ArchetypeComponentEdge) * src_component_transitions->AddEdgeCapacity);
        }

        uint32_t add_edge_index = 0;
        while (add_edge_index < src_component_transitions->AddEdgeCount &&
               component_type > src_component_transitions->AddEdges[add_edge_index].ComponentType) {
            add_edge_index++;
        }

        if (add_edge_index < src_component_transitions->AddEdgeCount) {
            SDL_memmove(
                &src_component_transitions->AddEdges[add_edge_index + 1],
                &src_component_transitions->AddEdges[add_edge_index],
                sizeof(YULDUZ_ArchetypeComponentEdge) * (src_component_transitions->AddEdgeCount - add_edge_index));
        }

        src_component_transitions->AddEdgeCount++;
        src_component_transitions->AddEdges[add_edge_index].ComponentType = component_type;
        src_component_transitions->AddEdges[add_edge_index].ArchetypeType = dst_archetype_type;

        SDL_stack_free(tag_types);
        SDL_stack_free(component_types);
    }

    YULDUZ_ComponentTypeDataInfo component_data_info = (YULDUZ_ComponentTypeDataInfo){
        .Type = component_type,
        .Data = component_data,
    };

    if (!YULDUZ_MoveEntityInECSRegistry(registry, entity, dst_archetype_type, &component_data_info, 1)) {
        return false;
    }

    if (nullptr != component_type_description.OnCreatePFN) {
        YULDUZ_EntityRecord record = {0};
        YULDUZ_GetEntityRecordUnsafeInEntityRegistry(registry->EntityRegistry, entity, &record);
        YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(
            &registry->Dense[record.ArchetypeType], component_type);
        (*component_type_description.OnCreatePFN)(
            YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex),
            component_type_description.UserData);
    }

    return true;
}

bool YULDUZ_RemoveComponentWithTypeInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_TagType component_type) {
    YULDUZ_ComponentTypeDescription component_type_description = {0};
    if (!YULDUZ_GetComponentTypeDescriptionInComponentTypeRegistry(
            registry->ComponentTypeRegistry, component_type, &component_type_description)) {
        return false;
    }

    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordInEntityRegistry(registry->EntityRegistry, entity, &src_record)) {
        return false;
    }

    YULDUZ_EnsureDenseCapacityInECSRegistry(registry);

    YULDUZ_Archetype *src_archetype = &registry->Dense[src_record.ArchetypeType];

    if (nullptr == YULDUZ_QueryStoreInArchetype(src_archetype, component_type)) {
        return true;
    }

    YULDUZ_ArchetypeType dst_archetype_type = YULDUZ_INVALID_ARCHETYPE_TYPE;

    YULDUZ_ArchetypeComponentTransitions *src_component_transitions =
        &registry->DenseComponentTransitions[src_record.ArchetypeType];

    YULDUZ_ArchetypeComponentEdge *remove_component_edge = SDL_bsearch(
        &component_type, src_component_transitions->RemoveEdges, src_component_transitions->RemoveEdgeCount,
        sizeof(YULDUZ_ArchetypeComponentEdge), YULDUZ_SDL_CompareComponentTypes);
    if (nullptr != remove_component_edge) {
        dst_archetype_type = remove_component_edge->ArchetypeType;
    } else {
        uint32_t component_count = src_archetype->StoreCount - 1;
        uint32_t tag_count       = src_archetype->TagCount;

        YULDUZ_ComponentTypeInfo *component_types = SDL_stack_alloc(YULDUZ_ComponentTypeInfo, component_count + 1);
        YULDUZ_TagType           *tag_types       = SDL_stack_alloc(YULDUZ_TagType, tag_count + 1);

        for (uint32_t i = 0, j = 0; i < component_count + 1; i++) {
            if (component_type == src_archetype->Stores[i].Type) continue;
            component_types[j].Type      = src_archetype->Stores[i].Type;
            component_types[j].Size      = src_archetype->Stores[i].TypeSize;
            component_types[j].Alignment = src_archetype->Stores[i].TypeAlignment;
            j++;
        }
        SDL_memcpy(tag_types, src_archetype->Tags, sizeof(YULDUZ_TagType) * tag_count);

        dst_archetype_type = registry->DenseCount;

        if (!YULDUZ_InitializeArchetype(
                &registry->Dense[dst_archetype_type],
                component_types, component_count,
                tag_types, tag_count,
                registry->InitialArchetypeCapacity)) {
            SDL_stack_free(tag_types);
            SDL_stack_free(component_types);
            return false;
        }
        registry->DenseCount++;

        YULDUZ_ArchetypeTagTransitions *dst_tag_transitions =
            &registry->DenseTagTransitions[dst_archetype_type];
        dst_tag_transitions->AddEdgeCapacity    = 1;
        dst_tag_transitions->AddEdgeCount       = 0;
        dst_tag_transitions->AddEdges           = SDL_malloc(sizeof(YULDUZ_ArchetypeTagEdge));
        dst_tag_transitions->RemoveEdgeCapacity = 1;
        dst_tag_transitions->RemoveEdgeCount    = 0;
        dst_tag_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeTagEdge));

        YULDUZ_ArchetypeComponentTransitions *dst_component_transitions =
            &registry->DenseComponentTransitions[dst_archetype_type];
        dst_component_transitions->AddEdgeCapacity    = 1;
        dst_component_transitions->AddEdgeCount       = 1;
        dst_component_transitions->AddEdges           = SDL_malloc(sizeof(YULDUZ_ArchetypeComponentEdge));
        dst_component_transitions->RemoveEdgeCapacity = 1;
        dst_component_transitions->RemoveEdgeCount    = 0;
        dst_component_transitions->RemoveEdges        = SDL_malloc(sizeof(YULDUZ_ArchetypeComponentEdge));

        dst_component_transitions->AddEdges[0].ComponentType = component_type;
        dst_component_transitions->AddEdges[0].ArchetypeType = src_record.ArchetypeType;

        if (src_component_transitions->RemoveEdgeCount >= src_component_transitions->RemoveEdgeCapacity) {
            src_component_transitions->RemoveEdgeCapacity *= 2;
            src_component_transitions->RemoveEdges = SDL_realloc(
                src_component_transitions->RemoveEdges,
                sizeof(YULDUZ_ArchetypeComponentEdge) * src_component_transitions->RemoveEdgeCapacity);
        }

        uint32_t remove_edge_index = 0;
        while (remove_edge_index < src_component_transitions->RemoveEdgeCount &&
               component_type > src_component_transitions->RemoveEdges[remove_edge_index].ComponentType) {
            remove_edge_index++;
        }

        if (remove_edge_index < src_component_transitions->RemoveEdgeCount) {
            SDL_memmove(
                &src_component_transitions->RemoveEdges[remove_edge_index + 1],
                &src_component_transitions->RemoveEdges[remove_edge_index],
                sizeof(YULDUZ_ArchetypeComponentEdge) *
                    (src_component_transitions->RemoveEdgeCount - remove_edge_index));
        }

        src_component_transitions->RemoveEdgeCount++;
        src_component_transitions->RemoveEdges[remove_edge_index].ComponentType = component_type;
        src_component_transitions->RemoveEdges[remove_edge_index].ArchetypeType = dst_archetype_type;

        SDL_stack_free(tag_types);
        SDL_stack_free(component_types);
    }

    if (nullptr != component_type_description.OnDestroyPFN) {
        YULDUZ_EntityRecord record = {0};
        YULDUZ_GetEntityRecordUnsafeInEntityRegistry(registry->EntityRegistry, entity, &record);
        YULDUZ_ComponentStore *store = YULDUZ_QueryStoreInArchetype(
            &registry->Dense[record.ArchetypeType], component_type);
        (*component_type_description.OnDestroyPFN)(
            YULDUZ_GetComponentInComponentStore(store, record.ArchetypeIndex),
            component_type_description.UserData);
    }

    return YULDUZ_MoveEntityInECSRegistry(registry, entity, dst_archetype_type, nullptr, 0);
}

bool YULDUZ_GetEntityRecordInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_EntityRecord *record) {
    return YULDUZ_GetEntityRecordInEntityRegistry(registry->EntityRegistry, entity, record);
}

void YULDUZ_GetEntityRecordUnsafeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_EntityRecord *record) {
    YULDUZ_GetEntityRecordUnsafeInEntityRegistry(registry->EntityRegistry, entity, record);
}

bool YULDUZ_GetArchetypeCountInECSRegistry(const YULDUZ_ECSRegistry *registry, uint32_t *archetype_count) {
    *archetype_count = registry->DenseCount;
    return true;
}

YULDUZ_Archetype *YULDUZ_GetArchetypeInECSRegistry(
    const YULDUZ_ECSRegistry *registry, YULDUZ_ArchetypeType archetype_type) {
    return &registry->Dense[archetype_type];
}

YULDUZ_ComponentStore *YULDUZ_QueryArchetypeStoreInECSRegistry(
    const YULDUZ_ECSRegistry *registry, const YULDUZ_Archetype *archetype, const char *component_name) {
    YULDUZ_ComponentType component_type;
    if (!YULDUZ_GetComponentTypeInComponentTypeRegistry(
            registry->ComponentTypeRegistry, component_name, &component_type)) {
        return nullptr;
    }
    return YULDUZ_QueryStoreInArchetype(archetype, component_type);
}

bool YULDUZ_MoveEntityInECSRegistry(
    YULDUZ_ECSRegistry *registry, YULDUZ_Entity entity, YULDUZ_ArchetypeType dst_archetype_type,
    YULDUZ_NULLABLE const YULDUZ_ComponentTypeDataInfo *component_data, uint32_t component_data_count) {
    YULDUZ_EntityRecord src_record = {0};
    if (!YULDUZ_GetEntityRecordInEntityRegistry(registry->EntityRegistry, entity, &src_record)) {
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
        YULDUZ_SetEntityRecordUnsafeInEntityRegistry(registry->EntityRegistry, src_moved_entity, src_record);
    }

    YULDUZ_EntityRecord dst_record = {0};
    dst_record.ArchetypeType       = dst_archetype_type;
    dst_record.ArchetypeIndex      = dst_archetype_index;
    YULDUZ_SetEntityRecordUnsafeInEntityRegistry(registry->EntityRegistry, entity, dst_record);
    return true;
}

void YULDUZ_EnsureDenseCapacityInECSRegistry(YULDUZ_ECSRegistry *registry) {
    if (registry->DenseCount < registry->DenseCapacity) {
        return;
    }

    uint32_t old_capacity = registry->DenseCapacity;
    uint32_t new_capacity = old_capacity * 2;

    YULDUZ_Archetype *new_dense = SDL_realloc(registry->Dense, sizeof(YULDUZ_Archetype) * new_capacity);

    YULDUZ_ArchetypeTagTransitions *new_dense_tag_transitions =
        SDL_realloc(registry->DenseTagTransitions, sizeof(YULDUZ_ArchetypeTagTransitions) * new_capacity);
    YULDUZ_ArchetypeComponentTransitions *new_dense_component_transitions =
        SDL_realloc(registry->DenseComponentTransitions, sizeof(YULDUZ_ArchetypeComponentTransitions) * new_capacity);

    registry->DenseCapacity             = new_capacity;
    registry->Dense                     = new_dense;
    registry->DenseTagTransitions       = new_dense_tag_transitions;
    registry->DenseComponentTransitions = new_dense_component_transitions;
}
