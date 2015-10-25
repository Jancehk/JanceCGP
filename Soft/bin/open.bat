@echo off
set RootPath=D:\store\branches\Soft\
echo ##complice CPSS.lib##
cd %RootPath%Framwork\make\windows\vc6.0
msdev cpss.dsw
echo ##complice XCAP##
cd %RootPath%xcap\make\windows\vc6.0
msdev xcap.dsw
echo ##complice DBSvr##
cd %RootPath%DBSvr\make\windows\vc6.0
msdev DBSvr.dsw
echo ##complice Money##
cd %RootPath%money\make\windows\vc6.0
msdev money.dsw
exit
echo ##complice CJanceDBdll##
cd %RootPath%DBDll\make\windows\vc6.0
msdev CJanceDBdll.dsw /make "- Win32 Debug"
cd %RootPath%bin
pause