//#include <assert.h>
#include <stdio.h>
#include "malloc.h"
//#include <stdlib.h>

int main(int argc, char **argv)
{
  size_t size = 120;
  void *mem = malloc(size);
  size_t size1 = 120;
  void *mem1 = malloc(size);
  size_t size2 = 252;
  void *mem2 = malloc(size);
  size_t size3 = 120;
  void *mem3 = malloc(size);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size, mem);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size1, mem1);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size2, mem2);
  printf("Successfully malloc'd %zu bytes at addr %p\n", size3, mem3);
  //assert(mem != NULL);
  free(mem);
  printf("Successfully free'd %zu bytes from addr %p\n", size, mem);
  return 0;
}