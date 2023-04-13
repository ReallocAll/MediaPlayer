#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <libspng/spng.h>

struct rgb {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct rgb **get_png_pixels(const char *filename);
