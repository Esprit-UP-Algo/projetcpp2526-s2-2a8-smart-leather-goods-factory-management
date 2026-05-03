@echo off
chcp 65001 >nul
color 0A
echo.
echo ╔══════════════════════════════════════════════════════════════════════════════╗
echo ║                                                                              ║
echo ║   🔍 VÉRIFICATION DES CARTES ARDUINO                                         ║
echo ║                                                                              ║
echo ╚══════════════════════════════════════════════════════════════════════════════╝
echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ ÉTAPE 1 : Ports COM disponibles                                             │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.

:: Lister les ports COM
for /f "tokens=*" %%a in ('wmic path Win32_SerialPort get DeviceID^,Description 2^>nul') do (
    echo   %%a
)

echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ ÉTAPE 2 : Vérification des sketches                                         │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.
echo   Pour chaque carte Arduino branchée :
echo.
echo   1. Ouvrir Arduino IDE
echo   2. Ouvrir le Moniteur Série (Ctrl+Shift+M)
echo   3. Régler la vitesse à 9600 bauds
echo   4. Sélectionner le port COM
echo   5. Débrancher et rebrancher la carte
echo.
echo   ┌─────────────────────────────────────────────────────────────────────────┐
echo   │ Carte TEMP_BALANCE                                                      │
echo   ├─────────────────────────────────────────────────────────────────────────┤
echo   │ Sketch requis : cuirea_arduino/cuirea_arduino.ino                       │
echo   │ Message attendu : ROLE:TEMP_BALANCE                                     │
echo   │ Fonction : Température + Balance                                        │
echo   └─────────────────────────────────────────────────────────────────────────┘
echo.
echo   ┌─────────────────────────────────────────────────────────────────────────┐
echo   │ Carte LIVRAISON                                                         │
echo   ├─────────────────────────────────────────────────────────────────────────┤
echo   │ Sketch requis : arduino_sketch/arduino_sketch.ino                       │
echo   │ Message attendu : ROLE:LIVRAISON                                        │
echo   │ Fonction : Expédition (servos + LCD + keypad)                           │
echo   └─────────────────────────────────────────────────────────────────────────┘
echo.
echo   ┌─────────────────────────────────────────────────────────────────────────┐
echo   │ Carte POINTAGE                                                          │
echo   ├─────────────────────────────────────────────────────────────────────────┤
echo   │ Sketch requis : arduino_pointage/arduino_pointage.ino                   │
echo   │ Message attendu : ROLE:POINTAGE                                         │
echo   │ Fonction : Pointage RFID                                                │
echo   └─────────────────────────────────────────────────────────────────────────┘
echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ ÉTAPE 3 : Checklist de vérification                                         │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.
echo   [ ] Les 3 cartes Arduino sont branchées en USB
echo   [ ] Chaque carte a un port COM différent
echo   [ ] Arduino IDE est fermé (pour libérer les ports COM)
echo   [ ] Chaque carte envoie son message ROLE au démarrage
echo   [ ] Les câbles USB sont en bon état
echo   [ ] Les drivers USB sont installés
echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ ÉTAPE 4 : Lancer l'application                                              │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.
echo   Une fois toutes les vérifications faites :
echo.
echo   1. Fermer Arduino IDE
echo   2. Lancer : debug\CUIREA_Management.exe
echo   3. Aller sur la page Production
echo   4. Vérifier l'indicateur Arduino :
echo.
echo      ● 3 Arduino connectés  (VERT)   → ✅ Tout fonctionne
echo      ● 2/3 Arduino          (ORANGE) → ⚠️  Une carte manquante
echo      ● Non connecté         (ROUGE)  → ❌ Aucune carte détectée
echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ PROBLÈME AVEC LA CARTE LIVRAISON ?                                          │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.
echo   Voir les fichiers de documentation :
echo.
echo   📄 GUIDE_RAPIDE_LIVRAISON.txt     → Guide rapide
echo   📄 SOLUTION_CARTE_LIVRAISON.md    → Solutions détaillées
echo   📄 DIAGNOSTIC_LIVRAISON.md        → Diagnostic complet
echo.
echo.
echo ╔══════════════════════════════════════════════════════════════════════════════╗
echo ║                                                                              ║
echo ║  Appuyez sur une touche pour fermer cette fenêtre                           ║
echo ║                                                                              ║
echo ╚══════════════════════════════════════════════════════════════════════════════╝
echo.
pause >nul
