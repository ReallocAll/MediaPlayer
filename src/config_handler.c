#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <mediaplayer/config_handler.h>

int config_open_file(struct config_file *in_config_file, char *in_filename)
{
    int func_ret = 0;

    FILE *config_file = fopen(in_filename, "rb");
    if (!config_file) {
        return -1;
    }

    func_ret = fseek(config_file, 0, SEEK_END);
    if (func_ret) {
        return -2;
    }

    in_config_file->size = ftell(config_file);
    if (in_config_file->size == -1) {
        return -3;
    }

    if (in_config_file->size > CONFIG_MAX_SIZE) {
        return -4;
    }

    in_config_file->data = (char *)malloc((size_t)in_config_file->size);
    if (!*in_config_file->data) {
        return -5;
    }

    fseek(config_file, 0, SEEK_SET);
    func_ret = fread(in_config_file->data, in_config_file->size, 1, config_file);
    if (!func_ret) {
        return -6;
    }

    fclose(config_file);

    return 0;
}

int config_read(struct config_file *in_config_file, char *in_valname, char **out_val)
{
    size_t val_len = strlen(in_valname);
    for (int i = 0; i < in_config_file->size; i++) { // search valname in buf
        if (in_config_file->data[i] == '\n' || in_config_file->data[i] == '\r' || in_config_file->data[i] == ' ') {
            continue;
        }

        for (size_t j = 0, jf = i; j < val_len; j++, jf++) { // str match
            if (in_valname[j] != in_config_file->data[jf]) { // skip if ne
                break;
            }

            if (j == val_len - 1) { // if valname found
                char eq_found = 0;
                char var_found = 0;
                size_t var_start_pos = 0;
                size_t var_size = 0;

                for (int l = jf + 1; l < in_config_file->size; l++) {
                    if (in_config_file->data[l] == ' ') { // skip blank
                        continue;
                    }

                    if (in_config_file->data[l] == '=') { // find eq
                        eq_found = 1;
                        continue;
                    }

                    if (in_config_file->data[l] == '\n' || in_config_file->data[l] == '\r') { // when eol
                        return -2;
                    }

                    if (eq_found) { // eq found
                        for (size_t a = l; a < in_config_file->size; a++) {
                            if (in_config_file->data[a] == ' ') {
                                continue;
                            }

                            if (in_config_file->data[a] == '\n' || in_config_file->data[a] == '\r') { // when eol
                                break;
                            }

                            var_found = 1; // var found
                            var_start_pos = a;
                            while(in_config_file->data[a] != '\n' && in_config_file->data[a] != '\r' && a < in_config_file->size) {
                                if (in_config_file->data[a] == ' ') {
                                    return -3;
                                }
                                var_size++;
                                a++;
                            }
                            break;
                        }

                        if (!var_found) {
                            return -4;
                        }

                        *out_val = (char *)malloc(var_size + 1);
                        memcpy(*out_val, &in_config_file->data[var_start_pos], var_size);
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