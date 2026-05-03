# 🔧 Diagnostic : Keypad 4x4 ne fonctionne pas

## 🎯 Problème
Le keypad (clavier matriciel 4x4) ne répond pas aux pressions de touches.

---

## 📋 Checklist de diagnostic

### ✅ Étape 1 : Vérifier la bibliothèque Keypad

1. Ouvrir **Arduino IDE**
2. Aller dans **Croquis → Inclure une bibliothèque → Gérer les bibliothèques**
3. Rechercher **"Keypad"** par Mark Stanley
4. Si elle n'est pas installée, cliquer sur **Installer**

### ✅ Étape 2 : Vérifier le câblage

Le keypad 4x4 a **8 pins** :
- **4 pins pour les lignes (ROWS)** : R1, R2, R3, R4
- **4 pins pour les colonnes (COLS)** : C1, C2, C3, C4

**Câblage attendu selon le sketch :**

| Pin Keypad | Pin Arduino | Type |
|------------|-------------|------|
| R1 | A0 | Ligne 1 |
| R2 | A1 | Ligne 2 |
| R3 | A2 | Ligne 3 |
| R4 | A3 | Ligne 4 |
| C1 | A4 | Colonne 1 |
| C2 | A5 | Colonne 2 |
| C3 | 2 | Colonne 3 |
| C4 | 3 | Colonne 4 |

**⚠️ IMPORTANT :**
- Vérifiez que les câbles sont bien enfoncés
- Vérifiez qu'il n'y a pas de faux contact
- Vérifiez que les pins correspondent exactement

### ✅ Étape 3 : Test isolé du keypad

1. **Téléverser le sketch de test** :
   - Ouvrir `arduino_sketch/test_keypad/test_keypad.ino`
   - Téléverser sur la carte LIVRAISON
   
2. **Ouvrir le Moniteur Série** (9600 bauds)

3. **Appuyer sur les touches du keypad**

**Résultats attendus :**
```
=== TEST KEYPAD 4x4 ===
Appuyez sur une touche...

Configuration:
  Rows: A0, A1, A2, A3
  Cols: A4, A5, 2, 3

Touche appuyee : 1
  Code ASCII : 49

Touche appuyee : 5
  Code ASCII : 53
```

**Si rien ne s'affiche** → Problème de câblage ou keypad défectueux

### ✅ Étape 4 : Identifier le type de keypad

Il existe différents types de keypad 4x4 avec des dispositions de pins différentes :

**Type A (le plus courant) :**
```
Pin 1 = R1, Pin 2 = R2, Pin 3 = R3, Pin 4 = R4
Pin 5 = C1, Pin 6 = C2, Pin 7 = C3, Pin 8 = C4
```

**Type B (inversé) :**
```
Pin 1 = C1, Pin 2 = C2, Pin 3 = C3, Pin 4 = C4
Pin 5 = R1, Pin 6 = R2, Pin 7 = R3, Pin 8 = R4
```

**Solution** : Si le Type A ne fonctionne pas, essayez le Type B en inversant les pins dans le code.

---

## 🔧 Solutions selon le diagnostic

### Solution 1 : Bibliothèque manquante

Si la bibliothèque Keypad n'est pas installée :

1. Arduino IDE → **Croquis → Inclure une bibliothèque → Gérer les bibliothèques**
2. Rechercher **"Keypad"**
3. Installer **"Keypad" by Mark Stanley, Alexander Brevig**
4. Redémarrer Arduino IDE
5. Téléverser à nouveau le sketch

### Solution 2 : Câblage incorrect

Si le test isolé ne fonctionne pas, vérifiez le câblage :

**Méthode de test manuel :**

1. Téléverser ce sketch de test des pins :

```cpp
void setup() {
    Serial.begin(9600);
    pinMode(A0, INPUT_PULLUP);
    pinMode(A1, INPUT_PULLUP);
    pinMode(A2, INPUT_PULLUP);
    pinMode(A3, INPUT_PULLUP);
    pinMode(A4, OUTPUT);
    pinMode(A5, OUTPUT);
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
}

void loop() {
    // Activer colonne 1
    digitalWrite(A4, LOW);
    digitalWrite(A5, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    
    Serial.print("Col1: ");
    Serial.print(digitalRead(A0)); Serial.print(" ");
    Serial.print(digitalRead(A1)); Serial.print(" ");
    Serial.print(digitalRead(A2)); Serial.print(" ");
    Serial.println(digitalRead(A3));
    
    delay(500);
}
```

2. Appuyer sur les touches de la colonne 1 (1, 4, 7, *)
3. Vous devriez voir des 0 apparaître quand vous appuyez

### Solution 3 : Inverser rows et cols

Si votre keypad a les pins inversées, modifiez le sketch :

```cpp
// AVANT
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 2, 3};

// APRÈS (inversé)
byte rowPins[ROWS] = {A4, A5, 2, 3};
byte colPins[COLS] = {A0, A1, A2, A3};
```

### Solution 4 : Changer les pins

Si les pins A0-A5 ne fonctionnent pas, utilisez des pins digitales :

```cpp
// Configuration alternative
byte rowPins[ROWS] = {5, 6, 7, 8};   // Nouvelles pins pour rows
byte colPins[COLS] = {A0, A1, A2, A3}; // Nouvelles pins pour cols
```

**⚠️ ATTENTION** : Si vous changez les pins, vérifiez qu'elles ne sont pas utilisées par le LCD ou les servos !

### Solution 5 : Keypad défectueux

Si rien ne fonctionne après tous les tests :

1. **Tester avec un multimètre** :
   - Mettre en mode continuité
   - Appuyer sur une touche
   - Vérifier qu'il y a continuité entre la ligne et la colonne correspondantes

2. **Tester avec un autre keypad** si disponible

3. **Vérifier les soudures** si c'est un keypad DIY

---

## 📊 Tableau de dépannage rapide

| Symptôme | Cause probable | Solution |
|----------|----------------|----------|
| Aucune touche ne répond | Bibliothèque manquante | Installer la bibliothèque Keypad |
| Aucune touche ne répond | Câblage incorrect | Vérifier le câblage avec le test isolé |
| Certaines touches fonctionnent | Faux contact | Vérifier les connexions |
| Mauvaises touches détectées | Pins inversées | Inverser rowPins et colPins |
| Touches aléatoires | Interférences | Ajouter des résistances pull-up |

---

## 🔌 Schéma de câblage recommandé

```
Keypad 4x4          Arduino Uno
┌─────────┐         ┌─────────┐
│ R1 ●────┼────────→│ A0      │
│ R2 ●────┼────────→│ A1      │
│ R3 ●────┼────────→│ A2      │
│ R4 ●────┼────────→│ A3      │
│ C1 ●────┼────────→│ A4      │
│ C2 ●────┼────────→│ A5      │
│ C3 ●────┼────────→│ D2      │
│ C4 ●────┼────────→│ D3      │
└─────────┘         └─────────┘
```

---

## 🧪 Sketch de test complet avec debug

Si le test simple ne fonctionne pas, utilisez ce sketch avec plus de debug :

```cpp
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
    {'1','2','A','3'},
    {'4','5','B','6'},
    {'7','8','C','9'},
    {'*','0','D','#'}
};

byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 2, 3};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
    Serial.begin(9600);
    Serial.println("=== TEST KEYPAD AVEC DEBUG ===");
    
    // Test des pins
    Serial.println("Test des pins en INPUT_PULLUP:");
    for (int i = 0; i < ROWS; i++) {
        pinMode(rowPins[i], INPUT_PULLUP);
        Serial.print("  Row "); Serial.print(i);
        Serial.print(" (pin "); Serial.print(rowPins[i]);
        Serial.print(") = "); Serial.println(digitalRead(rowPins[i]));
    }
    
    Serial.println("Test des pins en OUTPUT:");
    for (int i = 0; i < COLS; i++) {
        pinMode(colPins[i], OUTPUT);
        digitalWrite(colPins[i], HIGH);
        Serial.print("  Col "); Serial.print(i);
        Serial.print(" (pin "); Serial.print(colPins[i]);
        Serial.println(") = HIGH");
    }
    
    Serial.println("\nAppuyez sur une touche...\n");
}

unsigned long lastCheck = 0;

void loop() {
    char key = keypad.getKey();
    
    if (key) {
        Serial.print("✅ Touche detectee : ");
        Serial.print(key);
        Serial.print(" (ASCII: ");
        Serial.print((int)key);
        Serial.println(")");
    }
    
    // Afficher l'état toutes les 2 secondes
    if (millis() - lastCheck > 2000) {
        lastCheck = millis();
        Serial.println("⏱ Keypad en attente...");
    }
}
```

---

## 📞 Support

Si le problème persiste après tous ces tests :

1. Vérifier que le keypad est bien un modèle 4x4 (et pas 3x4)
2. Vérifier la tension d'alimentation (5V)
3. Essayer avec un autre Arduino pour isoler le problème
4. Consulter la datasheet de votre keypad spécifique

**Fichiers de référence :**
- `arduino_sketch/test_keypad/test_keypad.ino` - Test isolé du keypad
- `arduino_sketch/arduino_sketch.ino` - Sketch complet avec LCD + servos
