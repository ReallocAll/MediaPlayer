#include <mediaplayer/process_png.h>

// Decode PNG header and optionally pixels.
// When get_ihdr is true or image is nullptr, only the IHDR is read.
// When get_ihdr is false and image is non-nullptr, pixels are decoded
// into the pre-allocated image buffer (RGBA8, size = width * height * 4).
bool get_pixels(FILE *png, struct spng_ihdr *ihdr,
                unsigned char *image, bool get_ihdr)
{
    int ret = 0;
    spng_ctx *ctx = nullptr;
    size_t image_size = 0;

    if (png == nullptr)
        goto error;

    ctx = spng_ctx_new(0);
    if (ctx == nullptr)
        goto error;

    spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);
    spng_set_png_file(ctx, png);

    ret = spng_get_ihdr(ctx, ihdr);
    if (ret)
        goto error;

    if (get_ihdr || !image) {
        spng_ctx_free(ctx);
        return true;
    }

    ret = spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &image_size);
    if (ret)
        goto error;

    ret = spng_decode_image(ctx, image, image_size, SPNG_FMT_RGBA8, 0);
    if (ret)
        goto error;

    spng_ctx_free(ctx);
    return true;

error:
    spng_ctx_free(ctx);
    return false;
}

void set_pixels(unsigned char *image, struct map_item_saved_data *map_data,
                struct start_pixel *start_pixel, struct spng_ihdr *ihdr)
{
    int (*inner_pixels)[128][128];

    if (!image || !map_data)
        return;

    inner_pixels = *((void **)map_data + 6);
    for(unsigned y = 0; y < 128; y++) {
        unsigned char *base_pixel = 
            image + 
                ((y + start_pixel->y)       // target line index (y ~ y + 128)
                * ihdr->width               // locate to first pixel of target line
                + start_pixel->x)           // locate to pixel of target row
                * 4;                        // single pixel size is 4 byte == 4 * sizeof(char)
        // copy 128 pixels of single line to map
        memcpy(&(*inner_pixels)[y], base_pixel, 128 * 4);  
    }
    set_pixel_dirty(map_data);
}

void set_pixel_dirty(struct map_item_saved_data *this)
{
    *(short *)((char *)this + 121) = 257;
    int64_t item = *(int64_t *)*((uintptr_t *)this + 12);
    // set the pixel as dirty and set the row and line boundaries
    *(char *)(item + 32) = 1;
    *(int *)(item + 36) = 0;
    *(int *)(item + 40) = 0;
    *(int *)(item + 44) = 127;
    *(int *)(item + 48) = 127;
}
