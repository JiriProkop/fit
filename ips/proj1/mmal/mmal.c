/**
 * Implementace My MALloc
 * Demonstracni priklad pro 1. ukol IPS/2022
 * Ales Smrcka
 */

#include "mmal.h"
#include <sys/mman.h> // mmap
#include <stdbool.h>  // bool
#include <assert.h>   // assert
#include <string.h>   // memcpy
#include <stdio.h>    // printf

#ifdef NDEBUG
/**
 * The structure header encapsulates data of a single memory block.
 *   ---+------+----------------------------+---
 *      |Header|DDD not_free DDDDD...free...|
 *   ---+------+-----------------+----------+---
 *             |-- Header.asize -|
 *             |-- Header.size -------------|
 */
typedef struct header Header;
struct header
{

    /**
     * Pointer to the next header. Cyclic list. If there is no other block,
     * points to itself.
     */
    Header *next;

    /// size of the block
    size_t size;

    /**
     * Size of block in bytes allocated for program. asize=0 means the block
     * is not used by a program.
     */
    size_t asize;
};

/**
 * The arena structure.
 *   /--- arena metadata
 *   |     /---- header of the first block
 *   v     v
 *   +-----+------+-----------------------------+
 *   |Arena|Header|.............................|
 *   +-----+------+-----------------------------+
 *
 *   |--------------- Arena.size ---------------|
 */
typedef struct arena Arena;
struct arena
{

    /**
     * Pointer to the next arena. Single-linked list.
     */
    Arena *next;

    /// Arena size.
    size_t size;
};

#define PAGE_SIZE (128 * 1024)

#endif // NDEBUG

#define MIN_ASIZE (8)
#define ARENA_SIZE (128 * 1024)

Arena *first_arena = NULL;

/**
 * Return size alligned to PAGE_SIZE
 */
static size_t allign_page(size_t size)
{
    return ((size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
}

/**
 * Allocate a new arena using mmap.
 * @param req_size requested size in bytes. Should be alligned to PAGE_SIZE.
 * @return pointer to a new arena, if successfull. NULL if error.
 * @pre req_size > sizeof(Arena) + sizeof(Header)
 */

/**
 *   +-----+------------------------------------+
 *   |Arena|....................................|
 *   +-----+------------------------------------+
 *
 *   |--------------- Arena.size ---------------|
 */
static Arena *arena_alloc(size_t req_size)
{
    Arena *tmp = mmap(NULL, req_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (tmp == NULL)
        return NULL;
    else
    {
        tmp->next = NULL;
        tmp->size = req_size;
    }
    return tmp;
}

/**
 * Appends a new arena to the end of the arena list.
 * @param a     already allocated arena
 */
static void arena_append(Arena *a)
{
    if (first_arena == NULL)
    {
        first_arena = a;
        return;
    }
    Arena *tmp = first_arena;
    while (tmp->next != NULL)
    {
        tmp = tmp->next;
    }
    tmp->next = a;
}

/**
 * Header structure constructor (alone, not used block).
 * @param hdr       pointer to block metadata.
 * @param size      size of free block
 * @pre size > 0
 */
/**
 *   +-----+------+------------------------+----+
 *   | ... |Header|........................| ...|
 *   +-----+------+------------------------+----+
 *
 *                |-- Header.size ---------|
 */
static void hdr_ctor(Header *hdr, size_t size)
{
    hdr->asize = size - sizeof(Header);
    hdr->size = size;
    if (first_arena == NULL)
        hdr->next = hdr;
    else
        hdr->next = (Header *)(first_arena + 1);
}

/**
 * Checks if the given free block should be split in two separate blocks.
 * @param hdr       header of the free block
 * @param size      requested size of data
 * @return true if the block should be split
 * @pre hdr->asize == 0
 * @pre size > 0
 */
static bool hdr_should_split(Header *hdr, size_t size)
{
    if (hdr->asize - sizeof(Header) - size >= sizeof(Header) + MIN_ASIZE)
        return true;
    else
        return false;
}

/**
 * Splits one block in two.
 * @param hdr       pointer to header of the big block
 * @param req_size  requested size of data in the (left) block.
 * @return pointer to the new (right) block header.
 * @pre   (hdr->size >= req_size + 2*sizeof(Header))
 */
/**
 * Before:        |---- hdr->size ---------|
 *
 *    -----+------+------------------------+----
 *         |Header|........................|
 *    -----+------+------------------------+----
 *            \----hdr->next---------------^
 */
/**
 * After:         |- req_size -|
 *
 *    -----+------+------------+------+----+----
 *     ... |Header|............|Header|....|
 *    -----+------+------------+------+----+----
 *             \---next--------^  \--next--^
 */
static Header *hdr_split(Header *hdr, size_t req_size)
{
    hdr->asize = req_size;
    char *tmp = (char *)hdr;
    tmp += hdr->asize + sizeof(Header);
    
    ((Header *)tmp)->next = hdr->next;
    ((Header *)tmp)->size = hdr->size - sizeof(Header) - hdr->asize;
    ((Header *)tmp)->asize = 0;

    hdr->next = (Header *)tmp;
    hdr->size = hdr->asize + sizeof(Header);
    return (Header *)tmp;
}

/**
 * Detect if two adjacent blocks could be merged.
 * @param left      left block
 * @param right     right block
 * @return true if two block are free and adjacent in the same arena.
 * @pre left->next == right
 * @pre left != right
 */
static bool hdr_can_merge(Header *left, Header *right)
{
    if (left->asize == 0 && right->asize == 0)
        return true;
    else
        return false;
}

/**
 * Merge two adjacent free blocks.
 * @param left      left block
 * @param right     right block
 * @pre left->next == right
 * @pre left != right
 */
static void hdr_merge(Header *left, Header *right)
{
    left->asize += right->size;
    left->size = left->asize + sizeof(Header);
    left->next = right->next;
}

/**
 * Finds the free block that fits best to the requested size.
 * @param size      requested size
 * @return pointer to the header of the block or NULL if no block is available.
 * @pre size > 0
 */
static Header *best_fit(size_t size)
{
    if (first_arena == NULL)
        return NULL;

    Header *tmp = (Header *)(first_arena + 1);
    size_t *best = NULL;
    bool flag = true;
    while (tmp != ((Header *)(first_arena + 1)) || flag)
    {
        if (tmp->size - sizeof(Header) >= size && tmp->asize == 0 && (best == NULL || tmp->asize < *best))
        {
            best = &tmp->asize;
        }
        tmp = tmp->next;
        flag = false;
    }
    if (best == NULL)
        return NULL;
    else
    {
        return (Header *)(best - 2); // same as &(best[-2])
    }
}

/**
 * Search the header which is the predecessor to the hdr. Note that if
 * @param hdr       successor of the search header
 * @return pointer to predecessor, hdr if there is just one header.
 * @pre first_arena != NULL
 * @post predecessor->next == hdr
 */
static Header *hdr_get_prev(Header *hdr)
{

    if (first_arena == NULL)
        return NULL;
    Header *tmp = (Header *)(first_arena + 1);
    if (tmp == hdr)
        return NULL;
    while (tmp->next != NULL)
    {
        if (tmp->next == hdr)
            return tmp;
        else
            tmp = tmp->next;
    }
    return NULL;
}

/**
 * Allocate memory. Use best-fit search of available block.
 * @param size      requested size for program
 * @return pointer to allocated data or NULL if error or size = 0.
 */
void *mmalloc(size_t size)
{
    if (size == 0)
        return NULL;
    Header *elem = best_fit(size);
    if (elem == NULL)
    {
        Arena *tmp = arena_alloc(ARENA_SIZE);
        if (tmp == NULL)
            return NULL;

        arena_append(tmp);
        tmp = tmp + 1;
        hdr_ctor((Header *)tmp, ARENA_SIZE - sizeof(Arena));
        hdr_split((Header *)tmp, size);
        return (void *) ((Header *)tmp + 1);
    }
    else
    {
        if (hdr_should_split(elem, size))
        {
            hdr_split(elem, size);
            return (void *)(elem + 1);
        }
        else
        {
            elem->asize = size;
            elem->size = size + sizeof(Header);
            return (void *)(elem + 1);
        }
    }
}

/**
 * Free memory block.
 * @param ptr       pointer to previously allocated data
 * @pre ptr != NULL
 */
void mfree(void *ptr)
{
    (void)ptr;
    // FIXME
}

/**
 * Reallocate previously allocated block.
 * @param ptr       pointer to previously allocated data
 * @param size      a new requested size. Size can be greater, equal, or less
 * then size of previously allocated block.
 * @return pointer to reallocated space or NULL if size equals to 0.
 * @post header_of(return pointer)->size == size
 */
void *mrealloc(void *ptr, size_t size)
{
    // FIXME
    (void)ptr;
    (void)size;
    return NULL;
}
