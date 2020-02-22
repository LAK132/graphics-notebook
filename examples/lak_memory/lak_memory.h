#ifndef LAK_MEMORY_H
#define LAK_MEMORY_H

/* size_t */
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
  size_t size;
  void *data;
} lak_array_t;

void lak_array_malloc(lak_array_t *array, size_t size);

void lak_array_free(lak_array_t *array);

void *lak_malloc(size_t size);

void lak_free(void *data);

#ifdef __cplusplus
}
#endif

#endif