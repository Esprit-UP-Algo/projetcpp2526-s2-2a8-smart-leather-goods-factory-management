@echo off
set ROOT=%~dp0
set BUILD=%ROOT%build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug
set QT=C:\Qt\6.7.3\mingw_64\bin
set MINGW=C:\Qt\Tools\mingw1120_64\bin
set PATH=%QT%;%MINGW%;%PATH%

echo Killing CUIREA_Management.exe if running...
taskkill /F /IM CUIREA_Management.exe 2>nul

echo Running qmake...
cd /d "%BUILD%"
"%QT%\qmake.exe" ..\..\sans_titre.pro -spec win32-g++

echo Building...
"%MINGW%\mingw32-make.exe" -f Makefile.Debug

echo.
echo Build finished with exit code: %ERRORLEVEL%
