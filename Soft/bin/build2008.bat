@echo off
set RootPath=D:\store\branches\Soft\
set build=Debug

echo ################清理环境##################
cd /d %RootPath%
del /s /q *.ilk *.lib *.exe
cd /d D:\Study Soft\Microsoft Visual Studio 9.0\Common7\IDE\
echo ################CPSS开始编译程序##################
rem devenv %RootPath%Framwork\make\windows\vc2008\cpss.sln /rebuild %build%
devenv %RootPath%Framwork\make\windows\vc2008\cpss.sln /build %build%
echo ################XCAP开始编译程序##################
devenv %RootPath%xcap\make\windows\vc2008\xcap.sln /build %build%
echo ################DBSvr开始编译程序##################
devenv %RootPath%DBSvr\make\windows\vc2008\DBSvr.sln /build %build%
echo ################money开始编译程序##################
devenv %RootPath%money\make\windows\vc2008\money.sln /build %build%
cd /d %RootPath%
del /s /q *.ilk *.lib
exit
echo 更新 bin 目录程序
copy %CLIENT%\bin\*.*dll %pwd%\bin
copy %CLIENT%\bin\*.*dat %pwd%\bin


exit


set RootPath=D:\store\branches\Soft\
set MSDVS="D:\Study Soft\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe"
del xcap.exe
del DBSvr.exe
del money.exe
echo ##complice CPSS.lib##
cd %RootPath%Framwork\make\windows\vc2008
%MSDVS% cpss.sln /make
rem /out D:\store\branches\Soft\bin\result.txt
echo ##complice DBSvr##
cd %RootPath%DBSvr\make\windows\vc2008
start %MSDVS% DBSvr.sln
echo ##complice XCAP##
cd %RootPath%xcap\make\windows\vc2008
start %MSDVS% xcap.sln
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