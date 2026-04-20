@echo off
echo ========================================
echo  RECOMPILATION PROJET AVEC ARDUINO
echo ========================================
echo.

echo [1/3] Nettoyage...
if exist Makefile del Makefile
if exist Makefile.Debug del Makefile.Debug
if exist Makefile.Release del Makefile.Release

echo [2/3] Generation du Makefile avec qmake...
qmake

echo [3/3] Compilation...
mingw32-make

echo.
echo ========================================
echo  TERMINE!
echo ========================================
pause
