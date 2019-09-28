#include "array.h"
const size_t DEFAULT_CAPACITY = 1024;

void push_to_array(struct array *array, void *item) {
  if (array->item_size >= array->capacity) {
    array->array_ptr =
        realloc(array->array_ptr, array->item_size * array->capacity * 2);
    array->capacity *= 2;
  }
  memcpy(array->array_ptr + array->size * array->item_size, item,
         array->item_size);
  array->size += 1;
}

void *get(const struct array *array, size_t index) {
  return array->array_ptr + index * array->item_size;
}

struct array create_array(size_t item_size) {
  return (struct array){
      .size = 0,
      .capacity = DEFAULT_CAPACITY,
      .array_ptr = malloc(DEFAULT_CAPACITY * item_size),
      .item_size = item_size,

  };
}