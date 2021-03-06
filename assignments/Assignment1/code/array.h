#ifndef ARRAY_H_
#define ARRAY_H_

struct array {
  size_t size;
  size_t capacity;
  void *array_ptr;
  size_t item_size;
};

void push_to_array(struct array *array, void *item);

void *get_from_array(const struct array *array, size_t index);

struct array create_array(size_t item_size);

void free_array(struct array *array);

#endif // ARRAY_H_