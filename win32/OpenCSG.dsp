# Microsoft Developer Studio Project File - Name="OpenCSG" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=OpenCSG - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OpenCSG.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OpenCSG.mak" CFG="OpenCSG - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OpenCSG - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OpenCSG - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OpenCSG - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I ".." /I "../include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/OpenCSG.lib"

!ELSEIF  "$(CFG)" == "OpenCSG - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I ".." /I "../include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"../lib/OpenCSGd.lib"

!ENDIF 

# Begin Target

# Name "OpenCSG - Win32 Release"
# Name "OpenCSG - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\src\area.cpp
# End Source File
# Begin Source File

SOURCE=..\src\area.h
# End Source File
# Begin Source File

SOURCE=..\src\batch.cpp
# End Source File
# Begin Source File

SOURCE=..\src\batch.h
# End Source File
# Begin Source File

SOURCE=..\src\channelManager.cpp
# End Source File
# Begin Source File

SOURCE=..\src\channelManager.h
# End Source File
# Begin Source File

SOURCE=..\src\context.cpp
# End Source File
# Begin Source File

SOURCE=..\src\context.h
# End Source File
# Begin Source File

SOURCE=..\src\frameBufferObject.cpp
# End Source File
# Begin Source File

SOURCE=..\src\frameBufferObject.h
# End Source File
# Begin Source File

SOURCE=..\src\frameBufferObjectExt.cpp
# End Source File
# Begin Source File

SOURCE=..\src\frameBufferObjectExt.h
# End Source File
# Begin Source File

SOURCE=..\src\glad\src\gl.cpp
# End Source File
# Begin Source File

SOURCE=..\src\glad\include\glad\gl.h
# End Source File
# Begin Source File

SOURCE=..\src\glad\include\KHR\khrplatform.h
# End Source File
# Begin Source File

SOURCE=..\src\occlusionQuery.cpp
# End Source File
# Begin Source File

SOURCE=..\src\occlusionQuery.h
# End Source File
# Begin Source File

SOURCE=..\src\offscreenBuffer.h
# End Source File
# Begin Source File

SOURCE=..\src\opencsgConfig.h
# End Source File
# Begin Source File

SOURCE=..\src\opencsgRender.cpp
# End Source File
# Begin Source File

SOURCE=..\src\opencsgRender.h
# End Source File
# Begin Source File

SOURCE=..\src\openglExt.h
# End Source File
# Begin Source File

SOURCE=..\src\openglHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\src\openglHelper.h
# End Source File
# Begin Source File

SOURCE=..\src\primitive.cpp
# End Source File
# Begin Source File

SOURCE=..\src\primitiveHelper.cpp
# End Source File
# Begin Source File

SOURCE=..\src\primitiveHelper.h
# End Source File
# Begin Source File

SOURCE=..\src\renderGoldfeather.cpp
# End Source File
# Begin Source File

SOURCE=..\src\renderSCS.cpp
# End Source File
# Begin Source File

SOURCE=..\src\scissorMemo.cpp
# End Source File
# Begin Source File

SOURCE=..\src\scissorMemo.h
# End Source File
# Begin Source File

SOURCE=..\src\settings.cpp
# End Source File
# Begin Source File

SOURCE=..\src\settings.h
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\include\opencsg.h
# End Source File
# End Group
# End Target
# End Project
