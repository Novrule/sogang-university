/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include "mm.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your information in the following struct.
 ********************************************************/
team_t team = {
    /* Your student ID */
    "20191282",
    /* Your full name*/
    "Wonjun Han",
    /* Your email address */
    "gksdnjswns@sogang.ac.kr",
};

// Basic constants and macros
#define WSIZE 4
#define DSIZE 8
#define CHUNKSIZE (1 << 12)

#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define PACK(size, alloc) ((size) | (alloc))

#define GET(p) (*(unsigned int *)(p))
#define PUT(p, val) (*(unsigned int *)(p) = (val))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)

#define HDRP(bp) ((char *)(bp) - WSIZE)
#define FTRP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

#define NEXT_BLKP(bp) (((char *)(bp) + GET_SIZE((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp) (((char *)(bp) - GET_SIZE((char *)(bp) - DSIZE)))

#define BUFFER (1 << 7)

static void *heap_listp;
static char *last_bp;

static void *extend_heap(size_t words);
static void *coalesce(void *bp);
static void *find_fit(size_t adjusted_size);
static void place(void *bp, size_t adjusted_size);

/*
 * mm_init - initialize the malloc package.
 */
int mm_init(void) {
  if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void *)-1) {
    return -1;
  }

  PUT(heap_listp, 0);
  PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));
  PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));
  PUT(heap_listp + (3 * WSIZE), PACK(0, 1));
  heap_listp += (2 * WSIZE);

  if (extend_heap(CHUNKSIZE / WSIZE) == NULL) {
    return -1;
  } else {
    last_bp = (char *)heap_listp;

    return 0;
  }
}

/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size) {
  size_t asize;
  size_t extendsize;
  char *bp;

  if (size == 0) {
    return NULL;
  }

  if (size <= DSIZE) {
    asize = 2 * DSIZE;
  } else {
    asize = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
  }

  if ((bp = find_fit(asize)) != NULL) {
    place(bp, asize);
    last_bp = bp;

    return bp;
  }

  extendsize = MAX(asize, CHUNKSIZE);

  if ((bp = extend_heap(extendsize / WSIZE)) == NULL) {
    return NULL;
  }

  place(bp, asize);
  last_bp = bp;

  return bp;
}

static void *find_fit(size_t adjusted_size) {
  char *bp = last_bp;

  for (bp = NEXT_BLKP(bp); GET_SIZE(HDRP(bp)) != 0; bp = NEXT_BLKP(bp)) {
    if (!GET_ALLOC(HDRP(bp)) && GET_SIZE(HDRP(bp)) >= adjusted_size) {
      last_bp = bp;

      return bp;
    }
  }

  bp = heap_listp;
  while (bp < last_bp) {
    bp = NEXT_BLKP(bp);
    if (!GET_ALLOC(HDRP(bp)) && GET_SIZE(HDRP(bp)) >= adjusted_size) {
      last_bp = bp;

      return bp;
    }
  }

  return NULL;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp) {
  size_t size = GET_SIZE(HDRP(bp));

  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  coalesce(bp);
}

static void *extend_heap(size_t words) {
  char *bp;
  size_t size;

  size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
  if ((long)(bp = mem_sbrk(size)) == -1) {
    return NULL;
  }

  PUT(HDRP(bp), PACK(size, 0));
  PUT(FTRP(bp), PACK(size, 0));
  PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));

  return coalesce(bp);
}

/*
 * coalesce - Coalesce free blocks.
 */
static void *coalesce(void *bp) {
  size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  if (prev_alloc && next_alloc) {
    last_bp = bp;

    return bp;
  } else if (prev_alloc && !next_alloc) {
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
  } else if (!prev_alloc && next_alloc) {
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  } else {
    size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(FTRP(NEXT_BLKP(bp)));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    bp = PREV_BLKP(bp);
  }

  last_bp = bp;

  return bp;
}

static void place(void *bp, size_t adjusted_size) {
  size_t csize = GET_SIZE(HDRP(bp));

  if ((csize - adjusted_size) >= (2 * (DSIZE))) {
    PUT(HDRP(bp), PACK(adjusted_size, 1));
    PUT(FTRP(bp), PACK(adjusted_size, 1));
    bp = NEXT_BLKP(bp);
    PUT(HDRP(bp), PACK(csize - adjusted_size, 0));
    PUT(FTRP(bp), PACK(csize - adjusted_size, 0));
  } else {
    PUT(HDRP(bp), PACK(csize, 1));
    PUT(FTRP(bp), PACK(csize, 1));
  }
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size) {
  void *new_ptr;
  size_t new_size;
  size_t copy_size;

  if (size == 0) {
    mm_free(ptr);

    return NULL;
  }

  if (ptr == NULL) {
    return mm_malloc(size);
  }

  if (size <= DSIZE) {
    new_size = 2 * DSIZE;
  } else {
    new_size = DSIZE * ((size + (DSIZE) + (DSIZE - 1)) / DSIZE);
  }

  new_size += BUFFER;

  int blkBUF = GET_SIZE(HDRP(ptr)) - new_size;

  if (blkBUF < 0) {
    if (!GET_ALLOC(HDRP(NEXT_BLKP(ptr))) || !GET_SIZE(HDRP(NEXT_BLKP(ptr)))) {
      size_t next_size = GET_SIZE(HDRP(NEXT_BLKP(ptr)));
      size_t total_size = GET_SIZE(HDRP(ptr)) + next_size;

      if (total_size >= new_size) {
        PUT(HDRP(ptr), PACK(total_size, 1));
        PUT(FTRP(ptr), PACK(total_size, 1));

        return ptr;
      } else {
        size_t extendsize = MAX(new_size - total_size, CHUNKSIZE);

        if (extend_heap(extendsize / WSIZE) == NULL) {
          return NULL;
        }

        total_size += extendsize;
        PUT(HDRP(ptr), PACK(total_size, 1));
        PUT(FTRP(ptr), PACK(total_size, 1));

        return ptr;
      }
    } else {
      new_ptr = mm_malloc(new_size);

      if (new_ptr == NULL) {
        return NULL;
      }

      copy_size = MIN(size, GET_SIZE(HDRP(ptr)) - WSIZE);
      memmove(new_ptr, ptr, copy_size);
      mm_free(ptr);

      return new_ptr;
    }
  }

  return ptr;
}
