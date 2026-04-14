@echo off
set ROOT=C:\Users\lynam\Desktop\projetcpp2526-s2-2a8-smart-leather-goods-factory-management
set BUILD=%ROOT%\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug
set QT=C:\Qt\6.7.3\mingw_64\bin
set MINGW=C:\Qt\Tools\mingw1120_64\bin
set PATH=%QT%;%MINGW%;%PATH%

cd /d "%BUILD%"
"%MINGW%\mingw32-make.exe" -f Makefile.Debug 2> "%ROOT%\build_errors.txt"
echo Build exit code: %ERRORLEVEL%
echo === First 50 error lines ===
type "%ROOT%\build_errors.txt" | findstr /i "error: fatal note:" | head
