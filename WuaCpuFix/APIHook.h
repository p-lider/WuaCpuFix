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

#pragma once
#include "Dbghelp.h"
#include "Tlhelp32.h"
#include "Psapi.h"
#include <atlstr.h>

#define MakePtr( cast, ptr, addValue ) (cast)( (LONG_PTR)(ptr)+(LONG_PTR)(addValue))

#pragma pack(push)  /* push current alignment to stack */
#pragma pack(1)     /* set alignment to 1 byte boundary */

typedef struct far_jump_64 {
    unsigned short movOp;
    PVOID jmpAddr;
	unsigned short jmpOp;
} FARJUMP64, *LPFARJUMP64;

typedef struct far_jump_32 {
    //unsigned char jmpOp;
	//PVOID pushAddr;
	unsigned char pushOp;
    PVOID pushAddr;
	unsigned char retOp;
} FARJUMP32, *LPFARJUMP32;

typedef struct mov_far_jump_32 {
    unsigned char movOp;
	unsigned char movArg;
	unsigned char pushOp;
    PVOID pushAddr;
	unsigned char retOp;
} MOVFARJUMP32, *LPMOVFARJUMP32;

typedef struct func_pattern {
	unsigned short osVersion;
	byte patternLength;
	BYTE pattern[255];
} FUNCPATTERN, *LPFUNCPATTERN;

const int NumberOfPatterns = 4;

const FUNCPATTERN Patterns64[NumberOfPatterns] =
{ 
	FUNCPATTERN{ {0x0601} , { 40 }, { 0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x4c,0x8b,0xdc,0x49,0x89,0x5b,0x08,0x49,0x89,0x6b,0x10,0x49,0x89,0x73,0x18,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x81,0xec,0xc0,0x00,0x00,0x00,0x48,0x8b } },
	FUNCPATTERN{ {0x0601} , { 42 }, { 0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x4c,0x8b,0xdc,0x49,0x89,0x5b,0x08,0x49,0x89,0x6b,0x10,0x49,0x89,0x73,0x18,0x57,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x81,0xec,0xd0,0x00,0x00,0x00,0x48,0x8b,0x05,0xea } },

	FUNCPATTERN{ {0x0603} , { 55 }, { 0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x48,0x8B,0xC4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,0x55,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8D,0x68,0xA1,0x48,0x81,0xEC,0xD0,0x00,0x00,0x00,0x48,0x8B,0x05,0x22,0xDF,0x13,0x00,0x48,0x33,0xC4,0x48,0x89,0x45 } },
	FUNCPATTERN{ {0x0603} , { 47 }, { 0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x48,0x8b,0xc4,0x48,0x89,0x58,0x08,0x48,0x89,0x70,0x10,0x48,0x89,0x78,0x18,0x55,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57,0x48,0x8d,0x68,0xa1,0x48,0x81,0xec,0xd0,0x00,0x00,0x00,0x48,0x8b,0x05,0x9e,0xe0 } },
};

#pragma pack(pop)   /* restore original alignment from stack */

class APIHook
{
private:
	PROC newFunctionAddress;
	CString functionName;
	CString functionModule;
	PBYTE oldICSCode[20];
	PROC WINAPI HookImportedFunction(
	HMODULE hModule, //Module to intercept calls from
     CString FunctionModule, //The dll file that contains the function you want to 
	 CString FunctionName,   //The function that you want to hook 
     PROC pfnNewProc);     //New function, this gets called instead
	DWORD_PTR FindPattern(DWORD_PTR dwAddress, DWORD dwLen, PBYTE bMask, DWORD maskLen);
	PROC GetCodePatternAddress(PTCHAR moduleName, PBYTE functionPattern, DWORD patternLen);
public:
	PROC oldFunctionAddress;
	APIHook(CString functionModule, PROC newFunctionAddress);
	APIHook(CString FunctionName,CString functionModule, PROC newFunctionAddress);
	~APIHook(void);
	BYTE* HookByFarJump32(PROC newSLWin8FuncAddr);
	BOOL HookByFarJump64(PROC newSLWin8FuncAddr);
	void UnHookByFarJump32();
	void UnHookByFarJump64();
};
