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

#include <TlHelp32.h>
#include "apihook.h"

#ifdef WuaCpuFix_EXPORTS
#define WuaCpuFix_API __declspec(dllexport)
#else
#define WuaCpuFix_API __declspec(dllimport)
#endif

typedef __success(return >= 0) LONG NTSTATUS;


typedef struct _SYSTEM_POLICY_INFORMATION {
    PVOID Reserved1[2];
    ULONG Reserved2[3];
} SYSTEM_POLICY_INFORMATION;

typedef struct _UNICODE_STRING {
  USHORT Length;
  USHORT MaximumLength;
  PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

int __fastcall New_IsCPUSupported();

// This class is exported from the WuaCpuFix.dll
class CWuaCpuFix {
public:
	void HookFunctions();
	void UnHookFunctions();
	bool IsHooked();
	static CWuaCpuFix* GetInstance();
	static void LogInfo(CString message);
	static void LogError(CString message);
	static BYTE* oldCode;
	// TODO: add your methods here.
private:
	APIHook *hook;
	CWuaCpuFix(void);
	~CWuaCpuFix(void);
	static CWuaCpuFix *instance;
	bool isHooked;
	static void RunThreads();
	static void StopThreads();
	static void LogEvent(CString message, WORD eventType);
};

/*
extern WuaCpuFix_API int nWuaCpuFix;

WuaCpuFix_API int fnWuaCpuFix(void);
*/