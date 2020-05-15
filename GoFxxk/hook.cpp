#include "includes.h"

// hooking functions by rake and solair

// patch bytes function
void Patch(BYTE* dst, BYTE* src, unsigned int size) {
	DWORD oProc;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oProc);
	memcpy(dst, src, size);
	VirtualProtect(dst, size, oProc, &oProc);
}

// hook function
bool Hook(char* src, char* dst, int len) {
	if (len < 5) return false;
	DWORD oProc;
	VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &oProc);
	memset(src, 0x90, len);
	uintptr_t relAddy = (uintptr_t)(dst - src - 5);
	_asm
	{
		jz _P2
		jnz _P2
		_P1 :
		__emit 0xE8
	}
_P2:
	*src = (char)0xE9;
	*(uintptr_t*)(src + 1) = (uintptr_t)relAddy;
	VirtualProtect(src, len, oProc, &oProc);
	return true;
}

// trampoline hook function
char* TrampHook(char* src, char* dst, unsigned int len) {
	if (len < 5) return 0;
	char* gateway = (char*)VirtualAlloc(0, len + 5, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	memcpy(gateway, src, len);
	_asm
	{
		jz _P2
		jnz _P2
		_P1 :
		__emit 0xE8
	}
_P2:
	uintptr_t jumpAddy = (uintptr_t)(src - gateway - 5);
	*(gateway + len) = (char)0xE9;
	*(uintptr_t*)(gateway + len + 1) = jumpAddy;
	if (Hook(src, dst, len)) {
		return gateway;
	}
	else return nullptr;
}