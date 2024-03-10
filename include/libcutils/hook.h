#ifndef LIBCUTILS_HOOK_H
#define LIBCUTILS_HOOK_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "memory.h"

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

enum loader_type {
    LOADER_TYPE_LIGHTBASE,
    LOADER_TYPE_MODLOADER,
    LOADER_TYPE_PRELOADER
};

extern FARPROC hook_func_address;
extern FARPROC unhook_func_address;
extern FARPROC dlsym_func_address;
extern int loader_type;

inline int hook_func(void *target, void *detour, void **original)
{
    if (loader_type == LOADER_TYPE_LIGHTBASE) {
        return ((int (*)(void *target, void *detour, void **original)) ((void *)hook_func_address)) (target, detour, original);
    } else if (loader_type == LOADER_TYPE_PRELOADER) {
        return ((int (*)(void *target, void *detour, void **original, int priority)) ((void *)hook_func_address)) (target, detour, original, 200);
    }
    return 0;
}

inline bool unhook_func(void *target, void *detour)
{
    if (loader_type == LOADER_TYPE_LIGHTBASE) {
	    return ((bool (*)(void *target)) ((void *)unhook_func_address)) (target);
    } else if (loader_type == LOADER_TYPE_PRELOADER) {
	    return ((bool (*)(void *target, void *detour)) ((void *)unhook_func_address)) (target, detour);
    }
    return false;
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
                         name.detour,                        \
                         (void **)&name.original);           \
    }                                                        \
                                                             \
    bool _destroy_##name(void)                               \
    {                                                        \
        return unhook_func(name.hook, name.detour);          \
    }                                                        \
                                                             \
    ret_type _detour_##name(__VA_ARGS__)


#endif

#endif
