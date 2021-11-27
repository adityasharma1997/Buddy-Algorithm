#include "malloc.h"
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
//#include <stdlib.h>

void *func(void *args) {
	int i=*((int *)args);
	void *p = malloc(50);
	void *p1 = realloc(p, 100);
   
	printf("-----Thread %d allocated 100 bytes -- address --at %p\n",i,p1);
	free(p);
	return NULL;
}

int main() {
	pthread_t tid[50];
	for(int i=0;i<50;i++){
		pthread_create(&tid[i], NULL, func,  (void *)&i);
	}
	for(int i=0;i<50;i++){
		pthread_join(tid[i], NULL);
	}
    
	return 0;
}