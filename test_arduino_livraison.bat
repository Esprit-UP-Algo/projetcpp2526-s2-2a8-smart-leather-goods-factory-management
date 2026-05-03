@echo off
echo ========================================
echo Test de detection Arduino LIVRAISON
echo ========================================
echo.

echo 1. Verification des ports COM disponibles...
echo.
mode

echo.
echo ========================================
echo 2. Instructions :
echo ========================================
echo.
echo a) Ouvrez Arduino IDE
echo b) Ouvrez le Moniteur Serie (Ctrl+Shift+M)
echo c) Reglez la vitesse a 9600 bauds
echo d) Selectionnez le port COM de la carte LIVRAISON
echo e) Debranche et rebranche la carte
echo.
echo Vous devriez voir :
echo   ROLE:LIVRAISON
echo   Systeme pret
echo   Saisir ID + D
echo.
echo Si vous ne voyez PAS "ROLE:LIVRAISON" :
echo   - Le mauvais sketch est televerse
echo   - Ouvrez arduino_sketch/arduino_sketch.ino
echo   - Televerser sur la carte LIVRAISON
echo.

pause
