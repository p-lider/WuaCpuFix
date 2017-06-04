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

void CloseAllHostedServices();
void LogDebugInfoToFile(LPTSTR message);
CWuaCpuFix *wrapper = NULL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	WCHAR* message = new WCHAR[200];
	DWORD currentPID = GetCurrentProcessId();

	swprintf(message, L"PROCESS ATTACH - pid %d", currentPID);

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		wrapper = CWuaCpuFix::GetInstance();
		wrapper->UnHookFunctions();
		//CloseAllHostedServices();
		//TerminateProcess(GetCurrentProcess(), 0);
		FreeLibrary(GetModuleHandle(TEXT("wuaueng.dll")));
		break;
	}
	return TRUE;
}

extern "C" __declspec(dllexport) void WINAPI GetAUOptionsEx();
extern "C" __declspec(dllexport) void WINAPI WUCheckForUpdatesAtShutdown();
extern "C" __declspec(dllexport) void WINAPI WUAutoUpdateAtShutdown();
extern "C" __declspec(dllexport) DWORD WINAPI GeneralizeForImaging();
extern "C" __declspec(dllexport) int __fastcall GetEngineStatusInfo(void*);
extern "C" __declspec(dllexport) int WINAPI RegisterServiceVersion(DWORD, void*);
extern "C" __declspec(dllexport) DWORD WINAPI ServiceMain(DWORD, void*, void*, int);
extern "C" __declspec(dllexport) DWORD WINAPI WUServiceMain(DWORD, LPTSTR *);

typedef DWORD(WINAPI *SMFunc)(DWORD, void*, void*, int);
typedef DWORD (WINAPI *WUSMFunc)(DWORD, LPTSTR *);
typedef HRESULT(WINAPI *DllInstallFunc)(BOOL, LPTSTR *);
typedef HRESULT(WINAPI *DllRegUnregServerFunc)();
typedef DWORD (WINAPI *SHFunc)(DWORD, DWORD, void *, void *);
typedef int(WINAPI *RSVFunc)(DWORD, void *);
typedef int(__fastcall *GESIFunc)(void *);
typedef DWORD(WINAPI *GFIFunc)();
typedef void(WINAPI *SimpleFunc)();

extern "C" __declspec(dllexport) void WINAPI GetAUOptionsEx()
{
	HMODULE hModule = LoadLibrary(TEXT("wuaueng.dll"));
	wrapper = CWuaCpuFix::GetInstance();
	if (hModule)
	{
		SimpleFunc foo = (SimpleFunc)GetProcAddress(hModule, "GetAUOptionsEx");
		if (foo != NULL)
		{
			if (wrapper && !wrapper->IsHooked())
				wrapper->HookFunctions();
			foo();
		}
	}
}

extern "C" __declspec(dllexport) void WINAPI WUCheckForUpdatesAtShutdown()
{
	HMODULE hModule = LoadLibrary(TEXT("wuaueng.dll"));
	wrapper = CWuaCpuFix::GetInstance();
	if (hModule)
	{
		SimpleFunc foo = (SimpleFunc)GetProcAddress(hModule, "WUCheckForUpdatesAtShutdown");
		if (foo != NULL)
		{
			if (wrapper && !wrapper->IsHooked())
				wrapper->HookFunctions();
			foo();
		}
	}
}

extern "C" __declspec(dllexport) void WINAPI WUAutoUpdateAtShutdown()
{
	HMODULE hModule = LoadLibrary(TEXT("wuaueng.dll"));
	wrapper = CWuaCpuFix::GetInstance();
	if (hModule)
	{
		SimpleFunc foo = (SimpleFunc)GetProcAddress(hModule, "WUAutoUpdateAtShutdown");
		if (foo != NULL)
		{
			if (wrapper && !wrapper->IsHooked())
				wrapper->HookFunctions();
			foo();
		}
	}
}

extern "C" __declspec(dllexport) DWORD WINAPI GeneralizeForImaging()
{
	HMODULE hModule = LoadLibrary(TEXT("wuaueng.dll"));
	wrapper = CWuaCpuFix::GetInstance();
	if (hModule)
	{
		GFIFunc foo = (GFIFunc)GetProcAddress(hModule, "GeneralizeForImaging");
		if (foo != NULL)
		{
			if (wrapper && !wrapper->IsHooked())
				wrapper->HookFunctions();
			return foo();
		}
	}
	return 0;
}

extern "C" __declspec(dllexport) int __fastcall GetEngineStatusInfo(void* arg1)
{
	HMODULE hModule = LoadLibrary(TEXT("wuaueng.dll"));
	wrapper = CWuaCpuFix::GetInstance();
	if (hModule)
	{
		GESIFunc foo = (GESIFunc)GetProcAddress(hModule, "GetEngineStatusInfo");
		if (foo != NULL)
		{
			if (wrapper && !wrapper->IsHooked())
				wrapper->HookFunctions();
			return foo(arg1);
		}
	}
	return 0;
}

extern "C" __declspec(dllexport) int WINAPI RegisterServiceVersion(DWORD arg1, void* arg2)
{
	HMODULE hModule = LoadLibrary(TEXT("wuaueng.dll"));
	wrapper = CWuaCpuFix::GetInstance();
	if (hModule)
	{
		RSVFunc foo = (RSVFunc)GetProcAddress(hModule, "RegisterServiceVersion");
		if (foo != NULL)
		{
			if (wrapper && !wrapper->IsHooked())
				wrapper->HookFunctions();
			return foo(arg1, arg2);
		}
	}
	return 0;
}

extern "C" __declspec(dllexport) DWORD WINAPI ServiceMain(DWORD arg1, void* arg2, void* arg3, int arg4)
{
	HMODULE hModule = LoadLibrary(TEXT("wuaueng.dll"));
	wrapper = CWuaCpuFix::GetInstance();
	if (hModule)
	{
		SMFunc foo = (SMFunc)GetProcAddress(hModule,"ServiceMain");
		if (foo != NULL)
		{
			if (wrapper && !wrapper->IsHooked())
				wrapper->HookFunctions();
			return foo(arg1, arg2, arg3, arg4);
		}
	}
	return 0;
}

extern "C" __declspec(dllexport) DWORD WINAPI WUServiceMain(DWORD arg1, LPTSTR *arg2)
{
	HMODULE hModule = LoadLibrary(TEXT("wuaueng.dll"));
	wrapper = CWuaCpuFix::GetInstance();

	if (hModule)
	{
		WUSMFunc foo = (WUSMFunc)GetProcAddress(hModule, "WUServiceMain");
		if (foo != NULL)
		{
			if (wrapper && !wrapper->IsHooked())
				wrapper->HookFunctions();
			return foo(arg1, arg2);
		}
	}
	return 0;
}

void LogDebugInfoToFile(LPTSTR message)
{
	HANDLE hFile = CreateFile(TEXT("c:\\debug.txt"), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile)
	{
		DWORD written = 0;
		WriteFile(hFile, message, lstrlen(message), &written, NULL);
		CloseHandle(hFile);
	}
}

void CloseAllHostedServices()
{
	ENUM_SERVICE_STATUS_PROCESS *services = new ENUM_SERVICE_STATUS_PROCESS[128];
	SC_HANDLE servicesManager = OpenSCManager(NULL,NULL,SC_MANAGER_ENUMERATE_SERVICE);
	DWORD sizeNeeded = 0,servicesReturned = 0;

	if (servicesManager)
	{
		if (EnumServicesStatusEx(servicesManager, 
									SC_ENUM_PROCESS_INFO, 
									SERVICE_WIN32_SHARE_PROCESS, 
									SERVICE_ACTIVE,(LPBYTE)services,128*sizeof(ENUM_SERVICE_STATUS_PROCESS),
									&sizeNeeded, &servicesReturned, NULL,NULL ))
		{
			for (int i=0;i<servicesReturned;i++)
			{
				ENUM_SERVICE_STATUS_PROCESS sInfo = services[i];
				if (sInfo.ServiceStatusProcess.dwProcessId == GetCurrentProcessId())
				{
					SC_HANDLE hService = OpenService(servicesManager, sInfo.lpServiceName, SERVICE_STOP );
					if (hService)
					{
						SERVICE_STATUS sStatus;
						memset(&sStatus,0,sizeof(sStatus));
						ControlService(hService,SERVICE_CONTROL_STOP,&sStatus);
						CloseServiceHandle(hService);	
					}
				}
			}
		}
		CloseServiceHandle(servicesManager);
	}
	delete services;
}