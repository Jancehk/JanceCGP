@echo off
set RootPath=D:\store\branches\Soft\
rem del /s /q %RootPath%Framwork\lib\cpss.lib
taskkill /f /im DBsvr.exe
taskkill /f /im money.exe
taskkill /f /im XCAP.exe
cd ..\
rem call Clear.bat
cd .\bin
del /s /q *.ilk *.lib *.exe *.dll
del /s /q %RootPath%\bin\make.txt
call :COMP CPSS.lib "%RootPath%Framwork\make\windows\vc6.0" cpss.dsw
call :COMP DBSvr   "%RootPath%DBSvr\make\windows\vc6.0"       DBSvr.dsw
call :COMP XCAP    "%RootPath%xcap\make\windows\vc6.0"          xcap.dsw
call :COMP Money  "%RootPath%money\make\windows\vc6.0"       money.dsw
call :COMP DBdll    "%RootPath%DBDll\make\windows\vc6.0"          CJanceDBdll.dsw
cd %RootPath%
del /s /q *.ilk >>%RootPath%\bin\make.txt
exit
pause
echo ##complice CPSS.lib##
cd %RootPath%Framwork\make\windows\vc6.0
msdev cpss.dsw /make "- Win32 Debug"
rem /out D:\store\branches\Soft\bin\result.txt
echo ##complice XCAP##
cd %RootPath%xcap\make\windows\vc6.0
msdev xcap.dsw /make "- Win32 Debug"
echo ##complice DBSvr##
cd %RootPath%DBSvr\make\windows\vc6.0
msdev DBSvr.dsw /make "- Win32 Debug"  
echo ##complice Money##
cd %RootPath%money\make\windows\vc6.0
msdev money.dsw /make "- Win32 Debug"
echo ##complice CJanceDBdll##
cd %RootPath%DBDll\make\windows\vc6.0
msdev CJanceDBdll.dsw /make "- Win32 Debug"
del /s /q *.ilk
exit
pause
cd %RootPath%bin

:COMP
echo ##complice %1##
cd %2
rem msdev %3 /make "- Win32 Debug"
msdev %3 /make "- Win32 Release" >>%RootPath%\bin\make.txt
goto :EOF