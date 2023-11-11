#ifndef LIBCUTILS_STRING_H
#define LIBCUTILS_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * std::string::basic_string(const char *c_str)
 * 
 * void *sstr = NULL;
 * std_string_string(&sstr, "example");
 */
void std_string_string(void **sstr, const char *c_str);

/*
 * std::string::c_str()
 *
 * char *saved_str = strdup(std_string_c_str(sstr));
*/
const char *std_string_c_str(void *sstr);

// std::string::~basic_string()
void std_string_destroy(void *sstr, bool should_free);

#ifdef __cplusplus
}
#endif

#endif
