#include <mediaplayer/LLPreLoaderAPI.h>

// This code is copied from horionclient's Horion project,
// licensed under Attribution-NonCommercial 4.0 International.
// Modifications were made by WillowSauceR to adapt the code for use in MediaPlayer,
// which is licensed under GPL-3.0.
// Modified by WillowSauceR in order to support use in C code.

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
