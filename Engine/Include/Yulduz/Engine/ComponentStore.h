#pragma once

#include <Yulduz/Engine/ComponentType.h>

typedef uint32_t YULDUZ_ComponentIndex;

typedef struct YULDUZ_ComponentStore YULDUZ_ComponentStore;

struct YULDUZ_ComponentStore {
    YULDUZ_ComponentType Type;

    uint32_t _yulduz_padding;

    uint8_t *Dense;

    uint32_t TypeAlignment;
    uint32_t TypeSize;
};

constexpr YULDUZ_ComponentIndex YULDUZ_INVALID_COMPONENT_INDEX = 0xFFFFFFFF;

YULDUZ_API bool YULDUZ_InitializeComponentStore(
    YULDUZ_ComponentStore *store, YULDUZ_ComponentTypeInfo type_info, uint32_t initial_capacity);
YULDUZ_API void YULDUZ_ReleaseComponentStore(YULDUZ_ComponentStore *store);

YULDUZ_API void YULDUZ_ReallocateComponentStore(
    YULDUZ_ComponentStore *store, uint32_t old_capacity, uint32_t new_capacity);

YULDUZ_API void YULDUZ_CopyComponentInComponentStore(
    const YULDUZ_ComponentStore *store, YULDUZ_ComponentIndex src_index, YULDUZ_ComponentIndex dst_index);

// UNSAFE! CAUTION! returns a mapping pointer to the component at index. (The index must exist in the store or else
// it may cause segmentation faults)
YULDUZ_API void *YULDUZ_GetComponentInComponentStore(const YULDUZ_ComponentStore *store, YULDUZ_ComponentIndex index);
