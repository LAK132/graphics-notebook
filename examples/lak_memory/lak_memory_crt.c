#include "lak_memory.h"

#include <stdlib.h>

void lak_array_malloc(lak_array_t *array, size_t size)
{
  array->data = lak_malloc(size);
  array->size = size;
}

void lak_array_free(lak_array_t *array)
{
  lak_free(array->data);
  array->data = NULL;
  array->size = 0;
}

void *lak_malloc(size_t size)
{
  return malloc(size);
}

void lak_free(void *data)
{
  free(data);
}

