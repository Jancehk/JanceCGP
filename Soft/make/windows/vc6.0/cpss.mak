# Microsoft Developer Studio Generated NMAKE File, Based on cpss.dsp
!IF "$(CFG)" == ""
CFG=cpss - Win32 Debug
!MESSAGE No configuration specified. Defaulting to cpss - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "cpss - Win32 Release" && "$(CFG)" != "cpss - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cpss.mak" CFG="cpss - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cpss - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "cpss - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "cpss - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\..\..\lib\cpss.lib" "$(OUTDIR)\cpss.bsc"


CLEAN :
	-@erase "$(INTDIR)\cpss_com_app.obj"
	-@erase "$(INTDIR)\cpss_com_app.sbr"
	-@erase "$(INTDIR)\cpss_com_pid.obj"
	-@erase "$(INTDIR)\cpss_com_pid.sbr"
	-@erase "$(INTDIR)\cpss_linux_entery.obj"
	-@erase "$(INTDIR)\cpss_linux_entery.sbr"
	-@erase "$(INTDIR)\cpss_public.obj"
	-@erase "$(INTDIR)\cpss_public.sbr"
	-@erase "$(INTDIR)\cpss_tm_timer.obj"
	-@erase "$(INTDIR)\cpss_tm_timer.sbr"
	-@erase "$(INTDIR)\cpss_vk_file.obj"
	-@erase "$(INTDIR)\cpss_vk_file.sbr"
	-@erase "$(INTDIR)\cpss_vk_print.obj"
	-@erase "$(INTDIR)\cpss_vk_print.sbr"
	-@erase "$(INTDIR)\cpss_vk_shell.obj"
	-@erase "$(INTDIR)\cpss_vk_shell.sbr"
	-@erase "$(INTDIR)\cpss_vk_socket.obj"
	-@erase "$(INTDIR)\cpss_vk_socket.sbr"
	-@erase "$(INTDIR)\cpss_vk_thread.obj"
	-@erase "$(INTDIR)\cpss_vk_thread.sbr"
	-@erase "$(INTDIR)\cpss_vk_tm_sntp.obj"
	-@erase "$(INTDIR)\cpss_vk_tm_sntp.sbr"
	-@erase "$(INTDIR)\cpss_vos_sem.obj"
	-@erase "$(INTDIR)\cpss_vos_sem.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\cpss.bsc"
	-@erase "..\..\..\lib\cpss.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MT /W3 /GX /O2 /I "../../../cpss/vk/include" /I "../../../cpss/public/include" /I "../../../cpss/com/include" /I "../../../cpss/vos/include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\cpss.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cpss.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cpss_com_app.sbr" \
	"$(INTDIR)\cpss_com_pid.sbr" \
	"$(INTDIR)\cpss_linux_entery.sbr" \
	"$(INTDIR)\cpss_public.sbr" \
	"$(INTDIR)\cpss_tm_timer.sbr" \
	"$(INTDIR)\cpss_vk_file.sbr" \
	"$(INTDIR)\cpss_vk_print.sbr" \
	"$(INTDIR)\cpss_vk_shell.sbr" \
	"$(INTDIR)\cpss_vk_socket.sbr" \
	"$(INTDIR)\cpss_vk_thread.sbr" \
	"$(INTDIR)\cpss_vk_tm_sntp.sbr" \
	"$(INTDIR)\cpss_vos_sem.sbr"

"$(OUTDIR)\cpss.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\lib\cpss.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cpss_com_app.obj" \
	"$(INTDIR)\cpss_com_pid.obj" \
	"$(INTDIR)\cpss_linux_entery.obj" \
	"$(INTDIR)\cpss_public.obj" \
	"$(INTDIR)\cpss_tm_timer.obj" \
	"$(INTDIR)\cpss_vk_file.obj" \
	"$(INTDIR)\cpss_vk_print.obj" \
	"$(INTDIR)\cpss_vk_shell.obj" \
	"$(INTDIR)\cpss_vk_socket.obj" \
	"$(INTDIR)\cpss_vk_thread.obj" \
	"$(INTDIR)\cpss_vk_tm_sntp.obj" \
	"$(INTDIR)\cpss_vos_sem.obj"

"..\..\..\lib\cpss.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "cpss - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\..\..\lib\cpss.lib" "$(OUTDIR)\cpss.bsc"


CLEAN :
	-@erase "$(INTDIR)\cpss_com_app.obj"
	-@erase "$(INTDIR)\cpss_com_app.sbr"
	-@erase "$(INTDIR)\cpss_com_pid.obj"
	-@erase "$(INTDIR)\cpss_com_pid.sbr"
	-@erase "$(INTDIR)\cpss_linux_entery.obj"
	-@erase "$(INTDIR)\cpss_linux_entery.sbr"
	-@erase "$(INTDIR)\cpss_public.obj"
	-@erase "$(INTDIR)\cpss_public.sbr"
	-@erase "$(INTDIR)\cpss_tm_timer.obj"
	-@erase "$(INTDIR)\cpss_tm_timer.sbr"
	-@erase "$(INTDIR)\cpss_vk_file.obj"
	-@erase "$(INTDIR)\cpss_vk_file.sbr"
	-@erase "$(INTDIR)\cpss_vk_print.obj"
	-@erase "$(INTDIR)\cpss_vk_print.sbr"
	-@erase "$(INTDIR)\cpss_vk_shell.obj"
	-@erase "$(INTDIR)\cpss_vk_shell.sbr"
	-@erase "$(INTDIR)\cpss_vk_socket.obj"
	-@erase "$(INTDIR)\cpss_vk_socket.sbr"
	-@erase "$(INTDIR)\cpss_vk_thread.obj"
	-@erase "$(INTDIR)\cpss_vk_thread.sbr"
	-@erase "$(INTDIR)\cpss_vk_tm_sntp.obj"
	-@erase "$(INTDIR)\cpss_vk_tm_sntp.sbr"
	-@erase "$(INTDIR)\cpss_vos_sem.obj"
	-@erase "$(INTDIR)\cpss_vos_sem.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\cpss.bsc"
	-@erase "..\..\..\lib\cpss.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../cpss/vk/include" /I "../../../cpss/public/include" /I "../../../cpss/com/include" /I "../../../cpss/vos/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\cpss.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cpss_com_app.sbr" \
	"$(INTDIR)\cpss_com_pid.sbr" \
	"$(INTDIR)\cpss_linux_entery.sbr" \
	"$(INTDIR)\cpss_public.sbr" \
	"$(INTDIR)\cpss_tm_timer.sbr" \
	"$(INTDIR)\cpss_vk_file.sbr" \
	"$(INTDIR)\cpss_vk_print.sbr" \
	"$(INTDIR)\cpss_vk_shell.sbr" \
	"$(INTDIR)\cpss_vk_socket.sbr" \
	"$(INTDIR)\cpss_vk_thread.sbr" \
	"$(INTDIR)\cpss_vk_tm_sntp.sbr" \
	"$(INTDIR)\cpss_vos_sem.sbr"

"$(OUTDIR)\cpss.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"..\..\..\lib\cpss.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cpss_com_app.obj" \
	"$(INTDIR)\cpss_com_pid.obj" \
	"$(INTDIR)\cpss_linux_entery.obj" \
	"$(INTDIR)\cpss_public.obj" \
	"$(INTDIR)\cpss_tm_timer.obj" \
	"$(INTDIR)\cpss_vk_file.obj" \
	"$(INTDIR)\cpss_vk_print.obj" \
	"$(INTDIR)\cpss_vk_shell.obj" \
	"$(INTDIR)\cpss_vk_socket.obj" \
	"$(INTDIR)\cpss_vk_thread.obj" \
	"$(INTDIR)\cpss_vk_tm_sntp.obj" \
	"$(INTDIR)\cpss_vos_sem.obj"

"..\..\..\lib\cpss.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("cpss.dep")
!INCLUDE "cpss.dep"
!ELSE 
!MESSAGE Warning: cannot find "cpss.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "cpss - Win32 Release" || "$(CFG)" == "cpss - Win32 Debug"
SOURCE=..\..\..\cpss\com\source\cpss_com_app.c

"$(INTDIR)\cpss_com_app.obj"	"$(INTDIR)\cpss_com_app.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cpss\com\source\cpss_com_pid.c

"$(INTDIR)\cpss_com_pid.obj"	"$(INTDIR)\cpss_com_pid.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cpss\public\source\cpss_linux_entery.c

"$(INTDIR)\cpss_linux_entery.obj"	"$(INTDIR)\cpss_linux_entery.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cpss\public\source\cpss_public.c

"$(INTDIR)\cpss_public.obj"	"$(INTDIR)\cpss_public.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cpss\vk\source\cpss_tm_timer.c

"$(INTDIR)\cpss_tm_timer.obj"	"$(INTDIR)\cpss_tm_timer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cpss\vk\source\cpss_vk_file.c

"$(INTDIR)\cpss_vk_file.obj"	"$(INTDIR)\cpss_vk_file.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cpss\vk\source\cpss_vk_print.c

"$(INTDIR)\cpss_vk_print.obj"	"$(INTDIR)\cpss_vk_print.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cpss\vk\source\cpss_vk_shell.c

"$(INTDIR)\cpss_vk_shell.obj"	"$(INTDIR)\cpss_vk_shell.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cpss\vk\source\cpss_vk_socket.c

"$(INTDIR)\cpss_vk_socket.obj"	"$(INTDIR)\cpss_vk_socket.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cpss\vk\source\cpss_vk_thread.c

"$(INTDIR)\cpss_vk_thread.obj"	"$(INTDIR)\cpss_vk_thread.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cpss\vk\source\cpss_vk_tm_sntp.c

"$(INTDIR)\cpss_vk_tm_sntp.obj"	"$(INTDIR)\cpss_vk_tm_sntp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\cpss\vos\source\cpss_vos_sem.c

"$(INTDIR)\cpss_vos_sem.obj"	"$(INTDIR)\cpss_vos_sem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

