## Windows Update Agent CPU Support Limitation Removal

## Description
This tool (which is really a simple DLL library) is given "AS IS" without any warranty **for testing/academical purpose only**. It is designed to allow updates to be installed using Windows Update on Windows 7 and Windows 8.1 systems running Intel Kaby Lake and AMD Ryzen processors or the later ones.

Microsoft recently disallowed any updates to be installed on such systems telling, that the company no longer supports older operating systems on newest processors. This tool circumwents that limitation.

## Caution
As mentioned above – this tool comes with totally no warranty from the author. **I do not recommend to use it** on any production environment or any device which is used for normal/regular work or any device having any valuable data stored on it.

Because any patches starting from April 2017 for Windows 7 and Windows 8.1 are likely not to be tested by Microsoft for working properly with newest processors then **this tool can potentially render such systems unusable or cause loss of data**. Why? Because it will allow such potentially not tested and not compatible updates to be installed on such systems.

**As a result of the above you use this tool completely at your own risk!!!**

## Reqirements

64 bit Windows 7 or Windows 8.1 systems. 32 bit operating systems are currently not supported.

## Installation
To install just download the "Deployment" folder and run "install.bat" with administrator rights (right click on the file and choose "Run As Administrator"). Follow the on screen instructions.

## Deinstallation

To deinstall just run "uninstall.bat" with administrator rights (right click on the file and choose "Run As Administrator") from the folder where you have previously downloaded the "Deployment" folder. Follow the on screen instructions.

## Manual installation

1. Download or build WuaCpuFix64.dll file and place it in %systemroot%\system32\ folder.
2. Open registry editor and navigate to the following key:
	HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Services\wuauserv\Parameters
3. Double click the "ServiceDLL" registry value and set its value to "%systemroot%\system32\wuacpufix64.dll" path.
4. Reboot computer.

## License

This tool is being licensed under GNU GPL License version 3.

## General implementation details

I was inspired to create this tool basing on findings made by Zeffy in his project called kb4012218-19 (https://github.com/zeffy/kb4012218-19). He did a great job finding the heart of the mechanizm Microsoft had introduced for limiting the CPU support in Windows Update. However in his solution one thing was dirty for me - direct modification of system files, which I simply do not want to do. I tried to do the same thing as he but trying to not modify a single byte on any of the system files. I think I managed to do so here.

My tool is simply a dll file which acts as a proxy to the original dll file called "***wuaueng.dll***" used by Windows Update service. Thanks to the registry setting mentioned in "manual installation" section it is being loaded by svchost.exe process directly to its memory instead of the original "wuaueng.dll" file.

When that dll is loaded and launched by **svchost.exe** process it then loads the original library and patches it in memory (by making a FAR JUMP from the original IsCpuSupported function to my function which always returns 1) before allowing its code to execute. Thanks to that no system files are being modified on disk by this tool. Such behavior preserves all integrity checks made by the system and potentially does not violate the EULA of Windows, however I am not a lawyer, so I am not sure about that.

Any feedback is welcome. Feel free to issue a bug reports or feature/improvements suggestions.

## Building the project
To build the project you have to install either Visual Studio 2013 Community or Visual Studio 2015 Community editions together with MFC C++ Libraries. 

Then just open the WuaCpuFix.sln file and build the solution. When building in Release mode the compiled dll will automatically be copied into the Deployment folder ready for installation.

## Not fully tested
I do have an AMD Ryzen 7 1700 processor at home, however I was not able to fully test this tool on my own. For some unknown reason neither Windows 7 nor Windows 8.1 were blocking updates to be installed using Windows Update on my PC. I have launched the low level debugging session to see what the original IsCpuSupported function is returning on my PC and it was always returning 1 (TRUE) but according to the official Microsoft statement it should not.

As a result I was able only to check if my tool is able to override the IsCpuSupported function to always return 1 (TRUE) which should be totally enough to say that it is working. 

If for any of you my tool is not working on 64 bit version of Windows 7 or 8.1 please let me know by creating an issue on github adding the following information:
* Version of Operating System
* Full version of wuaueng.dll file from Windows\System32 folder and its hash if possible.

Please note that on 32 bit Windows versions my tool will currently not work at all.

## First time on GitHub

Please note that this is my first project on github - I have never been here as an author/contributor, so I am not familiar with GitHub from that side yet. As a result my response time may be slow because I have to learn how this portal works, for example - how commits can be related to issues, etc.