/*
* This file is part of the Windows Update Agent CPU Support Limitation Removal distribution (https://github.com/p-lider/WuaCpuFix).
* Copyright (c) 2017 p_lider.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"
#include "apihook.h"

APIHook::APIHook(CString functionModule, PROC newFunctionAddress)
{
	this->oldFunctionAddress = NULL;
	this->newFunctionAddress = newFunctionAddress;
	this->functionName = CString("");
	this->functionModule = functionModule;
}

APIHook::APIHook(CString functionName, CString functionModule, PROC newFunctionAddress)
{
	this->oldFunctionAddress = NULL;
	this->newFunctionAddress = newFunctionAddress;
	this->functionName = functionName;
	this->functionModule = functionModule;
}

DWORD_PTR APIHook::FindPattern(DWORD_PTR dwAddress, DWORD dwLen, PBYTE bMask, DWORD maskLen)
{
	DWORD_PTR result = 0;
	DWORD oldProtection = 0;

	for (DWORD_PTR i = 0; i < dwLen - maskLen; i++)
	{
		if (!IsBadReadPtr((PBYTE)(dwAddress + i), maskLen))
		{
			if (memcmp((PBYTE)(dwAddress + i), bMask, maskLen) == 0)
			{
				result = (DWORD_PTR)(dwAddress + i);
				break;
			}
		}
	}
    return result;
}
PROC APIHook::GetCodePatternAddress(PTCHAR moduleName, PBYTE functionPattern, DWORD patternLen)
{
	// This will get the DLL base address (which can vary)
    HMODULE hMod = GetModuleHandle(moduleName);

    // Get module info
    MODULEINFO modinfo = { NULL, };
    GetModuleInformation( GetCurrentProcess(), hMod, &modinfo, sizeof(modinfo) );

	if (hMod != NULL)
	{
		DWORD_PTR dwAddress = 0;

		// This will search the module for the address of a given signature
		dwAddress = FindPattern((DWORD_PTR)hMod, modinfo.SizeOfImage,
			(PBYTE)functionPattern,patternLen);

		return (PROC)(dwAddress);
	}

	return 0;
}

BOOL APIHook::HookByFarJump64(PROC newSLWin8FuncAddr)
{
	BYTE* oldCode = new BYTE[20];
	SIZE_T bw = 0;
	FARJUMP64 Stub64_IsCPUSupported;

	if (sizeof(PVOID) != 8)	//not 64 bit process
	{
		return false;
	}
	DWORD Ver = GetVersion();
	Ver = ((Ver & 0xFF) << 8) | ((Ver & 0xFF00) >> 8);

	PROC oldIsCpuSupportedFuncAddress = NULL;
	for (int i = 0; i < NumberOfPatterns; i++)
	{
		FUNCPATTERN pattern = Patterns64[i];
		if (pattern.osVersion == Ver)
		{
			oldIsCpuSupportedFuncAddress = GetCodePatternAddress(this->functionModule.GetBuffer(), (PBYTE)pattern.pattern, pattern.patternLength);
			if (oldIsCpuSupportedFuncAddress > 0)
			{
				break;
			}
		}
	}

	if (oldIsCpuSupportedFuncAddress > 0)
	{
		oldIsCpuSupportedFuncAddress = (PROC)((DWORD_PTR)oldIsCpuSupportedFuncAddress + (DWORD_PTR)7); //7 is the number of NOP's from the signature
	}

	this->oldFunctionAddress = oldIsCpuSupportedFuncAddress;

	if (oldIsCpuSupportedFuncAddress > 0 && this->newFunctionAddress != NULL)
	{
		Stub64_IsCPUSupported.movOp = 0xb848;
		Stub64_IsCPUSupported.jmpAddr = this->newFunctionAddress;
		Stub64_IsCPUSupported.jmpOp = 0xe0ff;

		ReadProcessMemory(GetCurrentProcess(), oldIsCpuSupportedFuncAddress,
			this->oldICSCode, sizeof(FARJUMP64), &bw);

		BOOL writeResult = WriteProcessMemory(GetCurrentProcess(), oldIsCpuSupportedFuncAddress,
			&Stub64_IsCPUSupported, sizeof(FARJUMP64), &bw);

		if (writeResult)
		{
			return true;
		}

	}

	return false;
}

void APIHook::UnHookByFarJump64()
{
	BYTE* oldCode = new BYTE[20];
	SIZE_T bw = 0;
	PROC orgAddress = this->oldFunctionAddress;
	DWORD Ver = GetVersion();
	Ver = ((Ver & 0xFF) << 8) | ((Ver & 0xFF00) >> 8);

	if (sizeof(PVOID) != 8)	//not 64 bit process
		return;

	if (this->oldFunctionAddress != NULL)
	{
		//restore previous code
		WriteProcessMemory(GetCurrentProcess(), orgAddress,
			this->oldICSCode, sizeof(FARJUMP64), &bw);

		this->oldFunctionAddress = NULL;
	}

	return;
}

BYTE* APIHook::HookByFarJump32(PROC newSLWin8FuncAddr)
{
	BYTE* oldCode = new BYTE[20];
	SIZE_T bw = 0;
	FARJUMP32 Stub_SLGetWindowsInformationDWORD;

	if (sizeof(PVOID) != 4)	//not 32 bit process
		return NULL;
	
	return NULL; //TODO: implement

	PROC oldIsCpuSupportedFuncAddress = GetCodePatternAddress(this->functionModule.GetBuffer(),(PBYTE)NULL/*To Implement*/,0);
	this->oldFunctionAddress = oldIsCpuSupportedFuncAddress;

	if (oldIsCpuSupportedFuncAddress != NULL && this->newFunctionAddress != NULL)
	{
		MOVFARJUMP32 Stub_IsCPUSupported;
		Stub_IsCPUSupported.movOp = 0x89;		// mov eax, ecx
		Stub_IsCPUSupported.movArg = 0xc8;	// __msfastcall compatibility
		Stub_IsCPUSupported.pushOp = 0x68;
		Stub_IsCPUSupported.pushAddr = this->newFunctionAddress;
		Stub_IsCPUSupported.retOp = 0xC3;

		ReadProcessMemory(GetCurrentProcess(), oldIsCpuSupportedFuncAddress,
			this->oldICSCode, sizeof(MOVFARJUMP32), &bw);

		bw = 0;
		WriteProcessMemory(GetCurrentProcess(), oldIsCpuSupportedFuncAddress,
			&Stub_IsCPUSupported, sizeof(MOVFARJUMP32), &bw);

	}

	return oldCode;
}

void APIHook::UnHookByFarJump32()
{
	BYTE* oldCode = new BYTE[20];
	SIZE_T bw = 0;
	
	if (sizeof(PVOID) != 4)	//not 32 bit process
		return;
	
	PROC orgAddress = this->oldFunctionAddress;

	//restore previous code
	WriteProcessMemory(GetCurrentProcess(), orgAddress,
		this->oldICSCode, sizeof(MOVFARJUMP32), &bw);

	return;
}

APIHook::~APIHook(void)
{
	this->oldFunctionAddress = NULL;
	this->newFunctionAddress = NULL;
}
