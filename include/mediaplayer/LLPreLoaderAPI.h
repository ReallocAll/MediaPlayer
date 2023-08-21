#pragma once
#include <windows.h>
#include <psapi.h>
#include <stdbool.h>

bool hook_func(void *hook_func, void *detour_func, void *original_func);
void *dlsym(const char *sym);

extern bool using_ll_preloader_api;

inline bool hook_func_ll(void *oldFunc, void **pOutOld, void *newFunc)
{
	static FARPROC func = 0;
	if (!func)
		func = GetProcAddress(GetModuleHandleA("LLPreloader"), "HookFunction");
	((int (*)(void *oldFunc, void **pOutOld, void *newFunc)) ((void *)func)) (oldFunc, pOutOld, newFunc);

	return true;
}

inline bool hook_func_auto(void *target_func, void *detour_func, void *original_func)
{
	if (using_ll_preloader_api)
		return hook_func_ll(target_func, original_func, detour_func);
	else
		return hook_func(target_func, detour_func, original_func);
}

inline void *dlsym_ll(const char *x)
{
	static FARPROC func = 0;
	if (!func)
		func = GetProcAddress(GetModuleHandleA("LLPreloader"), "dlsym_real");
	
	return ((void *(*)(const char *x)) ((void *)func)) (x);
}

inline void *dlsym_auto(const char *sym)
{
	if (using_ll_preloader_api)
		return dlsym_ll(sym);
	else
		return dlsym(sym);
}
