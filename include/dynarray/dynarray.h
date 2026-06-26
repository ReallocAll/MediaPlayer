#pragma once

#include <stddef.h>

#define DYNARRAY_ADD 0
#define DYNARRAY_DEL 1

typedef struct {
	unsigned long long curr_arr_size;
	size_t single_value_size;
	void *start_addr;
} dynarray_t;

void *dynarray_new(size_t in_size, dynarray_t *out_info);
int dynarray_op(dynarray_t *in_info, void **in_value, unsigned long long in_pos, int in_operation);