@echo off
cd C:\nocheatz\server-plugin
del /S *.o
del /S *.obj
del /S *.tds
del /S *.~
del /S *.sdf
del /S *.opensdf
del /S *.suo
rmdir /S /Q .\Debug
EXIT