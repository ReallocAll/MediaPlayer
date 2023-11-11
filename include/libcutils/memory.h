#ifndef LIBCUTILS_MEMORY_H
#define LIBCUTILS_MEMORY_H

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

#endif
