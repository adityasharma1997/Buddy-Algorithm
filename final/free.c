#include "malloc.h"
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