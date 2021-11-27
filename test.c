/**
 * Test file for checking threads' requests for memory
 */

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include "malloc.h"

void *func() {
	void *p = malloc(50);
    printf("-----Thread 1 Done---%p-\n",p);
	free(p);
	return NULL;
}

void *func1() {
	void *p = malloc(50);
    printf("-----Thread 2 Done---%p-\n",p);
	free(p);
	return NULL;
}

int main() {
	pthread_t t1, t2;
	pthread_create(&t1, NULL, func,  NULL);
	pthread_create(&t2, NULL, func1, NULL);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	//malloc_stats();
	return 0;
}