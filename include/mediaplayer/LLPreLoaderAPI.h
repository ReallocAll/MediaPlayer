#pragma once
#include <windows.h>
#include <psapi.h>
#include <stdbool.h>

bool hook_func(void *hook_func, void *detour_func, void *original_func);
void *dlsym(const char *sym);

uintptr_t FindSignatureModule(const char* szModule, const char* szSignature);

extern bool using_ll_preloader_api;

inline bool hook_func_ll(void *oldFunc, void **pOutOld, void *newFunc)
{
	static FARPROC func = 0;
	if (!func)
		func = GetProcAddress(GetModuleHandleA("LLPreloader"), "HookFunction");
		//func = FindSignatureModule("LLPreloader.dll", "40 53 56 57 48 83 EC 40  48 8B 05 ?? ?? 03 00 48");
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
		//func = FindSignatureModule("LLPreloader.dll", "40 55 53 56 48 8D 6C 24  B9 48 81 EC 90 00 00 00");
	
	return ((void *(*)(const char *x)) ((void *)func)) (x);
}

inline void *dlsym_auto(const char *sym)
{
	if (using_ll_preloader_api)
		return dlsym_ll(sym);
	else
		return dlsym(sym);
}
