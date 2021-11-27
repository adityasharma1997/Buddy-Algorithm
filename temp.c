#include "malloc.h"
#include "utils.h"
#include "utils.h"
#include "malloc.h"

MemArena arena;

int log_2(int n) {
  int i = 0;
  while (n >> (i + 1)) i++;
  return i;
}

void *roundUp(void *addr, size_t alignment) {
  return (void *)((((ul)addr + sizeof(ul)) & ~(ul)(alignment - 1)) + alignment);
}

void popFront(int index) {
  MallocHeader *hdr = arena.freeList[index];
  arena.freeList[index] = hdr->next;
  hdr->next = NULL;
  hdr->is_free = 0;
}

void splitBlock(int index) {
  MallocHeader *blockA = arena.freeList[index];
  blockA->size /= 2;
  blockA->is_free = 1;
  popFront(index);

  MallocHeader *blockB = (MallocHeader *)((char *)blockA + blockA->size);
  blockB->size = blockA->size;
  blockB->is_free = 1;

  blockA->next = blockB;
  blockB->next = NULL;
  arena.freeList[index - 1] = blockA;
}

MallocHeader *getBuddy(MallocHeader *block) {
  ul offset = (ul)(((char *)block) - (char *)arena.heap);
  ul mask = (ul)(0x1 << log_2(block->size));
  return (MallocHeader *)((offset ^ mask) + (char *)arena.heap);
}

void pushFront(int index, MallocHeader *block) {
  block->next = arena.freeList[index];
  arena.freeList[index] = block;
  block->is_free = 1;
}

void mergeBlock(MallocHeader *block) {
  int index = log_2(block->size / MIN_BLOCK);
  if (index >= MAX_ORDER) {
    pushFront(index, block);
    return;
  }

  MallocHeader *buddy = getBuddy(block);
  if (!buddy->is_free || buddy->size != block->size) {
    pushFront(index, block);
    return;
  }

  MallocHeader *cur = arena.freeList[index];
  if (cur == buddy) {
    popFront(index);
  } else {
    while (cur->next != buddy) cur = cur->next;
    cur->next = buddy->next;
    buddy->next = NULL;
  }

  if (block < buddy) {
    block->size *= 2;
    mergeBlock(block);
  } else {
    buddy->size *= 2;
    mergeBlock(block);
  }
}


typedef struct MallocHeader {
  size_t size;
  struct MallocHeader *next;
  int is_free;
  void* header_addr;
} MallocHeader;


MemArena arena;
pthread_mutex_t malloc_lock;

void *buddy_malloc(size_t allocSize) {
  MallocHeader *hdr;

  int blockSize = MIN_BLOCK;
  int order = 0;

  while (blockSize < allocSize) {
    blockSize *= 2;
    order++;
  }

  pthread_mutex_lock(&malloc_lock);

  if (arena.freeList[order]) {
    hdr = arena.freeList[order];
    popFront(order);
    pthread_mutex_unlock(&malloc_lock);
    return hdr;
  }

  while (++order <= MAX_ORDER) {
    if (arena.freeList[order]) {
      splitBlock(order);
      pthread_mutex_unlock(&malloc_lock);
      return buddy_malloc(allocSize);
    }
  }

  hdr = sbrk(BUDDY_SIZE);
  if (hdr == (void *)-1) {
    return hdr;
  }

  hdr->size = BUDDY_SIZE;
  hdr->next = NULL;
  hdr->is_free = 1;
  arena.freeList[MAX_ORDER] = hdr;
  pthread_mutex_unlock(&malloc_lock);

  return buddy_malloc(allocSize);
}

void *malloc(size_t size) {
  void *ret;
  size_t allocSize = size + MALLOC_HEADER_SIZE;

  if (allocSize <= MALLOC_HEADER_SIZE) {
    return NULL;
  } else if (allocSize <= BUDDY_SIZE) {
    ret = buddy_malloc(allocSize);
    assert(ret != (void *)-1);
  } else if (allocSize > BUDDY_SIZE) {
    ret = mmap(0, allocSize, PROT_READ | PROT_WRITE,
               MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    assert(ret != MAP_FAILED);
  }

#if DEBUG
  // Can't use printf here because printf internally calls `malloc`
  char buf[1024];
  snprintf(buf, 1024, "%s:%d malloc(%zu): Allocated %zu bytes at %p\n",
           __FILE__, __LINE__, size, allocSize, ret);
  write(STDOUT_FILENO, buf, strlen(buf) + 1);
#endif

  MallocHeader *hdr = (MallocHeader *)ret;
  hdr->size = allocSize;

  if (arena.heap == NULL) arena.heap = ret;

  return ret + MALLOC_HEADER_SIZE;
}

__attribute__((constructor)) void init() {
  arena.heap = NULL;
  pthread_mutex_init(&malloc_lock, NULL);
}


#include "malloc.h"
#include "utils.h"

MemArena arena;
pthread_mutex_t malloc_lock;

void free(void *ptr) {
  if (ptr == NULL) return;

  MallocHeader *hdr = MALLOC_BASE(ptr);
  if (hdr->header_addr) return free(hdr->header_addr);

  if (hdr->size <= BUDDY_SIZE) {
    if (hdr->is_free == 0) {
      hdr->is_free = 1;
      hdr->next = NULL;
      pthread_mutex_lock(&malloc_lock);
      mergeBlock(hdr);
      pthread_mutex_unlock(&malloc_lock);
    }
  } else if (hdr->size > BUDDY_SIZE) {
    munmap(hdr, hdr->size);
  }

#if DEBUG
  // Can't use printf here because printf internally calls `malloc`
  char buf[1024];
  snprintf(buf, 1024, "%s:%d free(%p): Freeing %zu bytes from %p\n", __FILE__,
           __LINE__, ptr, hdr->size, hdr);
  write(STDOUT_FILENO, buf, strlen(buf) + 1);
#endif
}


 (void *)((((ul)addr + sizeof(ul)) & ~(ul)(alignment - 1)) + alignment);


 (x & ~(a-1)) +a
 (x-xmoda) +a


 x=25 a=8
 x-xmoda+a
 25-25%8=25-1=24+8



((MallocHeader *)((void *)((char *)ret - MALLOC_HEADER_SIZE)))->header_addr = ptr;