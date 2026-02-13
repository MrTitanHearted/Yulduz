#pragma once

#include <Yulduz/Common.h>

#include <Yulduz/Engine/Type.h>

typedef uint32_t YULDUZ_ComponentIndex;

typedef struct YULDUZ_ComponentStore YULDUZ_ComponentStore;

struct YULDUZ_ComponentStore {
    YULDUZ_Type Type;

    uint32_t _yulduz_padding;

    uint8_t *Dense;

    uint32_t TypeSize;
    uint32_t TypeAlignment;
};

constexpr YULDUZ_ComponentIndex YULDUZ_INVALID_COMPONENT_INDEX = 0xFFFFFFFF;

YULDUZ_API bool YULDUZ_InitializeComponentStore(
    YULDUZ_ComponentStore *store, YULDUZ_TypeInfo type_description, uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseComponentStore(YULDUZ_ComponentStore *store);

YULDUZ_API bool YULDUZ_ReallocateComponentStore(YULDUZ_ComponentStore *store, uint32_t old_capacity, uint32_t new_capacity);

YULDUZ_API void YULDUZ_CopyBackComponentInComponentStore(
    YULDUZ_ComponentStore *store, YULDUZ_ComponentIndex index, YULDUZ_ComponentIndex back_index);

YULDUZ_API void *YULDUZ_GetComponentInComponentStore(YULDUZ_ComponentStore *store, YULDUZ_ComponentIndex index);
