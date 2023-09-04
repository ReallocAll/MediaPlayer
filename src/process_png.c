#include <mediaplayer/process_png.h>

void get_png_pixels(const char *filename, struct spng_ihdr *ihdr, bool get_ihdr,
                            struct start_pixel *start_pixel, int (*inner_pixels)[128][128])
{
    FILE *png;
    int ret = 0;
    spng_ctx *ctx = NULL;
    unsigned char *image = NULL;
    size_t image_size;

    png = fopen(filename, "rb");
    if(png == NULL)
        goto error;

    ctx = spng_ctx_new(0);
    if(ctx == NULL)
        goto error;

    spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);
    spng_set_png_file(ctx, png);

    ret = spng_get_ihdr(ctx, ihdr);
    if(ret || get_ihdr)
        goto error;

    ret = spng_decoded_image_size(ctx, SPNG_FMT_RGBA8, &image_size);
    if(ret)
        goto error;

    image = malloc(image_size);
    if(image == NULL)
        goto error;

    ret = spng_decode_image(ctx, image, image_size, SPNG_FMT_RGBA8, 0);

    if(ret)
        goto error;
    
    for(unsigned x = 0; x < 128; x++) {
        for(unsigned y = 0; y < 128; y++) {
            // get pixel from image
            unsigned char *pixel = image + ((x + start_pixel->y) * ihdr->width + (y + start_pixel->x)) * 4;
            // set pixel to map pixel array
            (*inner_pixels)[x][y] = 0xFF000000 | *(unsigned int *)pixel;
        }
    }

error:
    fclose(png);
    spng_ctx_free(ctx);
    free(image);
}
