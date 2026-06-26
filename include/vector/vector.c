#include <vector/vector.h>
#include <stdlib.h>
#include <string.h>

#define VECTOR_INITIAL_CAPACITY 4

void vector_init(vector_t *v, size_t elem_size)
{
	v->data = NULL;
	v->size = 0;
	v->capacity = 0;
	v->elem_size = elem_size;
}

void vector_free(vector_t *v)
{
	free(v->data);
	v->data = NULL;
	v->size = 0;
	v->capacity = 0;
}

static int vector_grow(vector_t *v)
{
	size_t new_cap = v->capacity == 0 ? VECTOR_INITIAL_CAPACITY : v->capacity * 2;
	void *new_data = realloc(v->data, new_cap * v->elem_size);
	if (!new_data)
		return -1;
	v->data = new_data;
	v->capacity = new_cap;
	return 0;
}

void *vector_push(vector_t *v)
{
	if (v->size == v->capacity) {
		if (vector_grow(v) != 0)
			return NULL;
	}
	char *ptr = (char *)v->data + v->size * v->elem_size;
	v->size++;
	return ptr;
}

void *vector_get(vector_t *v, size_t index)
{
	if (index >= v->size)
		return NULL;
	return (char *)v->data + index * v->elem_size;
}

void vector_remove(vector_t *v, size_t index)
{
	size_t remaining;

	if (index >= v->size)
		return;

	remaining = v->size - index - 1;
	if (remaining > 0) {
		char *dst = (char *)v->data + index * v->elem_size;
		char *src = dst + v->elem_size;
		memmove(dst, src, remaining * v->elem_size);
	}
	v->size--;
}

void vector_remove_fast(vector_t *v, size_t index)
{
	if (index >= v->size)
		return;

	v->size--;
	if (index < v->size) {
		char *dst = (char *)v->data + index * v->elem_size;
		char *src = (char *)v->data + v->size * v->elem_size;
		memcpy(dst, src, v->elem_size);
	}
}

void vector_clear(vector_t *v)
{
	v->size = 0;
}