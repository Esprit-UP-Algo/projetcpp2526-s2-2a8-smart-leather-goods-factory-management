@echo off
set ROOT=C:\Users\lynam\Desktop\projetcpp2526-s2-2a8-smart-leather-goods-factory-management
set BUILD=%ROOT%\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug
set QT=C:\Qt\6.7.3\mingw_64\bin
set MINGW=C:\Qt\Tools\mingw1120_64\bin
set PATH=%QT%;%MINGW%;%PATH%

echo === Nettoyage complet ===
if exist "%BUILD%\build" rd /s /q "%BUILD%\build"
if exist "%BUILD%\debug" rd /s /q "%BUILD%\debug"
mkdir "%BUILD%\debug"

echo === qmake ===
cd /d "%BUILD%"
"%QT%\qmake.exe" "%ROOT%\sans_titre.pro" -spec win32-g++ "CONFIG+=debug"

echo === Build ===
"%MINGW%\mingw32-make.exe" -f Makefile.Debug -j4 2>&1

echo === Resultat ===
if exist "%BUILD%\debug\CUIREA_Management.exe" (
    echo SUCCES - exe cree
) else (
    echo ECHEC - exe absent
)
