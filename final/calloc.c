#include "malloc.h"
void *calloc(size_t nmemb, size_t size){
  size_t totalSize = nmemb * size;
  void *p = malloc(totalSize);
  if(p == NULL) {
  	return NULL;
  }
  if(p!=NULL){
  memset(p, 0, totalSize);
  }
  return p;
}
