@echo off
echo ========================================
echo   CUIREA - Systeme de Detection de Defauts du Cuir
echo ========================================
echo.

cd /d "%~dp0"

echo [1/3] Demarrage de l'API de detection en arriere-plan...
start /B pythonw leather_detection_api.py
timeout /t 5 /nobreak > nul
echo       API demarree sur http://localhost:5000
echo.

echo [2/3] Lancement de l'application CUIREA...
echo.
start /WAIT "" "crud client\build\Desktop_Qt_6_7_3_MinGW_64_bit-Debug\release\CUIREA_Management.exe"

echo.
echo [3/3] Fermeture de l'API...
taskkill /F /IM pythonw.exe > nul 2>&1
echo       Systeme arrete.
echo.
pause