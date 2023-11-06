#include <mediaplayer/process_png.h>

// get the pixels from a png file
// please use free() to free the returned pointer after use
void get_pixels(FILE *png, struct spng_ihdr *ihdr,
                unsigned char *image, bool get_ihdr)
{
    int ret = 0;
    spng_ctx *ctx = NULL;
    size_t image_size = 0;

    if(png == NULL)
        goto error;

    ctx = spng_ctx_new(0);
    if(ctx == NULL)
        goto error;

    spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);
    spng_set_png_file(ctx, png);

    ret = spng_get_ihdr(ctx, ihdr);
    if(ret || get_ihdr || !image)
        goto error;

    ret = spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &image_size);
    if(ret)
        goto error;

    ret = spng_decode_image(ctx, image, image_size, SPNG_FMT_RGBA8, 0);
    if(ret)
        goto error;

error:
    spng_ctx_free(ctx);
}

void set_pixels(unsigned char *image, struct map_item_saved_data *map_data,
                struct start_pixel *start_pixel, struct spng_ihdr *ihdr)
{
    int (*inner_pixels)[128][128];
    inner_pixels = *((void **)map_data + 6);
    if (!image)
        return;
    for(unsigned y = 0; y < 128; y++) {
        unsigned char *base_pixel = 
            image + ((y + start_pixel->y) * ihdr->width + start_pixel->x) * 4;
        memcpy(&(*inner_pixels)[y], base_pixel, 128 * 4);
    }
    set_pixel_dirty(map_data);
}

void set_pixel_dirty(struct map_item_saved_data *this)
{
    *(short *)((char *)this + 121) = 257;
    __int64_t item = *(__int64_t *)*((uintptr_t *)this + 12);
    // set the pixel as dirty and set the row and line boundaries
    *(char *)(item + 32) = 1;
    *(int *)(item + 36) = 0;
    *(int *)(item + 40) = 0;
    *(int *)(item + 44) = 127;
    *(int *)(item + 48) = 127;
}
