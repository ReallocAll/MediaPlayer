#include <dynarray/dynarray.h>
#include <stdlib.h>
#include <string.h>

void *dynarray_new(size_t in_size, dynarray_t *out_info)
{
	out_info->curr_arr_size = 0;
	out_info->single_value_size = in_size;
	out_info->start_addr = malloc(in_size);
	return out_info->start_addr;
}

int dynarray_op(dynarray_t *in_info, void **in_value, unsigned long long in_pos, int in_operation)
{
	switch (in_operation) {
	case DYNARRAY_ADD:
		if (in_pos > in_info->curr_arr_size)
			return -1;
		in_info->start_addr = realloc(in_info->start_addr,
			(in_info->curr_arr_size + 1) * in_info->single_value_size);
		if (!in_info->start_addr)
			return -1;
		if (in_pos < in_info->curr_arr_size) {
			char *dst = (char *)in_info->start_addr +
				(in_pos + 1) * in_info->single_value_size;
			char *src = (char *)in_info->start_addr +
				in_pos * in_info->single_value_size;
			memmove(dst, src,
				(in_info->curr_arr_size - in_pos) * in_info->single_value_size);
		}
		in_info->curr_arr_size++;
		*in_value = in_info->start_addr;
		return 0;

	case DYNARRAY_DEL:
		if (in_pos >= in_info->curr_arr_size)
			return -1;
		if (in_pos < in_info->curr_arr_size - 1) {
			char *dst = (char *)in_info->start_addr +
				in_pos * in_info->single_value_size;
			char *src = (char *)in_info->start_addr +
				(in_pos + 1) * in_info->single_value_size;
			memmove(dst, src,
				(in_info->curr_arr_size - in_pos - 1) * in_info->single_value_size);
		}
		in_info->curr_arr_size--;
		if (in_info->curr_arr_size > 0) {
			in_info->start_addr = realloc(in_info->start_addr,
				in_info->curr_arr_size * in_info->single_value_size);
			if (!in_info->start_addr)
				return -1;
		}
		*in_value = in_info->start_addr;
		return 0;

	default:
		return -1;
	}
}