#include <mediaplayer/cpp_string.h>

// std::string::string(const char *c_str)
struct string *cpp_string__string(const char *c_str);
// std::string::c_str()
const char *cpp_string__c_str(struct string *cpp_str);

struct string string = {
    .string = cpp_string__string,
    .c_str = cpp_string__c_str,
};


struct string *cpp_string__string(const char *c_str)
{
    size_t extra_size = 32;
    struct string *cpp_str = calloc(strnlen(c_str, 9223372036854775807ULL) + extra_size, 1);
    TLCALL("??0?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEAA@QEBD@Z",
        const char *(*)(struct string *cpp_str, const char *c_str),
        cpp_str, c_str);
    return cpp_str;
}

const char *cpp_string__c_str(struct string *cpp_str)
{
    const char *c_str = 
        TLCALL("?c_str@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEBAPEBDXZ",
            const char *(*)(struct string *cpp_str),
            cpp_str);
    return c_str;
}
