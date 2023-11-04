#include <mediaplayer/cpp_string.h>

struct string *std_string_string(const char *c_str)
{
	size_t extra_size = 32;
	struct string *cpp_str = calloc(strnlen(c_str, 9223372036854775807ULL) + extra_size, 1);
	SYMCALL("??0?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEAA@QEBD@Z",
		const char *(*)(struct string *cpp_str, const char *c_str),
		cpp_str, c_str);
	return cpp_str;
}

const char *std_string_c_str(struct string *std_str)
{
	const char *c_str = 
		SYMCALL("?c_str@?$basic_string@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEBAPEBDXZ",
			const char *(*)(struct string *),
			std_str);
	return c_str;
}
