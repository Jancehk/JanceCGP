@echo off
echo ----------------------------------------------------
echo Press any key to delete all files with ending:
echo  *.aps *.idb *.ncp *.obj *.pch *.tmp *.sbr *.trc
echo Visual c++/.Net junk 
echo ----------------------------------------------------

del /F /Q /S *.htm *.aps *.idb *.ncp *.obj *.pch *.sbr *.tmp *.pdb *.bsc *.ilk *.res *.ncb *.opt *.suo *.manifest
del /F /Q /S *.dep *.trc *.tli *.tlh *.log *.lib *.exp *.tlog


