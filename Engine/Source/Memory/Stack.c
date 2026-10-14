#include <Yulduz/Memory/Stack.h>

static Uint64 YULDUZ_MemoryStackGetPaddingWithHeader(const Uint8 *memory, Uint64 alignment, Uint64 header_size);

bool YULDUZ_MemoryStackCreate(YULDUZ_MemoryStack *stack, Uint64 size) {
    SDL_zerop(stack);

    Uint8 *memory = SDL_malloc(size);
    if (nullptr == memory)
        return false;

    *stack = YULDUZ_MemoryStackMake(memory, size);

    return true;
}

void YULDUZ_MemoryStackDestroy(YULDUZ_MemoryStack *stack) {
    SDL_free(stack->Memory);
    SDL_zerop(stack);
}

void YULDUZ_MemoryStackReset(YULDUZ_MemoryStack *stack) {
    stack->Offset = 0;
}

bool YULDUZ_MemoryStackAllocate(YULDUZ_MemoryStack *stack, Uint8 **memory, Uint64 size) {
    return YULDUZ_MemoryStackAllocateAligned(stack, memory, size, 1);
}

bool YULDUZ_MemoryStackAllocateAligned(YULDUZ_MemoryStack *stack, Uint8 **memory, Uint64 size, Uint32 alignment) {
    SDL_assert(alignment > 0 && (alignment & (alignment - 1)) == 0 && "alignment must be a power of two");

    Uint64 padding = YULDUZ_MemoryStackGetPaddingWithHeader(
        &stack->Memory[stack->Offset], alignment, sizeof(YULDUZ_MemoryStackAllocationHeader));
    if (padding > stack->Capacity - stack->Offset)
        return false;
    if (size > stack->Capacity - stack->Offset - padding)
        return false;

    Uint64 aligned_offset = stack->Offset + padding;

    YULDUZ_MemoryStackAllocationHeader *header =
        (void *)(&stack->Memory[aligned_offset] - sizeof(YULDUZ_MemoryStackAllocationHeader));

    header->Padding = padding;
    header->Size    = size;

    *memory = &stack->Memory[aligned_offset];

    stack->Offset = aligned_offset + size;

    return true;
}

void YULDUZ_MemoryStackFree(YULDUZ_MemoryStack *stack, Uint8 *memory) {
    SDL_assert(memory >= stack->Memory && memory < stack->Memory + stack->Capacity);

    YULDUZ_MemoryStackAllocationHeader *header = (void *)(memory - sizeof(YULDUZ_MemoryStackAllocationHeader));

    Uint64 padding       = header->Padding;
    Uint64 size          = header->Size;
    Uint64 memory_offset = (Uint64)(memory - stack->Memory);

    SDL_assert((Uint8 *)header >= stack->Memory);
    SDL_assert(padding <= memory_offset);

    /* A stack allocator may only free the most recently allocated (topmost) block.
     * Freeing anything else would rewind Offset past allocations that are still
     * considered live, silently letting future Allocate calls overwrite them. */
    SDL_assert(memory_offset + size == stack->Offset && "Stack allocations must be freed in LIFO order (most-recent-first)");

    Uint64 previous_offset = memory_offset - padding;

    SDL_assert(previous_offset <= stack->Offset);

    stack->Offset = previous_offset;
}

YULDUZ_MemoryStack YULDUZ_MemoryStackMake(Uint8 *memory, Uint64 size) {
    return (YULDUZ_MemoryStack){
        .Memory   = memory,
        .Capacity = size,
        .Offset   = 0,
    };
}

Uint64 YULDUZ_MemoryStackGetPaddingWithHeader(const Uint8 *memory, Uint64 alignment, Uint64 header_size) {
    SDL_assert(alignment > 0 && (alignment & (alignment - 1)) == 0 && "alignment must be a power of two");
    Uint64 current_address = (Uint64)memory;
    Uint64 aligned_address = YULDUZ_AddressAlignUp(current_address + header_size, alignment);
    return aligned_address - current_address;
}