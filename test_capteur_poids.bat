@echo off
echo ========================================
echo TEST CAPTEUR DE POIDS HX711
echo ========================================
echo.
echo Instructions:
echo 1. Assurez-vous que l'Arduino est connecte
echo 2. Uploadez le sketch cuirea_arduino.ino
echo 3. Ouvrez le moniteur serie (9600 baud)
echo 4. Tapez: GET_WEIGHT
echo 5. Verifiez que vous recevez: WEIGHT:XX.XX
echo.
echo Si vous recevez ERROR:SCALE_NOT_READY:
echo   - Verifiez le cablage HX711 (DOUT=4, SCK=5)
echo   - Verifiez l'alimentation du capteur
echo   - Verifiez que le capteur est bien connecte
echo.
echo Pour calibrer:
echo 1. Tapez: TARE (remettre a zero)
echo 2. Placez un poids connu (ex: 1kg)
echo 3. Ajustez le facteur de calibration dans le code
echo.
pause
