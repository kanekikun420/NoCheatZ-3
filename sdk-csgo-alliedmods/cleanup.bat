@echo off
cd C:\nocheatz\CSGO\
rmdir /S /Q .\common
rmdir /S /Q .\game\client
rmdir /S /Q .\game\missionchooser
rmdir /S /Q .\linux_sdk
rmdir /S /Q .\responserules
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
rmdir /S /Q .\public\missionchooser
del /S *.cpp
del /S *.sln
del /S *.vcproj
rmdir /S /Q .\lib\common
rmdir /S /Q .\lib\linux32
rmdir /S /Q .\lib\osx32
rmdir /S /Q .\lib\win32
del .\lib\public\bitmap.lib
del .\lib\public\choreoobjects.lib
del .\lib\public\dmxloader.lib
del .\lib\public\npsclient.lib
del .\lib\public\responserules_runtime.lib
del .\lib\public\nvtristrip.lib
del .\lib\public\particles.lib
del .\lib\public\steam_api.lib
del .\lib\public\vbsp2lib.lib
del .\lib\public\videocfg.lib
del .\lib\public\tier3.lib
del .\lib\public\vgui_controls.lib
del .\lib\public\vmpi.lib
del .\lib\public\vtf.lib
del .\lib\linux\tier3_i486.a
del .\lib\linux\particles_i486.a
del .\lib\linux\dmxloader_i486.a
del .\lib\linux\choreoobjects_i486.a
rmdir /S /Q .\lib\mac
EXIT