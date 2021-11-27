#include "malloc.h"
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