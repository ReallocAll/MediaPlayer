#include <stdio.h>

#include "../../include/mediaplayer/config_handler.h"

int main(int argc, char **argv)
{
    printf("argc: %d\n", argc);

    if (argc <= 1) {
        printf("no file path arg\n");
        return -1;
    }

    int func_ret = 0;

    struct config_file conf;

    func_ret = config_open_file(&conf, argv[1]);
    if (func_ret) {
        printf("config open failed: %d\n", func_ret);
        return func_ret;
    }

    char *val = 0;
    func_ret = config_read(&conf, "test", &val);
    if (func_ret) {
        printf("val not found: %d\n", func_ret);
        return func_ret;
    }

    printf("var: %s\n", val);

    func_ret = config_read(&conf, "aaa1", &val);
    if (func_ret) {
        printf("val not found: %d\n", func_ret);
        return func_ret;
    }

    
    printf("var: %s\n", val);

    return 0;
}