#include "malloc.h"
void *reallocarray(void *ptr, size_t nmemb, size_t size) {
    size_t total;
    total = nmemb*size;
  return realloc(ptr, total);
}