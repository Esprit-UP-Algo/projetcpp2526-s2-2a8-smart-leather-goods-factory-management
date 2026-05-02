# Support Multi-Cartes Arduino — Documentation des modifications

**Date** : 2026-05-02  
**Objectif** : Faire fonctionner 3 cartes Arduino simultanément avec identification automatique par message (Option A)

---

## Architecture finale

```
App Qt démarre
    │
    ▼
ArduinoManager::scanAndIdentify()
    │
    ├─ Scan USB → [COM5, COM6, COM7]
    │
    ├─ COM5 → ouvre → attend "ROLE:xxx" → "ROLE:TEMP_BALANCE"
    │         → m_arduinoTempBalance (DHT11 x2 + HX711 + LED RGB)
    │
    ├─ COM6 → ouvre → attend "ROLE:xxx" → "ROLE:LIVRAISON"
    │         → m_arduinoLivraison (keypad + servos + LCD 4 bits)
    │
    └─ COM7 → ouvre → attend "ROLE:xxx" → "ROLE:POINTAGE"
              → m_arduinoPointage (MFRC522 + LCD I2C + servo)
                  │
                  ▼
    onArduinoDiscoveryComplete()
        ├─ m_arduinoTempBalance → ArduinoMonitor (temp + balance)
        ├─ m_arduinoLivraison   → recevoir_donnee() + expedierActionArduino()
        └─ m_arduinoPointage    → recevoir_donnee() (RFID pointage)
```

---

## Fichiers modifiés

### 1. `arduinoconnection.h` / `arduinoconnection.cpp`

**Ajouts :**
- `connectToPort(portName)` — connexion directe à un port nommé (multi-cartes)
- `scanAllPorts()` — retourne la liste de **tous** les ports Arduino détectés (sans s'arrêter au premier)
- `openPort(portName)` — méthode privée partagée pour ouvrir et configurer un port série
- `knownIds()` — méthode statique retournant les 7 VID/PID connus (refactorisé depuis le code inline)

**Compatibilité conservée :**
- `connect_arduino()` — appelle maintenant `scanAllPorts().first()` (comportement identique à avant)

---

### 2. `arduino.h` / `arduino.cpp`

**Ajouts :**
- `connectToPort(portName)` — connexion directe à un port nommé
- `m_role` — rôle de la carte (`"TEMP_BALANCE"` | `"LIVRAISON"` | `"POINTAGE"`)
- `role()` / `setRole()` — accesseurs du rôle
- Log amélioré : affiche le rôle dans les messages debug

**Compatibilité conservée :**
- Toute l'API existante (`connect_arduino`, `write_to_arduino`, etc.) inchangée

---

### 3. `arduinomanager.h` / `arduinomanager.cpp` *(nouveau fichier)*

Classe responsable de la **découverte automatique** des 3 cartes.

**Fonctionnement :**
1. `scanAndIdentify()` — ouvre chaque port Arduino détecté avec un `QSerialPort` temporaire
2. Écoute le message `ROLE:xxx` envoyé par chaque carte au démarrage
3. Ferme le port temporaire, crée une instance `Arduino` définitive, la connecte au même port
4. Émet `discoveryComplete()` quand toutes les cartes sont traitées (ou après timeout de 3s)

**Signaux :**
- `discoveryComplete()` — toutes les cartes identifiées ou timeout
- `cardIdentified(role, port)` — une carte a été reconnue
- `cardMissing(role)` — une carte attendue n'a pas répondu

**Constante :**
- `ROLE_TIMEOUT_MS = 3000` — délai max d'attente du message ROLE: (3 secondes)

---

### 4. `arduinomonitor.h` / `arduinomonitor.cpp`

**Changements :**
- `m_arduino` renommé en `m_arduinoTempBalance` (clarté)
- `setArduino(arduino)` conservé comme alias de `setArduinoTempBalance(arduino)` (compatibilité)
- `setArduinoTempBalance(arduino)` — nouvelle méthode explicite
- `arduinoTempBalance()` — nouvel accesseur
- Tous les appels internes utilisent `m_arduinoTempBalance` au lieu de `m_arduino`
- Commentaires mis à jour pour documenter les 4 scénarios

**Scénarios gérés par ArduinoMonitor :**
- Scénario 1 : Surveillance température (carte TEMP_BALANCE)
- Scénario 2 : Validation livraison par pesée (carte TEMP_BALANCE, balance HX711)

**Scénarios gérés directement par MainWindow :**
- Scénario 3 : Expédition commandes (carte LIVRAISON)
- Scénario 4 : Pointage RFID (carte POINTAGE)

---

### 5. `mainwindow.h`

**Ajouts :**
```cpp
#include "arduinomanager.h"

ArduinoManager *m_arduinoManager       = nullptr;  // découverte automatique
Arduino        *m_arduinoTempBalance   = nullptr;  // ROLE:TEMP_BALANCE
Arduino        *m_arduinoLivraison     = nullptr;  // ROLE:LIVRAISON
Arduino        *m_arduinoPointage      = nullptr;  // ROLE:POINTAGE
QSerialPort    *m_serialLivraison      = nullptr;  // port série carte LIVRAISON
QSerialPort    *m_serialPointage       = nullptr;  // port série carte POINTAGE

void setupArduinoMultiCartes();
void onArduinoDiscoveryComplete();
```

**Suppressions :**
```cpp
Arduino       m_arduino;          // remplacé par 3 instances séparées
QSerialPort  *m_serialArduino;    // remplacé par m_serialLivraison + m_serialPointage
```

---

### 6. `mainwindow.cpp`

**`setupArduinoMultiCartes()` (remplace `setupArduinoPointage()`) :**
- Crée l'indicateur de connexion (overlay)
- Crée le timer absences
- Crée le miroir LCD
- Crée `ArduinoMonitor` et connecte ses signaux
- Crée `ArduinoManager` et lance `scanAndIdentify()`

**`onArduinoDiscoveryComplete()` (nouveau) :**
- Récupère les 3 instances depuis `ArduinoManager`
- Met à jour l'indicateur (vert=3/3, orange=partiel, rouge=0)
- Injecte `m_arduinoTempBalance` dans `ArduinoMonitor`
- Connecte `m_serialLivraison` et `m_serialPointage` à `recevoir_donnee()`

**`recevoir_donnee()` :**
- Lit depuis `m_arduinoLivraison` ET `m_arduinoPointage` (buffers séparés)
- Plus de `m_serialArduino` unique

**`expedierActionArduino()` :**
- Vérifie `m_arduinoLivraison` au lieu de `m_serialArduino`
- Envoie `"1"` via `m_arduinoLivraison->write_to_arduino("1")`

**`traiterMessageArduino()` :**
- Réponses RFID (`OK:`, `BYE:`, `NO`) → envoyées via `m_arduinoPointage`
- Réponses expédition (`"1"`, `"2"`) → envoyées via `m_arduinoLivraison`

**Destructeur :**
- `delete m_arduinoManager` — libère les 3 instances Arduino qu'il a créées

---

### 7. `arduino_sketch/arduino_sketch.ino` (carte LIVRAISON)

**Ajout dans `setup()` :**
```cpp
Serial.println("ROLE:LIVRAISON");  // ligne ajoutée après Serial.begin(9600)
```

---

### 8. `arduino_pointage/arduino_pointage.ino` (carte POINTAGE)

**Ajout dans `setup()` :**
```cpp
Serial.println("ROLE:POINTAGE");  // ligne ajoutée après Serial.begin(9600)
```

---

### 9. `cuirea_arduino/cuirea_arduino.ino` (carte TEMP_BALANCE)

**Ajout dans `setup()` :**
```cpp
Serial.println("ROLE:TEMP_BALANCE");  // ligne ajoutée après delay(1000)
```

---

### 10. `sans_titre.pro`

**Ajouts :**
```
SOURCES += arduinomanager.cpp
HEADERS += arduinomanager.h
```

---

## Protocole de communication par carte

### Carte TEMP_BALANCE
| Direction | Message | Signification |
|-----------|---------|---------------|
| Arduino → Qt | `ROLE:TEMP_BALANCE` | Identification au démarrage |
| Arduino → Qt | `TEMP:25.5,22.3` | Température matière, ambiance |
| Arduino → Qt | `WEIGHT:45.67` | Poids mesuré (kg) |
| Qt → Arduino | `GET_TEMP\n` | Demander lecture température |
| Qt → Arduino | `GET_WEIGHT\n` | Demander lecture poids |
| Qt → Arduino | `TARE\n` | Remettre balance à zéro |
| Qt → Arduino | `LED_RED\n` | LED rouge |
| Qt → Arduino | `LED_GREEN\n` | LED verte |

### Carte LIVRAISON
| Direction | Message | Signification |
|-----------|---------|---------------|
| Arduino → Qt | `ROLE:LIVRAISON` | Identification au démarrage |
| Arduino → Qt | `ID:xxx` | ID commande saisi sur keypad |
| Arduino → Qt | `INPUT:xxx` | Saisie en cours (miroir LCD) |
| Arduino → Qt | `CLEAR` | Effacer buffer |
| Arduino → Qt | `1` | Confirmation servos terminés |
| Qt → Arduino | `1` | Lancer expédition (servos) |
| Qt → Arduino | `2` | ID invalide |
| Qt → Arduino | `0` | Erreur générale |

### Carte POINTAGE
| Direction | Message | Signification |
|-----------|---------|---------------|
| Arduino → Qt | `ROLE:POINTAGE` | Identification au démarrage |
| Arduino → Qt | `UID:AABBCCDD` | Badge RFID scanné |
| Qt → Arduino | `OK:Prenom Nom` | Accès autorisé (entrée) |
| Qt → Arduino | `BYE:Prenom Nom` | Sortie enregistrée |
| Qt → Arduino | `NO` | Badge inconnu, accès refusé |

---

## Indicateur de connexion (overlay)

| État | Texte | Couleur |
|------|-------|---------|
| 3 cartes connectées | `● 3 Arduino connectés` | Vert `#27AE60` |
| 1 ou 2 cartes | `● 2/3 Arduino` | Orange `#F57C00` |
| Aucune carte | `● Non connecté` | Rouge `#E74C3C` |
| Découverte en cours | `● Recherche Arduino...` | Gris `#888` |

---

## Comportement si une carte est absente

- **TEMP_BALANCE absente** : surveillance température désactivée, bouton livraison fournisseur désactivé
- **LIVRAISON absente** : bouton Expédier affiche un warning, keypad physique inopérant
- **POINTAGE absente** : pointage RFID inopérant, le reste de l'app fonctionne normalement

L'application **ne plante pas** si une carte est manquante — chaque fonctionnalité vérifie `if (m_arduinoXxx && m_arduinoXxx->isConnected())` avant d'agir.
