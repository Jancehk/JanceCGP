@echo off
del /s /q .\DBSvr\DBSvr.exe
copy /Y .\DBSvr.exe .\DBSvr\DBSvr.exe
start .\DBSvr\DBSvr.exe
ping 127.0.0.1 -n 1 >null
del /s /q .\Money\money.exe
copy /Y .\money.exe .\Money\money.exe
start .\Money\money.exe
ping 127.0.0.1 -n 1 >null
start xcap.exe

rem for /l %%i in (1,1,10) do (test &echo %%i)