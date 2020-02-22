#include "lak_memory.h"

#include <sys/mman.h>

void lak_array_malloc(lak_array_t *array, size_t size)
{
  array->data = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  array->size = size;
}

void lak_array_free(lak_array_t *array)
{
  munmap(array->data, array->size);
  array->data = NULL;
  array->size = 0;
}

typedef struct
{
  lak_array_t pointers;
  lak_array_t sizes;
} lak_allocations_t;

lak_array_t allocations;

void *lak_malloc(size_t size)
{

}

void lak_free(void *data)
{

}
