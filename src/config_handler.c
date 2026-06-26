#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <mediaplayer/config_handler.h>

unsigned char g_conf_video_convert_mode = VIDEO_CONVERT_MODE_AT_PLAY;
unsigned char g_conf_video_output_fmt = VIDEO_IMG_OUTPUT_FMT_RAW;
unsigned char g_conf_video_perf_level = VIDEO_PROCESS_PERF_LOWER;
unsigned char g_conf_video_encoder = VIDEO_ENCODER_X264;
unsigned char g_conf_video_decoder = VIDEO_DECODER_H264;

struct config_file g_conf_data;


int config_open_file(struct config_file *in_config_file, char *in_filename)
{
    int func_ret;
    FILE *config_file;

    config_file = fopen(in_filename, "rb");
    if (!config_file)
        return -1;

    func_ret = fseek(config_file, 0, SEEK_END);
    if (func_ret) {
        fclose(config_file);
        return -2;
    }

    in_config_file->size = ftell(config_file);
    if (in_config_file->size == -1) {
        fclose(config_file);
        return -3;
    }

    if (in_config_file->size > CONFIG_MAX_SIZE) {
        fclose(config_file);
        return -4;
    }

    in_config_file->data = malloc((size_t)in_config_file->size);
    if (!in_config_file->data) {
        fclose(config_file);
        return -5;
    }

    fseek(config_file, 0, SEEK_SET);
    func_ret = (int)fread(in_config_file->data, in_config_file->size, 1, config_file);
    fclose(config_file);
    if (!func_ret)
        return -6;

    return 0;
}

int config_read(struct config_file *in_config_file, char *in_valname, char **out_val)
{
    size_t val_len = strlen(in_valname);
    size_t size = (size_t)in_config_file->size;
    size_t i;

    for (i = 0; i < size; i++) {
        if (in_config_file->data[i] == '\n' ||
            in_config_file->data[i] == '\r' ||
            in_config_file->data[i] == ' ')
            continue;

        for (size_t j = 0, jf = i; j < val_len; j++, jf++) {
            if (in_valname[j] != in_config_file->data[jf])
                break;

            if (j == val_len - 1) {
                char eq_found = 0;
                char var_found = 0;
                size_t var_start_pos = 0;
                size_t var_size = 0;
                size_t l;

                for (l = jf + 1; l < size; l++) {
                    if (in_config_file->data[l] == ' ')
                        continue;

                    if (in_config_file->data[l] == '=') {
                        eq_found = 1;
                        continue;
                    }

                    if (in_config_file->data[l] == '\n' ||
                        in_config_file->data[l] == '\r')
                        return -2;

                    if (eq_found) {
                        size_t a;

                        for (a = l; a < size; a++) {
                            if (in_config_file->data[a] == ' ')
                                continue;

                            if (in_config_file->data[a] == '\n' ||
                                in_config_file->data[a] == '\r')
                                break;

                            var_found = 1;
                            var_start_pos = a;
                            while (a < size &&
                                   in_config_file->data[a] != '\n' &&
                                   in_config_file->data[a] != '\r') {
                                if (in_config_file->data[a] == ' ')
                                    return -3;
                                var_size++;
                                a++;
                            }
                            break;
                        }

                        if (!var_found)
                            return -4;

                        *out_val = malloc(var_size + 1);
                        if (!*out_val)
                            return -5;
                        memcpy(*out_val, &in_config_file->data[var_start_pos],
                               var_size);
                        (*out_val)[var_size] = 0;

                        return 0;
                    }
                }
            }
        }
    }

    return -1;
}

int config_load()
{
    return 0;
}

int config_reload()
{
    return 0;
}