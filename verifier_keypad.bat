@echo off
chcp 65001 >nul
color 0B
echo.
echo ╔══════════════════════════════════════════════════════════════════════════════╗
echo ║                                                                              ║
echo ║   ⌨️  VÉRIFICATION DU KEYPAD 4x4                                            ║
echo ║                                                                              ║
echo ╚══════════════════════════════════════════════════════════════════════════════╝
echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ CHECKLIST DE VÉRIFICATION                                                   │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.
echo   [ ] Le keypad est bien un modèle 4x4 (16 touches)
echo   [ ] Le keypad a 8 pins (4 rows + 4 cols)
echo   [ ] Tous les câbles sont bien connectés
echo   [ ] Pas de faux contact visible
echo   [ ] La bibliothèque Keypad est installée dans Arduino IDE
echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ CÂBLAGE ATTENDU                                                              │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.
echo   Pin Keypad  →  Pin Arduino
echo   ─────────────────────────────
echo   R1 (Ligne 1) → A0
echo   R2 (Ligne 2) → A1
echo   R3 (Ligne 3) → A2
echo   R4 (Ligne 4) → A3
echo   C1 (Col 1)   → A4
echo   C2 (Col 2)   → A5
echo   C3 (Col 3)   → D2
echo   C4 (Col 4)   → D3
echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ ÉTAPE 1 : Installer la bibliothèque Keypad                                  │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.
echo   1. Ouvrir Arduino IDE
echo   2. Croquis → Inclure une bibliothèque → Gérer les bibliothèques
echo   3. Rechercher "Keypad"
echo   4. Installer "Keypad" by Mark Stanley, Alexander Brevig
echo.
pause
echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ ÉTAPE 2 : Téléverser le sketch de test                                      │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.
echo   1. Ouvrir : arduino_sketch/test_keypad/test_keypad.ino
echo   2. Sélectionner le port COM de la carte LIVRAISON
echo   3. Téléverser (bouton →)
echo.
pause
echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ ÉTAPE 3 : Tester le keypad                                                  │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.
echo   1. Ouvrir le Moniteur Série (Ctrl+Shift+M)
echo   2. Régler la vitesse à 9600 bauds
echo   3. Appuyer sur les touches du keypad
echo.
echo   ✅ Vous devriez voir :
echo      Touche appuyee : 1
echo      Touche appuyee : 5
echo      etc.
echo.
echo   ❌ Si rien ne s'affiche :
echo      → Vérifier le câblage
echo      → Vérifier que la bibliothèque est installée
echo      → Essayer d'inverser rows et cols dans le code
echo.
pause
echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ DISPOSITION DES TOUCHES                                                     │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.
echo   ┌───┬───┬───┬───┐
echo   │ 1 │ 2 │ A │ 3 │  ← Ligne 1 (R1 → A0)
echo   ├───┼───┼───┼───┤
echo   │ 4 │ 5 │ B │ 6 │  ← Ligne 2 (R2 → A1)
echo   ├───┼───┼───┼───┤
echo   │ 7 │ 8 │ C │ 9 │  ← Ligne 3 (R3 → A2)
echo   ├───┼───┼───┼───┤
echo   │ * │ 0 │ D │ # │  ← Ligne 4 (R4 → A3)
echo   └───┴───┴───┴───┘
echo     ↑   ↑   ↑   ↑
echo    C1  C2  C3  C4
echo    A4  A5  D2  D3
echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ PROBLÈMES FRÉQUENTS                                                          │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.
echo   ❌ Aucune touche ne répond
echo      → Bibliothèque Keypad non installée
echo      → Câblage incorrect
echo      → Keypad défectueux
echo.
echo   ❌ Certaines touches fonctionnent
echo      → Faux contact sur certains câbles
echo      → Vérifier les connexions
echo.
echo   ❌ Mauvaises touches détectées
echo      → Pins rows et cols inversées
echo      → Inverser dans le code
echo.
echo.
echo ┌──────────────────────────────────────────────────────────────────────────────┐
echo │ DOCUMENTATION COMPLÈTE                                                       │
echo └──────────────────────────────────────────────────────────────────────────────┘
echo.
echo   📄 GUIDE_RAPIDE_KEYPAD.txt  → Guide visuel rapide
echo   📄 DIAGNOSTIC_KEYPAD.md     → Diagnostic complet
echo   🔧 test_keypad.ino          → Sketch de test isolé
echo.
echo.
echo ╔══════════════════════════════════════════════════════════════════════════════╗
echo ║                                                                              ║
echo ║  Appuyez sur une touche pour fermer cette fenêtre                           ║
echo ║                                                                              ║
echo ╚══════════════════════════════════════════════════════════════════════════════╝
echo.
pause >nul
