#include <xiziya_r/misc/xr_dynamic_array.h>

void *xr_new_dynamic_array(size_t in_size, xr_dynamic_array_info *out_info)
{
    out_info->cur_arr_size = 0;
    out_info->singel_value_size = in_size;
    out_info->start_addr = malloc(in_size);
    return out_info->start_addr;
}

char xr_operator_dynamic_array(xr_dynamic_array_info *in_info, void **in_value, unsigned long long in_pos, char in_operation)
{
    switch (in_operation) {
    case XR_ARRAY_ADD:
        if (in_pos <= in_info->cur_arr_size) {
            in_info->start_addr = realloc(in_info->start_addr, (in_info->cur_arr_size + 1) * in_info->singel_value_size);
            if (!in_info->start_addr) return -1;
            if (in_pos < in_info->cur_arr_size++) {
                char *srcMem, *dstMem;
                for (unsigned long long posCount = in_info->cur_arr_size; posCount > in_pos; posCount--) {
                    srcMem = (char *)in_info->start_addr + (posCount - 1) * in_info->singel_value_size;
                    dstMem = (char *)in_info->start_addr + posCount * in_info->singel_value_size;
                    for (size_t memPos = 0; memPos < in_info->singel_value_size; memPos++) {
                        *dstMem++ = *srcMem++;
                    }
                }
            }
        } else {
            return -1;
        }
        *in_value = in_info->start_addr;
        return 0;
    break;
    case XR_ARRAY_DEL:
        if(in_pos < in_info->cur_arr_size) {
            if (in_pos < in_info->cur_arr_size - 1) {
                char *srcMem, *dstMem;
                for (unsigned long long posCount = in_pos; posCount < in_info->cur_arr_size; posCount++) {
                    srcMem = (char *)in_info->start_addr + (posCount + 1) * in_info->singel_value_size;
                    dstMem = (char *)in_info->start_addr + posCount * in_info->singel_value_size;
                    for (size_t memPos = 0; memPos < in_info->singel_value_size; memPos++) {
                        *dstMem++ = *srcMem++;
                    }
                }
            }
            if(in_info->cur_arr_size-- > 1) in_info->start_addr = realloc(in_info->start_addr, in_info->cur_arr_size * in_info->singel_value_size);
            if(!in_info->start_addr) return -1;
        } else {
            return -1;
        }
        *in_value = in_info->start_addr;
        return 0;
    break;
    default:
        return 0;
    break;
    }
}
