#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <pthread.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <errno.h>



#define PAGESIZE sysconf(_SC_PAGESIZE)
#define MIN_BLOCK 8

extern pthread_mutex_t lock;

typedef struct meta_data{
    size_t block_size;
    struct meta_data *next;
    bool isFree;
    size_t allocation_size;
    void *data_address; //to handle the address shift in memalign and posix

} meta_data;

struct arena{
    void *heap;
meta_data *block_list[13];
};

extern __thread struct arena Arena;

void split(int index);
void popFront(int index);
void *malloc(size_t user_size);
void pushFront(int index,meta_data *block);
int power(int x,int y);
meta_data *getBuddy(meta_data *block);
void merge(meta_data *block);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void *reallocarray(void *ptr, size_t nmemb, size_t size);
void *memalign(size_t alignment, size_t size);
int posix_memalign(void **memptr, size_t alignment, size_t size);
