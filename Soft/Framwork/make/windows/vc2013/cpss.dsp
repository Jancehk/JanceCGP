# Microsoft Developer Studio Project File - Name="cpss" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=cpss - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cpss.mak".
!MESSAGE 
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

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cpss - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cpss - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "../../../cpss/vk/include" /I "../../../cpss/public/include" /I "../../../cpss/com/include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /FD /GZ /c
# SUBTRACT CPP /YX
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\..\lib\cpss.lib"

!ENDIF 

# Begin Target

# Name "cpss - Win32 Release"
# Name "cpss - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\cpss\com\source\cpss_com_app.c
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\com\source\cpss_com_pid.c
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\public\source\cpss_linux_entery.c
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\public\source\cpss_public.c
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\vk\source\cpss_tm_timer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\vk\source\cpss_vk_file.c
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\vk\source\cpss_vk_print.c
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\vk\source\cpss_vk_socket.c
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\vk\source\cpss_vk_thread.c
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\vk\source\cpss_vk_tm_sntp.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\cpss\com\include\cpss_com_app.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\com\include\cpss_com_pid.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\public\include\cpss_common.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\public\include\cpss_config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\public\include\cpss_linux_config.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\public\include\cpss_linux_entery.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\public\include\cpss_msg.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\public\include\cpss_public.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\vk\include\cpss_tm_timer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\vk\include\cpss_vk_file.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\vk\include\cpss_vk_print.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\vk\include\cpss_vk_socket.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\vk\include\cpss_vk_thread.h
# End Source File
# Begin Source File

SOURCE=..\..\..\cpss\vk\include\cpss_vk_tm_sntp.h
# End Source File
# End Group
# End Target
# End Project
