@echo off
start /WAIT cleanup.bat 
cd C:\nocheatz\

set pathzip="C:\Program Files\7-Zip\7z.exe"
set pathrar="C:\Program Files (x86)\WinRar\Rar.exe"

set nocheatz-date=%date:~0,2%-%date:~3,2%-%date:~6,4%_%TIME:~0,2%-%TIME:~3,2%
set zipfile="C:\Backups\nocheatz\%nocheatz-date%.zip"
echo "Making Backup : %zipfile%"

mkdir C:\Backups
mkdir C:\Backups\nocheatz
cd C:\nocheatz\
%pathzip% a %zipfile% * -xr!.svn -xr!sdk-csgo-alliedmods -xr!sdk-css-alliedmods -xr!sdk-ep1-alliedmods -xr!sdk-source-2013-valve
pause