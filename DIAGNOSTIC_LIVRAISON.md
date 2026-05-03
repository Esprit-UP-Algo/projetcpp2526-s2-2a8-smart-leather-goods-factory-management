# 🔧 Diagnostic : Carte LIVRAISON branchée mais non détectée

## ✅ Modifications appliquées

### 1. Message d'erreur amélioré
Le message d'erreur affiche maintenant :
- L'état de connexion des 3 cartes
- Des solutions concrètes
- Un lien vers la documentation complète

**Fichier modifié** : `mainwindow.cpp` (fonction `expedierActionArduino`)

### 2. Documentation créée
- `SOLUTION_CARTE_LIVRAISON.md` : Guide complet de résolution
- `test_arduino_livraison.bat` : Script de diagnostic

### 3. Compilation réussie ✅
L'application a été recompilée avec succès.

---

## 🎯 Prochaines étapes pour résoudre votre problème

### Étape 1 : Vérifier le sketch téléversé

**Ouvrir Arduino IDE et vérifier :**

1. Ouvrir le **Moniteur Série** (Ctrl+Shift+M)
2. Régler la vitesse à **9600 bauds**
3. Sélectionner le port COM de la carte LIVRAISON
4. Débrancher et rebrancher la carte USB

**Résultat attendu :**
```
ROLE:LIVRAISON
Systeme pret
Saisir ID + D
```

**Si vous ne voyez PAS "ROLE:LIVRAISON" :**
→ Le mauvais sketch est téléversé sur cette carte

### Étape 2 : Téléverser le bon sketch

1. **Ouvrir** : `arduino_sketch/arduino_sketch.ino`
2. **Sélectionner** le port COM de la carte LIVRAISON
3. **Téléverser** (bouton →)
4. **Vérifier** dans le Moniteur Série que vous voyez maintenant `ROLE:LIVRAISON`

### Étape 3 : Redémarrer l'application Qt

1. Fermer l'application CUIREA
2. Lancer `debug\CUIREA_Management.exe`
3. Aller sur la page **Production**
4. Vérifier l'indicateur Arduino :
   - ✅ Vert "● 3 Arduino connectés" = Tout fonctionne
   - ⚠️ Orange "● 2/3 Arduino" = LIVRAISON manquante
   - ❌ Rouge "● Non connecté" = Aucune carte détectée

### Étape 4 : Tester l'expédition

1. Sélectionner une commande avec statut "Terminé"
2. Cliquer sur "🚚 Expédier"
3. Si la carte est bien détectée, les moteurs se déclenchent
4. Si la carte n'est pas détectée, le nouveau message d'erreur détaillé s'affiche

---

## 🔍 Causes possibles du problème

| Cause | Symptôme | Solution |
|-------|----------|----------|
| **Mauvais sketch téléversé** | Pas de "ROLE:LIVRAISON" dans le Moniteur Série | Téléverser `arduino_sketch.ino` |
| **Sketch non téléversé** | Rien dans le Moniteur Série | Téléverser `arduino_sketch.ino` |
| **Timeout trop court** | Carte détectée parfois, pas toujours | Augmenter `ROLE_TIMEOUT_MS` à 5000 |
| **Port COM utilisé** | Erreur d'ouverture du port | Fermer Arduino IDE avant de lancer Qt |
| **Câble USB défectueux** | Carte non visible dans Arduino IDE | Changer de câble USB |
| **Carte défectueuse** | Visible dans IDE mais pas de communication | Tester avec une autre carte |

---

## 📊 Tableau des sketches requis

| Carte Arduino | Sketch à téléverser | Message ROLE envoyé | Fonction |
|---------------|---------------------|---------------------|----------|
| **TEMP_BALANCE** | `cuirea_arduino/cuirea_arduino.ino` | `ROLE:TEMP_BALANCE` | Température + Balance |
| **LIVRAISON** | `arduino_sketch/arduino_sketch.ino` | `ROLE:LIVRAISON` | Expédition (servos + LCD + keypad) |
| **POINTAGE** | `arduino_pointage/arduino_pointage.ino` | `ROLE:POINTAGE` | Pointage RFID |

---

## 🛠️ Commandes de diagnostic

### Vérifier les ports COM disponibles
```bash
mode
```

### Tester la détection Arduino
```bash
test_arduino_livraison.bat
```

### Recompiler l'application
```bash
rebuild2.bat
```

### Lancer l'application en mode debug
```bash
debug\CUIREA_Management.exe
```

---

## 📝 Logs de débogage à surveiller

Quand l'application démarre, vous devriez voir dans la console :

```
=== Scan multi-cartes Arduino ===
 Port: COM3 VID: 0x2341 PID: 0x0043 Desc: Arduino Uno
  ✅ Arduino détecté sur COM3
 Port: COM4 VID: 0x2341 PID: 0x0043 Desc: Arduino Uno
  ✅ Arduino détecté sur COM4
 Port: COM5 VID: 0x2341 PID: 0x0043 Desc: Arduino Uno
  ✅ Arduino détecté sur COM5
=== Total Arduino trouvés: 3 ===

🔍 ArduinoManager: identification de 3 carte(s)...
⏳ En attente ROLE: sur COM3
⏳ En attente ROLE: sur COM4
⏳ En attente ROLE: sur COM5

📡 COM3 → ROLE:LIVRAISON
✅ LIVRAISON → COM3
📡 COM4 → ROLE:TEMP_BALANCE
✅ TEMP_BALANCE → COM4
📡 COM5 → ROLE:POINTAGE
✅ POINTAGE → COM5

🏁 Découverte terminée: TEMP_BALANCE=COM4 | LIVRAISON=COM3 | POINTAGE=COM5
✅ Carte LIVRAISON prête sur COM3
```

**Si vous voyez :**
```
⏰ Timeout identification Arduino — ports non identifiés:
  ❓ COM3 (pas de ROLE: reçu)
```
→ La carte COM3 n'a pas envoyé son message ROLE → Téléverser le bon sketch

---

## ✨ Améliorations apportées

1. **Message d'erreur informatif** : Affiche l'état de toutes les cartes
2. **Documentation complète** : Guide de résolution étape par étape
3. **Script de diagnostic** : Test automatique des ports COM
4. **Logs détaillés** : Identification claire du problème dans la console

---

## 📞 Support

Si le problème persiste après avoir suivi toutes ces étapes :

1. Vérifier que les 3 cartes ont des **ports COM différents**
2. Vérifier qu'**aucune autre application** n'utilise les ports COM
3. Tester **chaque carte individuellement** (débrancher les 2 autres)
4. Vérifier les **câbles USB** (essayer d'autres câbles)
5. Vérifier les **drivers USB** (réinstaller si nécessaire)

**Fichiers de référence :**
- `SOLUTION_CARTE_LIVRAISON.md` : Solutions détaillées
- `arduino_multi_cartes.md` : Architecture multi-cartes
- `arduino_modifications_completes.md` : Modifications complètes
