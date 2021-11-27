#include "malloc.h"
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
return 0;
}
