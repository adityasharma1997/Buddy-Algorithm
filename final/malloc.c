#include "malloc.h"
__thread struct arena Arena;
pthread_mutex_t lock;
void *malloc(size_t user_size){
    meta_data *info;
    void *ptr;
    size_t total_size = user_size + sizeof(meta_data);
    if(total_size<=sizeof(meta_data)){      // when user asked for 0
       if(user_size < 0){
           char buff[30]="User asked for invalid size";
           write(STDOUT_FILENO,buff,sizeof(buff));
       }
        return NULL;
    }
    if(total_size>PAGESIZE){
        info = mmap(0, total_size, PROT_READ | PROT_WRITE,
               MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);     // when user asked for more than pagesize

        info->block_size=total_size;
        info->isFree=0;
        info->allocation_size=total_size;
        ptr = (void *)((char *)info+sizeof(meta_data));
        return ptr;
    }
    int index = 3;
    int block_size = MIN_BLOCK;
    while(block_size<total_size){
        block_size=block_size*2;
        index=index+1;
    }
   pthread_mutex_lock(&lock);
    if(Arena.block_list[index]){
            info=Arena.block_list[index];
            popFront(index);
            pthread_mutex_unlock(&lock);
            info->isFree=0;
            info->allocation_size=total_size;
            ptr = (void *)((char *)info+sizeof(meta_data));

            if (Arena.heap == NULL) Arena.heap = ptr;
            return ptr;
    }
    else{
        index++;
        while(index<=12){
            if(Arena.block_list[index]){
                split(index);
		pthread_mutex_unlock(&lock);
                return malloc(user_size);

            }
            index++;

        }
    }
    info=sbrk(PAGESIZE);
     if (info == (void *)-1) {
        char buff[30]="Allocation error";
        write(STDOUT_FILENO,buff,sizeof(buff));
        return info;
    }
    
    info->block_size=PAGESIZE;
    info->isFree=1;
    info->next=NULL;
    info->allocation_size=0;

  

    Arena.block_list[12]=info;
    pthread_mutex_unlock(&lock);
   return malloc(user_size);

}

__attribute__((constructor)) void init() {
  Arena.heap = NULL;
  pthread_mutex_init(&lock, NULL);
}
