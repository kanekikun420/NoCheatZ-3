@echo off
cd C:\nocheatz\CSP\
rmdir /S /Q .\choreoobjects
rmdir /S /Q .\common
rmdir /S /Q .\devtools
rmdir /S /Q .\linux_sdk
rmdir /S /Q .\utils
rmdir /S /Q .\vgui2
rmdir /S /Q .\lib-vc7
rmdir /S /Q .\cl_dll\episodic
rmdir /S /Q .\cl_dll\game_controls
rmdir /S /Q .\cl_dll\hl2_hud
rmdir /S /Q .\cl_dll\hl2mp
rmdir /S /Q .\cl_dll\sdk
rmdir /S /Q .\dlls\episodic
rmdir /S /Q .\dlls\hl2_dll
rmdir /S /Q .\dlls\hl2mp_dll
rmdir /S /Q .\dlls\sdk
rmdir /S /Q .\game_shared\gamestats
rmdir /S /Q .\game_shared\hl2
rmdir /S /Q .\game_shared\hl2mp
rmdir /S /Q .\game_shared\sdk
rmdir /S /Q .\public\Friends
rmdir /S /Q .\public\VGuiMatSurface
rmdir /S /Q .\public\avi
rmdir /S /Q .\public\bitmap
rmdir /S /Q .\public\dmserializers
rmdir /S /Q .\public\dmxloader
rmdir /S /Q .\public\g15
rmdir /S /Q .\public\game\client
rmdir /S /Q .\public\jpeglib
rmdir /S /Q .\public\maya
rmdir /S /Q .\public\mdllib
rmdir /S /Q .\public\p4lib
rmdir /S /Q .\public\particles
rmdir /S /Q .\public\phonemeexctractor
rmdir /S /Q .\public\scenefilecache
rmdir /S /Q .\public\shaderlib
rmdir /S /Q .\public\tier3
rmdir /S /Q .\public\vgui
rmdir /S /Q .\public\vgui_controls
rmdir /S /Q .\public\vtf
rmdir /S /Q .\public\zip
del /S *.cpp
del /S *.sln
del /S *.vcproj
del filecopy.bat
rmdir /S /Q .\lib\common
rmdir /S /Q .\lib\vmpi
del .\lib\public\bitmap.lib
del .\lib\public\choreoobjects.lib
del .\lib\public\dme_controls.lib
del .\lib\public\fgdlib.lib
del .\lib\public\jcalg1_static.lib
del .\lib\public\movieobjects.lib
del .\lib\public\nvtc.lib
del .\lib\public\nvtristrip.lib
del .\lib\public\particles.lib
del .\lib\public\quantize.lib
del .\lib\public\scenedatabase.lib
del .\lib\public\shaderlib.lib
del .\lib\public\steam_api.lib
del .\lib\public\tier3.lib
del .\lib\public\toolutils.lib
del .\lib\public\unitlib.lib
del .\lib\public\vgui_controls.lib
del .\lib\public\vgui_surfacelib.lib
del .\lib\public\vmpi.lib
del .\lib\public\vtf.lib
rmdir /S /Q .\lib\mac
EXIT