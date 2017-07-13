@echo off
setlocal EnableDelayedExpansion
echo Windows Update Agent CPU Support Limitation Removal Installer v0.1 by p_lider
echo.

cd /d %~dp0

SET HasAdminRights=0
FOR /F %%i IN ('WHOAMI /PRIV /NH') DO (
	IF "%%i"=="SeTakeOwnershipPrivilege" SET HasAdminRights=1
)

IF NOT %HasAdminRights%==1 (
	ECHO.
	ECHO Error:No admin rights detected. 
	ECHO This script must be run as an Administrator.
	ECHO.
	GOTO END
)

echo [*] Looking for Windows Update PID...
tasklist /SVC /FI "SERVICES eq wuauserv" | find "PID" /V
echo.
if !errorlevel!==0 (
	net stop wuauserv /yes
)

set PROCESSOR_ARCHITECTURE | find "x86" > nul
if !errorlevel!==0 (
	goto WOW64CHK
) else (
	goto AMD64
)

:WOW64CHK
echo [*] Check if running WOW64 subsystem...
set PROCESSOR_ARCHITEW6432 > nul
if !errorlevel!==0 (
	echo "Error: This installer must be run from 64bit command line!"
	goto END
) else (
	goto X86
)

:X86
echo [+] Processor architecture is Intel x86 [not supported]
rem set DLLNAME=WuaCpuFix.dll
goto END

:AMD64
echo [+] Processor architecture is AMD64 [supported]
set DLLNAME=WuaCpuFix64.dll
goto INSTALL

:INSTALL
echo [*] Installing...
if not exist !DLLNAME! (
	echo [-] Error: !DLLNAME! file not found
	goto END
)

echo [*] Copying file to System32...
xcopy /y !DLLNAME! "%SystemRoot%\System32\"
if not !errorlevel!==0 (
	echo [-] Failed to copy !DLLNAME! to System32 folder
	goto END
)
echo [*] Backing up registry...
reg copy "HKLM\SYSTEM\CurrentControlSet\Services\wuauserv\Parameters" "HKLM\SYSTEM\CurrentControlSet\Services\wuauserv\Parameters_bak" /s /f
echo [*] Modifying registry...
reg add "HKLM\SYSTEM\CurrentControlSet\Services\wuauserv\Parameters" /v ServiceDll /t REG_EXPAND_SZ /d "%SystemRoot%\System32\!DLLNAME!" /f
if not !errorlevel!==0 (
	echo [-] Failed to modify registry
	goto END
)

:DONE
echo [*] Starting Windows Update...
net start wuauserv
echo [+] Installation complete!
echo Now reboot your computer.
goto END

:END
pause
