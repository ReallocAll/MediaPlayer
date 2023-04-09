#pragma once
#include <windows.h>
#include <psapi.h>
#include <littlehooker/littlehooker.h>

uintptr_t FindSignatureModule(const char* szModule, const char* szSignature);

bool hook_func_ll(void *oldFunc, void **pOutOld, void *newFunc);
bool hook_func_auto(void *target_func, void *detour_func, void *original_func);

void *dlsym_ll(const char *x);
void *dlsym_auto(const char *sym);

extern bool using_ll_preloader_api;
