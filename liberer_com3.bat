@echo off
echo Liberation du port COM3...

:: Tuer tous les processus Arduino qui bloquent le port serie
taskkill /IM "avrdude.exe"                /F 2>nul
taskkill /IM "arduino-cli.exe"            /F 2>nul
taskkill /IM "serial-discovery.exe"       /F 2>nul
taskkill /IM "mdns-discovery.exe"         /F 2>nul
taskkill /IM "arduino-language-server.exe"/F 2>nul

:: Tuer l'application Qt si elle tourne
taskkill /IM "sans_titre.exe" /F 2>nul
taskkill /IM "cuirea.exe"     /F 2>nul

echo.
echo Fait ! Maintenant :
echo  1. Debranche l'Arduino USB
echo  2. Rebranche l'Arduino USB
echo  3. Upload dans Arduino IDE
echo.
pause
