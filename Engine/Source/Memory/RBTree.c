#include <Yulduz/Memory/RBTree.h>

// Nodes use nullptr as the leaf marker rather than a shared sentinel node.
// YULDUZ_MemoryRBTree is copied by value (the same way every other allocator
// in this library is, e.g. YULDUZ_MemoryRBTreeMake returns one), and a
// sentinel embedded in the tree struct itself would leave every leaf pointer
// dangling the moment the struct is copied out of the function that made it.
// nullptr survives copies fine, at the cost of a Delete/DeleteFixup that has
// to thread a "logical parent" of x through explicitly, since x itself can be
// nullptr and therefore has no ->Parent to read.

static bool YULDUZ_MemoryRBTreeIsRed(const YULDUZ_MemoryRBTreeNode *node);

static void YULDUZ_MemoryRBTreeLeftRotate(YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *x);
static void YULDUZ_MemoryRBTreeRightRotate(YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *x);
static void YULDUZ_MemoryRBTreeTransplant(
    YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *u, YULDUZ_MemoryRBTreeNode *v);

static YULDUZ_MemoryRBTreeNode *YULDUZ_MemoryRBTreeMinimum(YULDUZ_MemoryRBTreeNode *x);

static void YULDUZ_MemoryRBTreeInsert(YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *z);
static void YULDUZ_MemoryRBTreeInsertFixup(YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *z);

static void YULDUZ_MemoryRBTreeDelete(YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *z);
static void YULDUZ_MemoryRBTreeDeleteFixup(
    YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *x, YULDUZ_MemoryRBTreeNode *x_parent);

static YULDUZ_MemoryRBTreeNode *YULDUZ_MemoryRBTreeFindBest(YULDUZ_MemoryRBTree *tree, Uint64 size);

static Uint64 YULDUZ_MemoryRBTreeGetPaddingWithHeader(const Uint8 *memory, Uint64 alignment, Uint64 header_size);

bool YULDUZ_MemoryRBTreeCreate(YULDUZ_MemoryRBTree *tree, Uint64 size) {
    SDL_zerop(tree);

    Uint8 *memory = SDL_malloc(size);
    if (nullptr == memory)
        return false;

    *tree = YULDUZ_MemoryRBTreeMake(memory, size);

    return true;
}

void YULDUZ_MemoryRBTreeDestroy(YULDUZ_MemoryRBTree *tree) {
    SDL_free(tree->Memory);
    SDL_zerop(tree);
}

void YULDUZ_MemoryRBTreeReset(YULDUZ_MemoryRBTree *tree) {
    tree->Offset = 0;

    YULDUZ_MemoryRBTreeNode *root = (void *)tree->Memory;

    root->Parent    = nullptr;
    root->Left      = nullptr;
    root->Right     = nullptr;
    root->Color     = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
    root->BlockSize = tree->Capacity;

    tree->Root = root;
}

bool YULDUZ_MemoryRBTreeAllocate(YULDUZ_MemoryRBTree *tree, Uint8 **memory, Uint64 size) {
    return YULDUZ_MemoryRBTreeAllocateAligned(tree, memory, size, sizeof(void *));
}

bool YULDUZ_MemoryRBTreeAllocateAligned(YULDUZ_MemoryRBTree *tree, Uint8 **memory, Uint64 size, Uint32 alignment) {
    SDL_assert(alignment > 0 && (alignment & (alignment - 1)) == 0 && "alignment must be a power of two");

    if (size < sizeof(YULDUZ_MemoryRBTreeNode))
        size = sizeof(YULDUZ_MemoryRBTreeNode);
    if (alignment < 8)
        alignment = 8;
    // Round up to a multiple of 8 too: padding is always a multiple of 8 (since
    // alignment is), so this keeps required_space - and therefore any split-off
    // YULDUZ_MemoryRBTreeNode's address - 8-byte aligned as well.
    size = (size + 7) & ~(Uint64)7;

    // The padding needed to satisfy `alignment` depends on the exact address
    // of whichever free block ends up being used, which isn't known until
    // after the tree has already picked one. Searching for a block at least
    // `size + (alignment - 1) + header` bytes big is a safe upper bound that
    // guarantees a fit regardless of the block's address, so the tree only
    // has to be searched once instead of possibly retrying on a near miss.
    Uint64 worst_case = size + (alignment - 1) + sizeof(YULDUZ_MemoryRBTreeAllocationHeader);

    YULDUZ_MemoryRBTreeNode *node = YULDUZ_MemoryRBTreeFindBest(tree, worst_case);
    if (nullptr == node)
        return false;

    YULDUZ_MemoryRBTreeDelete(tree, node);

    Uint64 padding = YULDUZ_MemoryRBTreeGetPaddingWithHeader(
        (Uint8 *)node, alignment, sizeof(YULDUZ_MemoryRBTreeAllocationHeader));
    Uint64 required_space = size + padding;
    Uint64 remaining      = node->BlockSize - required_space;
    Uint64 consumed       = node->BlockSize;

    // Only split off a new free node if what's left over is actually big
    // enough to be a node itself; otherwise the leftover bytes are absorbed
    // into this allocation as internal fragmentation.
    if (remaining > sizeof(YULDUZ_MemoryRBTreeNode)) {
        YULDUZ_MemoryRBTreeNode *new_node = (void *)((Uint8 *)node + required_space);
        new_node->BlockSize               = remaining;
        YULDUZ_MemoryRBTreeInsert(tree, new_node);
        consumed = required_space;
    }

    YULDUZ_MemoryRBTreeAllocationHeader *header =
        (void *)((Uint8 *)node + padding - sizeof(YULDUZ_MemoryRBTreeAllocationHeader));
    header->BlockSize = consumed;
    header->Padding   = padding;

    tree->Offset += consumed;

    *memory = (Uint8 *)node + padding;

    return true;
}

void YULDUZ_MemoryRBTreeFree(YULDUZ_MemoryRBTree *tree, Uint8 *memory) {
    if (nullptr == memory)
        return;

    YULDUZ_MemoryRBTreeAllocationHeader *header =
        (void *)(memory - sizeof(YULDUZ_MemoryRBTreeAllocationHeader));

    Uint64 padding    = header->Padding;
    Uint64 block_size = header->BlockSize;

    YULDUZ_MemoryRBTreeNode *free_node = (void *)(memory - padding);
    free_node->BlockSize               = block_size;

    YULDUZ_MemoryRBTreeInsert(tree, free_node);

    tree->Offset -= block_size;
}

YULDUZ_MemoryRBTree YULDUZ_MemoryRBTreeMake(Uint8 *memory, Uint64 size) {
    YULDUZ_MemoryRBTree tree = {0};
    SDL_zero(tree);
    tree.Memory   = memory;
    tree.Capacity = size;
    YULDUZ_MemoryRBTreeReset(&tree);
    return tree;
}

bool YULDUZ_MemoryRBTreeIsRed(const YULDUZ_MemoryRBTreeNode *node) {
    return node != nullptr && YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED == node->Color;
}

void YULDUZ_MemoryRBTreeLeftRotate(YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *x) {
    YULDUZ_MemoryRBTreeNode *y = x->Right;
    x->Right                   = y->Left;
    if (y->Left != nullptr)
        y->Left->Parent = x;
    y->Parent = x->Parent;
    if (nullptr == x->Parent)
        tree->Root = y;
    else if (x == x->Parent->Left)
        x->Parent->Left = y;
    else
        x->Parent->Right = y;
    y->Left   = x;
    x->Parent = y;
}

void YULDUZ_MemoryRBTreeRightRotate(YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *x) {
    YULDUZ_MemoryRBTreeNode *y = x->Left;
    x->Left                    = y->Right;
    if (y->Right != nullptr)
        y->Right->Parent = x;
    y->Parent = x->Parent;
    if (nullptr == x->Parent)
        tree->Root = y;
    else if (x == x->Parent->Right)
        x->Parent->Right = y;
    else
        x->Parent->Left = y;
    y->Right  = x;
    x->Parent = y;
}

void YULDUZ_MemoryRBTreeTransplant(YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *u, YULDUZ_MemoryRBTreeNode *v) {
    if (nullptr == u->Parent)
        tree->Root = v;
    else if (u == u->Parent->Left)
        u->Parent->Left = v;
    else
        u->Parent->Right = v;
    if (v != nullptr)
        v->Parent = u->Parent;
}

YULDUZ_MemoryRBTreeNode *YULDUZ_MemoryRBTreeMinimum(YULDUZ_MemoryRBTreeNode *x) {
    while (x->Left != nullptr)
        x = x->Left;
    return x;
}

void YULDUZ_MemoryRBTreeInsert(YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *z) {
    YULDUZ_MemoryRBTreeNode *y = nullptr;
    YULDUZ_MemoryRBTreeNode *x = tree->Root;

    while (x != nullptr) {
        y = x;
        x = (z->BlockSize < x->BlockSize) ? x->Left : x->Right;
    }

    z->Parent = y;
    if (nullptr == y)
        tree->Root = z;
    else if (z->BlockSize < y->BlockSize)
        y->Left = z;
    else
        y->Right = z;

    z->Left  = nullptr;
    z->Right = nullptr;
    z->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED;

    YULDUZ_MemoryRBTreeInsertFixup(tree, z);
}

void YULDUZ_MemoryRBTreeInsertFixup(YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *z) {
    while (YULDUZ_MemoryRBTreeIsRed(z->Parent)) {
        YULDUZ_MemoryRBTreeNode *grandparent = z->Parent->Parent;

        if (z->Parent == grandparent->Left) {
            YULDUZ_MemoryRBTreeNode *uncle = grandparent->Right;
            if (YULDUZ_MemoryRBTreeIsRed(uncle)) {
                z->Parent->Color   = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                uncle->Color       = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                grandparent->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED;
                z                  = grandparent;
            } else {
                if (z == z->Parent->Right) {
                    z = z->Parent;
                    YULDUZ_MemoryRBTreeLeftRotate(tree, z);
                }
                z->Parent->Color   = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                grandparent        = z->Parent->Parent;
                grandparent->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED;
                YULDUZ_MemoryRBTreeRightRotate(tree, grandparent);
            }
        } else {
            YULDUZ_MemoryRBTreeNode *uncle = grandparent->Left;
            if (YULDUZ_MemoryRBTreeIsRed(uncle)) {
                z->Parent->Color   = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                uncle->Color       = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                grandparent->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED;
                z                  = grandparent;
            } else {
                if (z == z->Parent->Left) {
                    z = z->Parent;
                    YULDUZ_MemoryRBTreeRightRotate(tree, z);
                }
                z->Parent->Color   = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                grandparent        = z->Parent->Parent;
                grandparent->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED;
                YULDUZ_MemoryRBTreeLeftRotate(tree, grandparent);
            }
        }
    }

    tree->Root->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
}

void YULDUZ_MemoryRBTreeDelete(YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *z) {
    YULDUZ_MemoryRBTreeNode     *y                = z;
    YULDUZ_MemoryRBTreeNode     *x                = nullptr;
    YULDUZ_MemoryRBTreeNode     *x_parent         = nullptr;
    YULDUZ_MemoryRBTreeNodeColor y_original_color = y->Color;

    if (nullptr == z->Left) {
        x        = z->Right;
        x_parent = z->Parent;
        YULDUZ_MemoryRBTreeTransplant(tree, z, z->Right);
    } else if (nullptr == z->Right) {
        x        = z->Left;
        x_parent = z->Parent;
        YULDUZ_MemoryRBTreeTransplant(tree, z, z->Left);
    } else {
        y                = YULDUZ_MemoryRBTreeMinimum(z->Right);
        y_original_color = y->Color;
        x                = y->Right;

        if (y->Parent == z) {
            x_parent = y;
        } else {
            x_parent = y->Parent;
            YULDUZ_MemoryRBTreeTransplant(tree, y, y->Right);
            y->Right         = z->Right;
            y->Right->Parent = y;
        }

        YULDUZ_MemoryRBTreeTransplant(tree, z, y);
        y->Left         = z->Left;
        y->Left->Parent = y;
        y->Color        = z->Color;
    }

    if (YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK == y_original_color)
        YULDUZ_MemoryRBTreeDeleteFixup(tree, x, x_parent);
}

void YULDUZ_MemoryRBTreeDeleteFixup(YULDUZ_MemoryRBTree *tree, YULDUZ_MemoryRBTreeNode *x, YULDUZ_MemoryRBTreeNode *x_parent) {
    while (x != tree->Root && !YULDUZ_MemoryRBTreeIsRed(x)) {
        if (x == x_parent->Left) {
            YULDUZ_MemoryRBTreeNode *sibling = x_parent->Right;

            if (YULDUZ_MemoryRBTreeIsRed(sibling)) {
                sibling->Color  = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                x_parent->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED;
                YULDUZ_MemoryRBTreeLeftRotate(tree, x_parent);
                sibling = x_parent->Right;
            }

            if (!YULDUZ_MemoryRBTreeIsRed(sibling->Left) && !YULDUZ_MemoryRBTreeIsRed(sibling->Right)) {
                sibling->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED;
                x              = x_parent;
                x_parent       = x_parent->Parent;
            } else {
                if (!YULDUZ_MemoryRBTreeIsRed(sibling->Right)) {
                    if (sibling->Left != nullptr)
                        sibling->Left->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                    sibling->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED;
                    YULDUZ_MemoryRBTreeRightRotate(tree, sibling);
                    sibling = x_parent->Right;
                }
                sibling->Color  = x_parent->Color;
                x_parent->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                if (sibling->Right != nullptr)
                    sibling->Right->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                YULDUZ_MemoryRBTreeLeftRotate(tree, x_parent);
                x        = tree->Root;
                x_parent = nullptr;
            }
        } else {
            YULDUZ_MemoryRBTreeNode *sibling = x_parent->Left;

            if (YULDUZ_MemoryRBTreeIsRed(sibling)) {
                sibling->Color  = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                x_parent->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED;
                YULDUZ_MemoryRBTreeRightRotate(tree, x_parent);
                sibling = x_parent->Left;
            }

            if (!YULDUZ_MemoryRBTreeIsRed(sibling->Right) && !YULDUZ_MemoryRBTreeIsRed(sibling->Left)) {
                sibling->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED;
                x              = x_parent;
                x_parent       = x_parent->Parent;
            } else {
                if (!YULDUZ_MemoryRBTreeIsRed(sibling->Left)) {
                    if (sibling->Right != nullptr)
                        sibling->Right->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                    sibling->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_RED;
                    YULDUZ_MemoryRBTreeLeftRotate(tree, sibling);
                    sibling = x_parent->Left;
                }
                sibling->Color  = x_parent->Color;
                x_parent->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                if (sibling->Left != nullptr)
                    sibling->Left->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
                YULDUZ_MemoryRBTreeRightRotate(tree, x_parent);
                x        = tree->Root;
                x_parent = nullptr;
            }
        }
    }

    if (x != nullptr)
        x->Color = YULDUZ_MEMORY_RB_TREE_NODE_COLOR_BLACK;
}

YULDUZ_MemoryRBTreeNode *YULDUZ_MemoryRBTreeFindBest(YULDUZ_MemoryRBTree *tree, Uint64 size) {
    YULDUZ_MemoryRBTreeNode *node = tree->Root;
    YULDUZ_MemoryRBTreeNode *best = nullptr;

    // Every node's BlockSize is >= everything in its left subtree and <=
    // everything in its right subtree, so this single walk down finds the
    // smallest sufficiently-large block in O(log n) instead of the O(n) scan
    // a plain linked free list needs.
    while (node != nullptr) {
        if (node->BlockSize >= size) {
            best = node;
            node = node->Left;
        } else {
            node = node->Right;
        }
    }

    return best;
}

Uint64 YULDUZ_MemoryRBTreeGetPaddingWithHeader(const Uint8 *memory, Uint64 alignment, Uint64 header_size) {
    SDL_assert(alignment > 0 && (alignment & (alignment - 1)) == 0 && "alignment must be a power of two");
    Uint64 current_address = (Uint64)memory;
    Uint64 aligned_address = YULDUZ_AddressAlignUp(current_address + header_size, alignment);
    return aligned_address - current_address;
}