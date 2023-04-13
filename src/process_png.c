#include <mediaplayer/process_png.h>

struct rgb **get_png_pixels(const char *filename)
{
    FILE *png;
    int ret = 0;
    spng_ctx *ctx = NULL;
    unsigned char *image = NULL;
    size_t image_size;
    struct rgb **pixels = NULL;

    png = fopen(filename, "rb");
    if(png == NULL)
        goto error;

    ctx = spng_ctx_new(0);
    if(ctx == NULL)
        goto error;

    spng_set_crc_action(ctx, SPNG_CRC_USE, SPNG_CRC_USE);
    spng_set_png_file(ctx, png);

    struct spng_ihdr ihdr;
    ret = spng_get_ihdr(ctx, &ihdr);
    if(ret)
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

    pixels = malloc(ihdr.height * sizeof(struct rgb *));
    if (!pixels)
        goto error;

    for(unsigned y = 0; y < ihdr.height; y++) {
        pixels[y] = malloc(ihdr.width * sizeof(struct rgb));
        if (!pixels[y])
            goto error;

        for(unsigned x = 0; x < ihdr.width; x++) {
            unsigned char *pixel = image + (y * ihdr.width + x) * 4;
            pixels[y][x].r = pixel[0];
            pixels[y][x].g = pixel[1];
            pixels[y][x].b = pixel[2];
        }
    }

error:
    fclose(png);
    spng_ctx_free(ctx);
    free(image);

    return pixels;
}
