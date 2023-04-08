#include <mediaplayer/LLPreLoaderAPI.h>

// This code is copied from horionclient's Horion project,
// licensed under Attribution-NonCommercial 4.0 International.
// Modifications were made by WillowSauceR to adapt the code for use in MediaPlayer,
// which is licensed under GPL-3.0.
// Modified by WillowSauceR in order to support use in C code.

// START
#define INRANGE(x, a, b) (x >= a && x <= b)
#define GET_BYTE(x) (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))
#define GET_BITS(x) (INRANGE((x & (~0x20)), 'A', 'F') ? ((x & (~0x20)) - 'A' + 0xa) : (INRANGE(x, '0', '9') ? x - '0' : 0))

uintptr_t FindSignatureModule(const char* szModule, const char* szSignature) {
	const char* pattern = szSignature;
	uintptr_t firstMatch = 0;
	const uintptr_t rangeStart = (uintptr_t)GetModuleHandleA(szModule);
	MODULEINFO miModInfo;
	bool init = false;
	if (!init) {
		init = true;
		GetModuleInformation(GetCurrentProcess(), (HMODULE)rangeStart, &miModInfo, sizeof(MODULEINFO));
	}
	const uintptr_t rangeEnd = rangeStart + miModInfo.SizeOfImage;

	BYTE patByte = GET_BYTE(pattern);
	const char* oldPat = pattern;

	for (uintptr_t pCur = rangeStart; pCur < rangeEnd; pCur++) {
		if (!*pattern)
			return firstMatch;

		while (*(PBYTE)pattern == ' ')
			pattern++;

		if (!*pattern)
			return firstMatch;

		if (oldPat != pattern) {
			oldPat = pattern;
			if (*(PBYTE)pattern != '\?')
				patByte = GET_BYTE(pattern);
		}

		if (*(PBYTE)pattern == '\?' || *(BYTE*)pCur == patByte) {
			if (!firstMatch)
				firstMatch = pCur;

			if (!pattern[2] || !pattern[1])
				return firstMatch;

			//if (*(PWORD)pattern == '\?\?' || *(PBYTE)pattern != '\?')
			//pattern += 3;

			//else
			pattern += 2;
		} else {
			pattern = szSignature;
			firstMatch = 0;
		}
	}
	return 0u;
}
// END

extern bool using_ll_preloader_api;

bool hook_func_ll(void *oldFunc, void **pOutOld, void *newFunc)
{
	static uintptr_t func = 0;
	if (!func)
		func = FindSignatureModule("LLPreloader.dll",
			"40 53 56 57 48 83 EC 40  48 8B 05 ?? ?? 03 00 48");
	VIRTUAL_CALL(func,
		int (*)(void *oldFunc, void **pOutOld, void *newFunc),
		oldFunc, pOutOld, newFunc);

	return true;
}

bool hook_func_auto(void *target_func, void *detour_func, void *original_func)
{
	if (using_ll_preloader_api)
		return hook_func_ll(target_func, original_func, detour_func);
	else
		return hook_func(target_func, detour_func, original_func);
}

void *dlsym_ll(const char *x)
{
	static uintptr_t func = 0;
	if (!func)
		func = FindSignatureModule("LLPreloader.dll",
			"40 55 53 56 48 8D 6C 24  B9 48 81 EC 90 00 00 00");
	
	return 
		VIRTUAL_CALL(func,
			void *(*)(const char *x),
			x);
}

void *dlsym_auto(const char *sym)
{
	if (using_ll_preloader_api)
		return dlsym_ll(sym);
	else
		return dlsym(sym);
}
