#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <libcutils/libcutils.h>
#include <libspng/spng.h>
#include "mc/structs.h"

#ifndef __linux__
typedef int64_t __int64_t;
typedef void* __int128_t;
#endif

struct rgb {
    unsigned char r, g, b;
};

struct start_pixel {
    uint32_t x, y;
};

// get the pixels from a png file
// please use free() to free the returned pointer after use
void get_pixels(FILE *png, struct spng_ihdr *ihdr,
                unsigned char *image, bool get_ihdr);

// copy 128 pixels from image to map line by line
void set_pixels(unsigned char *image, struct map_item_saved_data *map_data,
                struct start_pixel *start_pixel, struct spng_ihdr *ihdr);

void set_pixel_dirty(struct map_item_saved_data* this);
