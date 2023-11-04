#pragma once
#include "minhook/MinHook.h"

#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include <mediaplayer/LLPreLoaderAPI.h>

#define SYM_FILE "bedrock_server_sym.txt"
#define SYM_CACHE_FILE "bedrock_server_sym_cache.bin"

#define CVDUMP_URL "https://raw.github.com/microsoft/microsoft-pdb/master/cvdump/cvdump.exe"
#define CVDUMP_EXE_PATH "cvdump.exe"
#define CVDUMP_EXEC_ARGS " -headers -p "
#define CVDUMP_MISSING_MSG \
            CVDUMP_EXE_PATH " not found, please download it from " CVDUMP_URL \
            " and then put it in the same level folder as bedrock_server.pdb\n"

#define BDS_FILE_NAME "bedrock_server"
#define BDS_EXE_PATH BDS_FILE_NAME ".exe"
#define BDS_MOD_EXE_PATH BDS_FILE_NAME "_mod" ".exe"
#define BDS_PDB_PATH BDS_FILE_NAME ".pdb"

#define THOOK(name, ret_type, sym, ...)          		    \
    typedef ret_type (*_##name##_t)(__VA_ARGS__);           \
    ret_type _detour_##name(__VA_ARGS__);                   \
    bool _install_##name(void);                             \
    bool _destroy_##name(void);                             \
                                                            \
    struct _##name {                                        \
        _##name##_t hook;                                   \
        _##name##_t detour;                                 \
        _##name##_t original;                               \
        bool (*install)(void);                              \
        bool (*destroy)(void);                              \
    } name = {NULL, _detour_##name, NULL,                   \
                _install_##name, _destroy_##name};          \
                                                            \
    bool _install_##name(void)                              \
    {                                                       \
        name.hook = (_##name##_t)dlsym_auto(sym);           \
        bool result = hook_func_auto(name.hook,             \
                                     name.detour,           \
                                     &name.original);       \
        return result;                                      \
    }                                                       \
                                                            \
    bool _destroy_##name(void)                              \
    {                                                       \
        if (!using_ll_preloader_api)                        \
            return                                          \
                MH_RemoveHook(name.hook) == MH_OK           \
                    ? true : false;                         \
        return true;                                        \
    }                                                       \
                                                            \
    ret_type _detour_##name(__VA_ARGS__)


#define SYMCALL(sym, func_proto, ...)                       \
    ((func_proto)                                           \
    (dlsym_auto(sym)))                                      \
    (__VA_ARGS__)


#define VIRTUAL_CALL(ptr, func_proto, ...)                  \
    ((func_proto)                                           \
    ((void *)ptr))                                          \
    (__VA_ARGS__)


#define DEREFERENCE(type, ptr, offset)                      \
    (*(type*)((uintptr_t)ptr + offset))


#define REFERENCE(type, ptr, offset)                        \
    (type*)((uintptr_t)ptr + offset)


// for uintptr_t
#define PTR_OFFSET(ptr, offset)                             \
    ((uintptr_t)ptr + offset)


#ifdef __cplusplus
extern "C" {
#endif

bool hook_func(void *hook_func, void *detour_func, void *original_func);
inline void *rva2va(unsigned int rva);
void read_static_data(long offset, void *data, size_t size);
void *dlsym(const char *sym);
bool release_cvdump_exe(void);
inline int gen_sym_file(void);
inline int re_gen_sym_files(void);

bool init_section_infos(void);

int get_network_protocol_version(void);

void check_server_update(void);

bool lh_init(void);
bool lh_uninit(void);
bool lh_enable_all_hook(void);
bool lh_disable_all_hook(void);

#ifdef __cplusplus
}
#endif
