@echo off
start DBSvr.exe
ping 127.0.0.1 -n 1 >null
start money.exe
ping 127.0.0.1 -n 1 >null
start xcap.exe

rem for /l %%i in (1,1,10) do (test &echo %%i)