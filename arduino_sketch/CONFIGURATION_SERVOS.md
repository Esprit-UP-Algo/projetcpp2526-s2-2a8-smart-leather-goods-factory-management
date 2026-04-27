# Configuration des Servos - Système d'Expédition

## 🎯 Vue d'ensemble

Le système utilise **2 servomoteurs** pour automatiser l'expédition des colis:

1. **BRAS (Pin 9)** - Pousse le colis hors du convoyeur
2. **BARRIÈRE (Pin 10)** - Retient les colis suivants en attente

## ⚙️ Configuration actuelle

### Servo BRAS (pousse le colis)
```cpp
const int BRAS_REPOS = 90;           // Position repos (ne touche pas)
const int BRAS_POUSSE = 180;         // Position poussée (pousse à fond)
const int BRAS_TEMPS_POUSSE = 1500;  // Temps pour pousser (ms)
const int BRAS_TEMPS_RETOUR = 1000;  // Temps pour revenir (ms)
```

### Servo BARRIÈRE (retient/libère)
```cpp
const int BARRIERE_FERMEE = 90;      // Position fermée (bloque)
const int BARRIERE_OUVERTE = 0;      // Position ouverte (libère)
const int BARRIERE_TEMPS_OUVERTURE = 1000;  // Temps pour ouvrir (ms)
const int BARRIERE_TEMPS_FERMETURE = 800;   // Temps pour fermer (ms)
```

## 🔧 Comment ajuster selon ton montage

### Étape 1: Tester les angles individuellement

Ajoute ce code dans `setup()` pour tester:

```cpp
void setup() {
    // ... code existant ...
    
    // TEST BRAS
    Serial.println("Test BRAS - Position repos");
    bras.write(BRAS_REPOS);
    delay(2000);
    
    Serial.println("Test BRAS - Position pousse");
    bras.write(BRAS_POUSSE);
    delay(2000);
    
    bras.write(BRAS_REPOS);
    
    // TEST BARRIÈRE
    Serial.println("Test BARRIERE - Position fermee");
    bloqueur.write(BARRIERE_FERMEE);
    delay(2000);
    
    Serial.println("Test BARRIERE - Position ouverte");
    bloqueur.write(BARRIERE_OUVERTE);
    delay(2000);
    
    bloqueur.write(BARRIERE_FERMEE);
}
```

### Étape 2: Ajuster les angles

Les servos peuvent tourner de **0° à 180°**. Ajuste selon ton montage:

#### Si le BRAS ne pousse pas assez loin:
- Augmente `BRAS_POUSSE` (ex: 180 → 170 ou 160)
- Ou diminue si ça pousse trop (ex: 180 → 150)

#### Si la BARRIÈRE ne s'ouvre pas assez:
- Change `BARRIERE_OUVERTE` (ex: 0 → 45 ou 90)
- Change `BARRIERE_FERMEE` (ex: 90 → 120 ou 180)

### Étape 3: Ajuster les délais

Si les mouvements sont trop rapides/lents:

```cpp
// Augmente si le servo n'a pas le temps de finir son mouvement
const int BRAS_TEMPS_POUSSE = 2000;  // était 1500

// Diminue si tu veux aller plus vite
const int BARRIERE_TEMPS_OUVERTURE = 500;  // était 1000
```

## 📊 Séquence d'expédition

```
1. BARRIÈRE s'ouvre (0°)     → 1000ms
2. BRAS pousse (180°)        → 1500ms
3. BRAS revient (90°)        → 1000ms
4. BARRIÈRE se ferme (90°)   → 800ms
   ────────────────────────────────────
   TOTAL: ~4.3 secondes
```

## 🐛 Debug

Le code envoie des messages sur le port série:

```
DEBUG: Debut expedition
DEBUG: Ouverture barriere
DEBUG: Bras pousse
DEBUG: Bras revient
DEBUG: Fermeture barriere
DEBUG: Expedition terminee
```

Ouvre le **Moniteur Série** (9600 baud) pour voir ces messages.

## ⚡ Alimentation

**IMPORTANT:** Les servos consomment beaucoup de courant!

- ❌ Ne pas alimenter par USB uniquement
- ✅ Utiliser une alimentation externe 5V (2A minimum)
- ✅ Connecter GND commun (Arduino + Alimentation externe)

## 🔌 Branchements

```
BRAS (Servo 1):
  - Signal → Pin 9
  - VCC    → 5V externe
  - GND    → GND commun

BARRIÈRE (Servo 2):
  - Signal → Pin 10
  - VCC    → 5V externe
  - GND    → GND commun
```

## 📝 Notes

- Les angles peuvent être inversés selon le sens de montage du servo
- Si un servo tourne dans le mauvais sens, inverse les angles (0↔180)
- Teste toujours sans colis d'abord pour éviter les accidents!
