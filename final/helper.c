#include "malloc.h"

void popFront(int index){
    meta_data *node;
    node = Arena.block_list[index];
    Arena.block_list[index]=node->next;
    node->next=NULL;
}

void split(int index){
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
}

void pushFront(int index,meta_data *block){
    block->next = Arena.block_list[index];
    Arena.block_list[index]=block;
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
    current = Arena.block_list[index];
    if(current==buddy){
        popFront(index);
    }else if(current!=buddy){
        while(current->next!=buddy){
            meta_data *temp;
            temp=current->next;
            current=temp;
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
