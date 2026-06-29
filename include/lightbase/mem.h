#pragma once
#include <stdint.h>

#ifdef __linux__
    #include <dlfcn.h>
    #ifndef RTLD_DEFAULT
        #define RTLD_DEFAULT ((void *)0)
    #endif
    #define SYM(sym) dlsym(RTLD_DEFAULT, sym)
#else
    #include <lightbase/plugin.h>
    #define SYM(sym) lb_sym_find(sym)
#endif

#define SYMCALL(sym, proto, ...)   ((proto)(SYM(sym)))(__VA_ARGS__)
#define PTRCALL(ptr, proto, ...)   ((proto)(void *)(ptr))(__VA_ARGS__)

#define DEREFERENCE(type, ptr, off)  (*(type *)((uintptr_t)(ptr) + (off)))
#define REFERENCE(type, ptr, off)    ((type *)((uintptr_t)(ptr) + (off)))
#define PTR_OFFSET(ptr, off)         ((uintptr_t)(ptr) + (off))
