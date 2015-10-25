@echo off
set RootPath=D:\store\branches\Soft\
set MSDVS="D:\Study Soft\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe"
del xcap.exe
del DBSvr.exe
del money.exe
echo ##complice CPSS.lib##
cd %RootPath%Framwork\make\windows\vc2008
start %MSDVS% cpss.sln 
rem /out D:\store\branches\Soft\bin\result.txt
echo ##complice XCAP##
cd %RootPath%xcap\make\windows\vc2008
start %MSDVS% xcap.sln
exit
echo ##complice DBSvr##
cd %RootPath%DBSvr\make\windows\vc2008
start %MSDVS% DBSvr.sln
echo ##complice Money##
cd %RootPath%money\make\windows\vc2008
start %MSDVS% money.sln
pause
exit
echo ##complice CJanceDBdll##
cd %RootPath%DBDll\make\windows\vc2008
msdev CJanceDBdll.sln /make "- Win32 Debug"
cd %RootPath%bin
pause