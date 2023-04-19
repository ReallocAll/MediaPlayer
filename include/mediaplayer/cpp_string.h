#pragma once
#include <littlehooker/littlehooker.h>

struct string;


// std::string::string(const char *c_str)
struct string *std_string_string(const char *c_str);
// std::string::c_str()
const char *std_string_c_str(struct string *std_str);

