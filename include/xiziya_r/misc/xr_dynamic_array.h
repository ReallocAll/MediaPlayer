#pragma once

#include <malloc.h>

#define XR_ARRAY_ADD 0
#define XR_ARRAY_DEL 1

typedef struct {
    unsigned long long curr_arr_size;
    size_t singel_value_size;
    void* start_addr;
} xr_dynamic_array_info;

void *xr_new_dynamic_array(size_t in_size, xr_dynamic_array_info *out_info);
char xr_operator_dynamic_array(xr_dynamic_array_info *in_info, void **in_value, unsigned long long in_pos, char in_operation);
