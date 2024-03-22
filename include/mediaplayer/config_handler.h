#pragma once

#define CONFIG_MAX_SIZE 300

struct config_file {
    long size;
    char *data;
};

int config_open_file(struct config_file *in_config_file, char *in_filename);
int config_read(struct config_file *in_config_file, char *in_valname, char **out_val);