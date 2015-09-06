@echo off
cd C:\nocheatz
del /S *.o
del /S *.obj
del /S *.tds
del /S *.~
del /S *.*~
del /S *.sdf
del /S *.opensdf
del /S *.suo
del /S *.tmp
rmdir /S /Q .\Debug
start /WAIT .\hash\cleanup.bat
start /WAIT .\server-plugin\cleanup.bat
start /WAIT .\autoupdate\cleanup.bat
EXIT