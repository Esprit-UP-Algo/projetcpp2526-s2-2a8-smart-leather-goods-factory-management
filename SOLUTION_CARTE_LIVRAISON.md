# 🚚 Solution : Carte LIVRAISON non détectée

## Problème
L'erreur "Carte d'expédition non connectée. Vérifiez que l'Arduino LIVRAISON est branché." apparaît même si la carte est branchée.

## Cause
Le système attend que chaque carte Arduino envoie un message `ROLE:xxx` au démarrage pour s'identifier. Si la carte LIVRAISON ne l'envoie pas, elle n'est pas reconnue.

## Solutions

### Solution 1 : Téléverser le bon sketch (RECOMMANDÉ)

1. **Ouvrir Arduino IDE**

2. **Ouvrir le sketch LIVRAISON** :
   - Fichier → Ouvrir
   - Naviguer vers : `arduino_sketch/arduino_sketch.ino`

3. **Sélectionner le bon port COM** :
   - Outils → Port
   - Choisir le port COM de la carte LIVRAISON

4. **Téléverser le sketch** :
   - Cliquer sur le bouton "Téléverser" (→)
   - Attendre "Téléversement terminé"

5. **Redémarrer l'application Qt**
   - Fermer et relancer l'application
   - La carte devrait maintenant être détectée

### Solution 2 : Augmenter le délai de timeout

Si le problème persiste, le délai de 3 secondes peut être trop court.

**Modifier `arduinomanager.h` ligne 23 :**

```cpp
// AVANT
static constexpr int ROLE_TIMEOUT_MS = 3000;

// APRÈS
static constexpr int ROLE_TIMEOUT_MS = 5000;  // 5 secondes
```

### Solution 3 : Vérifier manuellement le message ROLE

1. **Ouvrir Arduino IDE**
2. **Ouvrir le Moniteur Série** (Ctrl+Shift+M)
3. **Régler la vitesse** : 9600 bauds
4. **Débrancher et rebrancher la carte**
5. **Vérifier** que vous voyez : `ROLE:LIVRAISON`

Si vous ne voyez PAS ce message :
- Le mauvais sketch est téléversé → Suivre Solution 1
- La carte est défectueuse → Tester avec une autre carte

### Solution 4 : Forcer la reconnexion

Dans l'application Qt, vous pouvez forcer une nouvelle découverte :

1. Aller sur la page **Production**
2. L'indicateur Arduino devrait afficher "● 2/3 Arduino" (orange)
3. Débrancher et rebrancher la carte LIVRAISON
4. Attendre 5 secondes
5. Redémarrer l'application

## Vérification

Une fois la carte détectée, vous devriez voir :

✅ **Dans les logs de débogage** :
```
🔍 ArduinoManager: identification de 3 carte(s)...
📡 COM3 → ROLE:LIVRAISON
✅ LIVRAISON → COM3
🏁 Découverte terminée: TEMP_BALANCE=COM4 | LIVRAISON=COM3 | POINTAGE=COM5
```

✅ **Dans l'interface** :
- Indicateur vert : "● 3 Arduino connectés"
- Bouton "🚚 Expédier" fonctionnel
- Pas de message d'erreur

## Sketches Arduino requis

| Carte | Sketch | Message ROLE |
|-------|--------|--------------|
| TEMP_BALANCE | `cuirea_arduino/cuirea_arduino.ino` | `ROLE:TEMP_BALANCE` |
| LIVRAISON | `arduino_sketch/arduino_sketch.ino` | `ROLE:LIVRAISON` |
| POINTAGE | `arduino_pointage/arduino_pointage.ino` | `ROLE:POINTAGE` |

## Aide supplémentaire

Si le problème persiste après avoir suivi ces étapes :

1. Vérifier que les 3 cartes ont des ports COM différents
2. Vérifier qu'aucune autre application n'utilise le port COM
3. Tester chaque carte individuellement
4. Vérifier les câbles USB
