#pragma once
#include <windows.h>
inline bool IsDbgPresentPrefixCheck()
{
	__try
	{
		_asm _emit 0xF3 // 0xF3 0x64 disassembles as PREFIX REP:
		_asm _emit 0x64
		_asm _emit 0xF1 // One byte INT 1
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	return true;
}