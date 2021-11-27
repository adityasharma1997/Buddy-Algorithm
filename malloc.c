#include "malloc.h"
#include<pthread.h>

pthread_mutex_t lock;


// typedef struct meta_data{
//     size_t block_size;
//     struct meta_data *next;
//     bool isFree;
//     size_t allocation_size;
//     void *data_address; //to handle the address shift in memalign and posix

// } meta_data;

// meta_data *block_list[13];

void print_msg(const char *buf)
{
    write(STDOUT_FILENO, buf, strlen(buf) + 1);
}

void popFront(int index){
    meta_data *node = Arena.block_list[index];
    Arena.block_list[index]=node->next;
    node->next=NULL;
}

void split(int index){
    // pthread_mutex_lock(&lock);
    meta_data *temp = Arena.block_list[index];
    popFront(index);
    meta_data *temp2 = (meta_data *)((char *)(temp) +((temp->block_size)/2));
    temp->block_size=temp->block_size/2;
    temp2->block_size=temp->block_size;
    temp2->isFree=1;
    //temp->isFree=1;
    temp->next=temp2;
    temp2->next=NULL;
    temp2->allocation_size=0;
    Arena.block_list[index-1]=temp;
//    pthread_mutex_unlock(&lock);
    // we are supposing the smaller list will never be present that's why we directl pointing it.

}

void *malloc(size_t user_size){
    meta_data *info;
    void *ptr;
    size_t total_size = user_size + sizeof(meta_data);
    if(total_size<=sizeof(meta_data)){      // when user asked for 0
        //printf("User asked for invalid size");
       // print_msg();
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
    //TO DO:-8 byte allignment later 
    int index = 3;
    int block_size = MIN_BLOCK;
    while(block_size<total_size){
        block_size*=2;
        index++;
    }
   // index--;
   pthread_mutex_lock(&lock);
    if(Arena.block_list[index]){
            //allocate and pop
            info=Arena.block_list[index];
            // pthread_mutex_unlock(&lock);
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
         //printf("Allocation error");
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

void pushFront(int index,meta_data *block){
    //pthread_mutex_lock(&lock);
    block->next = Arena.block_list[index];
    Arena.block_list[index]=block;
    block->isFree=1;
    //pthread_mutex_unlock(&lock);
}

int power(int x,int y) {
    if(y==0){
        return 1;
    }else if(y%2==0){
        return power(x,y/2)*power(x,y/2);
    }else{
        return x*power(x,y/2)*power(x,y/2);
    }
}

meta_data *getBuddy(meta_data *block){
     size_t limit =block->block_size;
    int i=8;
    int index=3;
    while(i<limit){
        i=i*2;
        index++;
    }
    void *buddy = (void *)((uintptr_t)block ^ (uintptr_t)power(2,index));
    meta_data *b = (meta_data *) buddy;
    return b;
}


// int log_2(int n) {
//   int i = 0;
//   while (n >> (i + 1)) i++;
//   return i;
// }

// meta_data *getBuddy(meta_data *block) {
//   unsigned long offset = (unsigned long)(((char *)block) - (char *)Arena.heap);
//   unsigned long mask = (unsigned long)(0x1 << log_2(block->allocation_size));
//   return (meta_data *)((offset ^ mask) + (char *)Arena.heap);
// }

void merge(meta_data *block){
    size_t limit =block->block_size;
    int i=8;
    int index=3;
    while(i<limit){
        i=i*2;
        index++;
    }
    if(index==12){
        pushFront(index,block);
        return;
    }

   
    meta_data *buddy;
    buddy=getBuddy(block);
    size_t first,second;
    first=block->block_size;
    second=buddy->block_size;
    
    if((buddy->isFree==0)|| (first!=second)){
        pushFront(index,block);
        return;
    }
    meta_data *current;
    current = Arena.block_list[index];
    if(current==buddy){
       // pthread_mutex_lock(&lock);
        popFront(index);
        //pthread_mutex_unlock(&lock);
    }else if(current!=buddy){
        while(current->next!=buddy){
            meta_data *temp;
            temp=current->next;
            current=temp;
            //current=current->next;

        }
        current->next=buddy->next;
        buddy->next=NULL;
    }
    
    if (block < buddy) {
    block->block_size = block->block_size*2;
    merge(block);
  } else if(block>=buddy) {
    buddy->block_size = buddy->block_size* 2;
    merge(buddy);
  }

}


void free(void *ptr){

    if(ptr==NULL){
        return;
    }else{
        meta_data *temp;

        temp = (meta_data *)((char *)ptr-sizeof(meta_data));
        if(temp->data_address!=NULL){
            return free(temp->data_address);
        }
        size_t s;
        s= temp->block_size;
        if(s<=PAGESIZE){
            temp->isFree=1;
            pthread_mutex_lock(&lock);
            merge(temp);
            pthread_mutex_unlock(&lock);
        }else{
            munmap(temp, s);
        }


    }

}


void *calloc(size_t nmemb, size_t size){
  size_t totalSize = nmemb * size;
  void *p = malloc(totalSize);
  if(p == NULL) {
  	return NULL;
  }
  if(p!=NULL){
 // pthread_mutex_lock(&mutex);
  memset(p, 0, totalSize);
  //pthread_mutex_unlock(&mutex);
  }
  return p;
}


void *realloc(void *ptr, size_t size){
    if(ptr==NULL){
        void *ans = malloc(size);
        return ans;
    }
    if(ptr!=NULL && size==0){
        free(ptr);
        return NULL;
    }
    meta_data *temp;
    temp=(meta_data *)((char *)ptr-sizeof(meta_data));
    if(temp->block_size>=size){
        return ptr;
    }
    void *new;
    new = malloc(size);
    memcpy((char *)new, ptr, temp->allocation_size);
    free(ptr);

    return new;
}

void *reallocarray(void *ptr, size_t nmemb, size_t size) {
    size_t total;
    total = nmemb*size;
  return realloc(ptr, total);
}


void *memalign(size_t alignment, size_t size){
        void *temp;
        void *temp2;
        void *base;
        temp=malloc(size+alignment+sizeof(meta_data));
        temp2=temp;
        if(temp!=NULL){
            void *newone;
            base = (char*)temp + sizeof(meta_data);
            unsigned long len = (unsigned long)base;
            unsigned long a = len%(unsigned long)alignment;
            len = len-a;
            if(a!=0){
            newone = (void *)(len+alignment);
            }else{
                newone = (void *)(len);
            }
            //((MallocHeader *)((void *)((char *)ret - MALLOC_HEADER_SIZE)))->header_addr = ptr;

            ((meta_data *)((void *)((char *)newone-sizeof(meta_data))))->data_address=temp2;
            return newone;

        }else{
            return NULL;
        }

}

int posix_memalign(void **memptr, size_t alignment, size_t size){
    void *ret;
    ret = memalign(alignment, size);
    if (ret!=NULL) {
    *memptr = ret;
    return 0;
    } 
    else if(ret == NULL) {
    return errno;
  }
}


// int main(int argc, char **argv)
// {
//   size_t size = 120;
//   void *mem = custom_malloc(size);
//   size_t size1 = 120;
//   void *mem1 = custom_malloc(size);
//   size_t size2 = 252;
//   void *mem2 = custom_malloc(size);
//   size_t size3 = 120;
//   void *mem3 = custom_malloc(size);
//   printf("Successfully malloc'd %zu bytes at addr %p\n", size, mem);
//   printf("Successfully malloc'd %zu bytes at addr %p\n", size1, mem1);
//   printf("Successfully malloc'd %zu bytes at addr %p\n", size2, mem2);
//   printf("Successfully malloc'd %zu bytes at addr %p\n", size3, mem3);
//   //assert(mem != NULL);
//   free(mem);
//   printf("Successfully free'd %zu bytes from addr %p\n", size, mem);
//   return 0;
// }
