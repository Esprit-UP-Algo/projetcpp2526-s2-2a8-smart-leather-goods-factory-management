@echo off
set ROOT=C:\Users\lynam\Desktop\projetcpp2526-s2-2a8-smart-leather-goods-factory-management
set BUILD=%ROOT%\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug
set QT=C:\Qt\6.7.3\mingw_64\bin
set MINGW=C:\Qt\Tools\mingw1120_64\bin
set PATH=%QT%;%MINGW%;%PATH%

echo === Cleaning parasite folders ===
if exist "%BUILD%\build" rmdir /s /q "%BUILD%\build"

echo === Running qmake from correct dir ===
cd /d "%BUILD%"
"%QT%\qmake.exe" "%ROOT%\sans_titre.pro" -spec win32-g++ "CONFIG+=debug"

echo === Building ===
"%MINGW%\mingw32-make.exe" -f Makefile.Debug 2>&1 | findstr /i "error: fatal note:"

echo === Done ===
