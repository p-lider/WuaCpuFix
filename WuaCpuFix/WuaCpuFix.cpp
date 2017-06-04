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
#include "WuaCpuFix.h"

CWuaCpuFix* CWuaCpuFix::instance = NULL;
BYTE* CWuaCpuFix::oldCode = NULL;

CWuaCpuFix::CWuaCpuFix()
{
	this->isHooked = false;
	this->hook = NULL;

	return;
}

CWuaCpuFix::~CWuaCpuFix()
{
	if (oldCode)
		delete oldCode;
	if (hook)
	{
		delete this->hook;
		this->hook = NULL;
	}
	return;
}

CWuaCpuFix* CWuaCpuFix::GetInstance()
{
	if (CWuaCpuFix::instance == NULL)
		CWuaCpuFix::instance = new CWuaCpuFix();
	return CWuaCpuFix::instance;
}

void CWuaCpuFix::HookFunctions()
{
	this->hook = new APIHook(CString("wuaueng.dll"), (PROC)New_IsCPUSupported);
	if (this->hook != NULL && this->isHooked == false)
	{
		this->StopThreads();
		
		if (sizeof(PVOID) == 8)	//means we are on x64 system - currently 32bit systems are not supported
		{
			bool result = this->hook->HookByFarJump64((PROC)New_IsCPUSupported);
			if (result)
			{
				CWuaCpuFix::LogInfo(CString("WuaCpuFix: IsCPUSupported function has been overriden successfully."));
			}
			else
			{
				CWuaCpuFix::LogInfo(CString("WuaCpuFix: IsCPUSupported function has not been overriden."));
			}
		}

		this->isHooked = true;
		this->RunThreads();
	}
	return;
}

void CWuaCpuFix::UnHookFunctions()
{
	if (this->hook != NULL && this->isHooked == true)
	{
		this->StopThreads();

		if (sizeof(PVOID) == 8)	//means we are on x64 system - currently 32bit systems are not supported
			this->hook->UnHookByFarJump64();

		if (CWuaCpuFix::oldCode)
			delete CWuaCpuFix::oldCode;
		CWuaCpuFix::oldCode = NULL;

		this->isHooked = false;
		this->RunThreads();

	}
	return;
}


bool CWuaCpuFix::IsHooked()
{
	return this->isHooked;
}

void CWuaCpuFix::RunThreads()
{
	HANDLE h, thrHandle;
	DWORD currTh, currPr;
	THREADENTRY32 thread;

	currPr = GetCurrentProcessId();
	currTh = GetCurrentThreadId();

	h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h != INVALID_HANDLE_VALUE)
	{
		thread.dwSize = sizeof(THREADENTRY32);
		if (Thread32First(h, &thread))
		{
			do
			{
				if (thread.th32ThreadID != currTh && thread.th32OwnerProcessID == currPr)
				{
					thrHandle = OpenThread(THREAD_SUSPEND_RESUME, false, thread.th32ThreadID);
					if (thrHandle > 0)
					{
						ResumeThread(thrHandle);
						CloseHandle(thrHandle);
					}
				}
			} while (!Thread32First(h, &thread));
		}
		CloseHandle(h);
	}
}

void CWuaCpuFix::StopThreads()
{
	HANDLE h, thrHandle;
	DWORD currTh, currPr;
	THREADENTRY32 thread;

	currPr = GetCurrentProcessId();
	currTh = GetCurrentThreadId();

	h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h != INVALID_HANDLE_VALUE)
	{
		thread.dwSize = sizeof(THREADENTRY32);
		if (Thread32First(h, &thread))
		{
			do
			{
				if (thread.th32ThreadID != currTh && thread.th32OwnerProcessID == currPr)
				{
					thrHandle = OpenThread(THREAD_SUSPEND_RESUME, false, thread.th32ThreadID);
					if (thrHandle > 0)
					{
						SuspendThread(thrHandle);
						CloseHandle(thrHandle);
					}
				}
			} while (!Thread32First(h, &thread));
		}
		CloseHandle(h);
	}
}

void CWuaCpuFix::LogEvent(CString message, WORD eventType)
{
	HANDLE hEventLog = RegisterEventSource(NULL, TEXT("Application"));
	LPTSTR pInsertStrings[1] = { message.GetBuffer() };
	if (hEventLog != NULL)
	{
		DWORD eventID = 0x60000000;
		if (eventType == EVENTLOG_ERROR_TYPE)
		{
			eventID = 0xE0000000;
		}

		ReportEvent(hEventLog, eventType, 0, eventID, NULL, 1, 0, (LPCTSTR*)pInsertStrings, NULL);

		DeregisterEventSource(hEventLog);
	}
}

void CWuaCpuFix::LogInfo(CString message)
{
	CWuaCpuFix::LogEvent(message, EVENTLOG_INFORMATION_TYPE);
}

void CWuaCpuFix::LogError(CString message)
{
	CWuaCpuFix::LogEvent(message, EVENTLOG_ERROR_TYPE);
}

int __fastcall New_IsCPUSupported()
{
	/*FILE* debug = fopen("c:\\wuacpufix_api_dbg.txt", "w");
	fprintf(debug, "New_IsCPUSupported executed");
	fclose(debug);*/
	CWuaCpuFix::LogInfo(CString("WuaCpuFix: IsCPUSupported override."));
	return 1;	//always return TRUE
}