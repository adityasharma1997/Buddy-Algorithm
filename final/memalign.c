#include "malloc.h"
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

            ((meta_data *)((void *)((char *)newone-sizeof(meta_data))))->data_address=temp2;
            return newone;

        }else{
            return NULL;
        }

}