#ifndef LIBCUTILS_H
#define LIBCUTILS_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#ifdef PLUGIN_ENVIRONMENT
    #ifdef __linux__
        #include <modloader/hook.h>
        #ifndef RTLD_DEFAULT
            #define RTLD_DEFAULT NULL
        #endif
        #define SYM(sym) dlsym(RTLD_DEFAULT, sym)
    #else
        #include <windows.h>
        #include <psapi.h>
        #define SYM(sym) dlsym(sym)
    #endif
#endif




#define SYMCALL(sym, func_proto, ...)                        \
    ((func_proto)                                            \
    (SYM(sym)))                                              \
    (__VA_ARGS__)


#define VIRTUAL_CALL(ptr, func_proto, ...)                   \
    ((func_proto)                                            \
    ((void *)ptr))                                           \
    (__VA_ARGS__)


#define DEREFERENCE(type, ptr, offset)                       \
    (*(type*)((uintptr_t)ptr + offset))


#define REFERENCE(type, ptr, offset)                         \
    (type*)((uintptr_t)ptr + offset)


// for uintptr_t
#define PTR_OFFSET(ptr, offset)                              \
    ((uintptr_t)ptr + offset)

#ifdef __linux__
#define THOOK(name, ret_type, sym, ...)          		     \
    typedef ret_type (*_##name##_t)(__VA_ARGS__);            \
    ret_type _detour_##name(__VA_ARGS__);                    \
    void _install_##name(void) __attribute__((constructor)); \
    void _destroy_##name(void);                              \
                                                             \
    struct _##name {                                         \
        modloader_hook_t *hook;                              \
        _##name##_t detour;                                  \
        _##name##_t original;                                \
        void (*install)(void);                               \
        void (*destroy)(void);                               \
    } name = {NULL, _detour_##name, NULL,                    \
                _install_##name, _destroy_##name};           \
                                                             \
    void _install_##name(void)                               \
    {                                                        \
        name.hook = modloader_hook(SYM(sym),                 \
                                name.detour,                 \
                                (void**)&name.original);     \
    }                                                        \
                                                             \
    void _destroy_##name(void)                               \
    {                                                        \
        modloader_destroy_hook(name.hook);                   \
    }                                                        \
                                                             \
    ret_type _detour_##name(__VA_ARGS__)
#else

extern FARPROC hook_func_address;
extern FARPROC dlsym_func_address;

inline int hook_func(void *oldFunc, void **pOutOld, void *newFunc)
{
	return ((int (*)(void *oldFunc, void **pOutOld, void *newFunc)) ((void *)hook_func_address)) (oldFunc, pOutOld, newFunc);
}

inline void *dlsym(const char *x)
{
	return ((void *(*)(const char *x)) ((void *)dlsym_func_address)) (x);
}

#define THOOK(name, ret_type, sym, ...)          		     \
    typedef ret_type (*_##name##_t)(__VA_ARGS__);            \
    ret_type _detour_##name(__VA_ARGS__);                    \
    int _install_##name(void);                               \
    bool _destroy_##name(void);                              \
                                                             \
    struct _##name {                                         \
        _##name##_t hook;                                    \
        _##name##_t detour;                                  \
        _##name##_t original;                                \
        int (*install)(void);                                \
        bool (*destroy)(void);                               \
    } name = {NULL, _detour_##name, NULL,                    \
                _install_##name, _destroy_##name};           \
                                                             \
    int _install_##name(void)                                \
    {                                                        \
        name.hook = (_##name##_t)dlsym(sym);                 \
        return hook_func(name.hook,                          \
                         (void **)&name.original,            \
                         name.detour);                       \
    }                                                        \
                                                             \
    bool _destroy_##name(void)                               \
    {                                                        \
        return false;                                        \
    }                                                        \
                                                             \
    ret_type _detour_##name(__VA_ARGS__)
#endif


#ifdef __cplusplus
extern "C" {
#endif


// void *sstr = std::string::basic_string(const char *c_str)
void std_string_string(void **sstr, const char *c_str);

// std::string::c_str()
const char *std_string_c_str(void *sstr);

// std::string::~basic_string()
void std_string_destroy(void *sstr, bool should_free);

#ifdef __cplusplus
}
#endif

#endif
