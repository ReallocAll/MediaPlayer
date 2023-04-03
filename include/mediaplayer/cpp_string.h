#pragma once
#include <littlehooker/littlehooker.h>

struct string {
    struct string *(*string)(const char *c_str);
    const char *(*c_str)(struct string *cpp_str);
};
