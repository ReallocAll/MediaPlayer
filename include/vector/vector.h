#pragma once

#include <stddef.h>

typedef struct {
	void  *data;
	size_t size;
	size_t capacity;
	size_t elem_size;
} vector_t;

/* Initialize a vector. Must be called before any other operation. */
void vector_init(vector_t *v, size_t elem_size);

/* Free all memory owned by the vector. */
void vector_free(vector_t *v);

/* Append an element, returns pointer to the new element (caller fills it). */
void *vector_push(vector_t *v);

/* Get a pointer to the element at index. */
void *vector_get(vector_t *v, size_t index);

/* Remove the element at index, shifting subsequent elements down (stable). */
void vector_remove(vector_t *v, size_t index);

/* Remove the element at index by swapping with the last element (fast, O(1)). */
void vector_remove_fast(vector_t *v, size_t index);

/* Clear all elements (does not free memory). */
void vector_clear(vector_t *v);