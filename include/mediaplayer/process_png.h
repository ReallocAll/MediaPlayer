#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <windows.h>
#include <libspng/spng.h>

struct rgb {
    unsigned char r, g, b;
};

struct start_pixel {
    int x, y;
};

void get_png_pixels(const char *filename, struct spng_ihdr *ihdr, bool get_ihdr,
                            struct start_pixel *start_pixel, int (*inner_pixels)[128][128]);
