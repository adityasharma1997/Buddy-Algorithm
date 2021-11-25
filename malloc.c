#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <errno.h>


#define PAGESIZE sysconf(_SC_PAGESIZE)
#define MIN_BLOCK 8

typedef struct meta_data{
    size_t block_size;
    struct meta_data *next;
    bool isFree;
    size_t allocation_size;
    void *data_address; //to handle the address shift in memalign and posix

} meta_data;

meta_data *block_list[13];

void popFront(int index){
    meta_data *node = block_list[index];
    block_list[index]=node->next;
    node->next=NULL;
}

void split(int index){
    meta_data *temp = block_list[index];
    popFront(index);
    meta_data *temp2 = (meta_data *)((char *)(temp) +((temp->block_size)/2));
    temp->block_size=temp->block_size/2;
    temp2->block_size=temp->block_size;
    temp2->isFree=1;
    //temp->isFree=1;
    temp->next=temp2;
    temp2->next=NULL;
    temp2->allocation_size=0;
    block_list[index-1]=temp;
    // we are supposing the smaller list will never be present that's why we directl pointing it.

}

void *custom_malloc(size_t user_size){
    meta_data *info;
    void *ptr;
    size_t total_size = user_size + sizeof(meta_data);
    if(total_size<=sizeof(meta_data)){      // when user asked for 0
        printf("User asked for invalid size");
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
    if(block_list[index]){
            //allocate and pop
            info=block_list[index];
            popFront(index);
            info->isFree=0;
            info->allocation_size=total_size;
            ptr = (void *)((char *)info+sizeof(meta_data));
            return ptr;
    }
    else{
        index++;
        while(index<=12){
            if(block_list[index]){
                split(index);

                return custom_malloc(user_size);

            }
            index++;

        }
    }
    info=sbrk(PAGESIZE);
     if (info == (void *)-1) {
         printf("Allocation error");
        return info;
    }
    
    info->block_size=PAGESIZE;
    info->isFree=1;
    info->next=NULL;
    info->allocation_size=0;

  

    block_list[12]=info;

    return custom_malloc(user_size);

    

}

void pushFront(int index,meta_data *block){
    block->next = block_list[index];
    block_list[index]=block;
    block->isFree=1;
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
    current = block_list[index];
    if(current==buddy){
        popFront(index);
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
            merge(temp);
        }else{
            munmap(temp, s);
        }


    }

}


void *calloc(size_t nmemb, size_t size){
  size_t totalSize = nmemb * size;
  void *p = custom_malloc(totalSize);
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
        void *ans = custom_malloc(size);
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
    new = custom_malloc(size);
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
        temp=custom_malloc(size+alignment+sizeof(meta_data));
        temp2=temp;
        if(temp!=NULL){
            void *newone;

            unsigned long len = (unsigned long)temp;
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

//TODO:- rename malloc,thread,8 byte alignment,error handling, seperate header file,makefile,interactive terminal

int main(){
    // int *p=(int*)calloc(4,sizeof(int));
    // for(int i=0;i<4;i++){
    //     p[i]=i*2;
    // }
    // printf("\n");
    // realloc(p,2*sizeof(int));
    // for(int i=0;i<2;i++){
    //     printf("%d",p[i]);
    // }
    size_t s=1500;
void *mem=NULL;
posix_memalign(&mem,128,s);
printf("Successfully malloc'd %zu bytes at addr %p\n", s, mem);
free(mem);
printf("Successfully free'd %zu bytes from addr %p\n", s, mem);
// printf("%ld",sizeof(meta_data));
//  size_t size = 5800;
//   void *mem = custom_malloc(size);
//   size_t size1 = 800;
//   void *mem2= custom_malloc(size1);
//   size_t size2 = 300;
//   void *mem3= custom_malloc(size2);
//   size_t size3 = 300;
//   void *mem4= custom_malloc(size3);
//   size_t size4 = 800;
//   void *mem5= custom_malloc(size4);
//   printf("Successfully malloc'd %zu bytes at addr %p\n", size, mem);
//   printf("Successfully malloc'd %zu bytes at addr %p\n", size1, mem2);
//   printf("Successfully malloc'd %zu bytes at addr %p\n", size2, mem3);
//   printf("Successfully malloc'd %zu bytes at addr %p\n", size3, mem4);
//   printf("Successfully malloc'd %zu bytes at addr %p\n", size4, mem5);
//   free(mem);
//   free(mem2);
//   free(mem3);
//   free(mem4);
//   free(mem5);
//   printf("Successfully free'd %zu bytes from addr %p\n", size, mem);
//    printf("Successfully malloc'd %zu bytes at addr %p\n", size1, mem2);
//   printf("Successfully malloc'd %zu bytes at addr %p\n", size2, mem3);
//   printf("Successfully malloc'd %zu bytes at addr %p\n", size3, mem4);
//   printf("Successfully malloc'd %zu bytes at addr %p\n", size4, mem5);

return 0;
}