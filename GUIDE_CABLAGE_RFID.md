# Guide de Cablage - Systeme Pointage RFID CUIREA

## Materiel Necessaire

### Composants
- 1x Arduino Uno
- 1x Module RFID RC522 + cartes RFID
- 1x Ecran LCD 16x2 avec module I2C
- 1x Servomoteur SG90
- 1x LED verte 5mm + resistance 220 ohms
- 1x LED rouge 5mm + resistance 220 ohms
- 1x Buzzer actif 5V
- 1x Breadboard
- Fils de connexion

### Bibliotheques Arduino a installer
1. MFRC522 (par GithubCommunity)
2. LiquidCrystal_I2C (par Frank de Brabander)

---

## Schema de Cablage

### Module RFID RC522 (ATTENTION: 3.3V seulement!)
```
RC522          Arduino
─────          ───────
SDA    ───────  Pin 10
SCK    ───────  Pin 13
MOSI   ───────  Pin 11
MISO   ───────  Pin 12
RST    ───────  Pin 8
3.3V   ───────  3.3V  (PAS 5V!)
GND    ───────  GND
```

### Ecran LCD I2C
```
LCD I2C        Arduino
───────        ───────
SDA    ───────  A4
SCL    ───────  A5
VCC    ───────  5V
GND    ───────  GND
```

### Servomoteur SG90
```
Servo          Arduino
─────          ───────
Signal (orange) ─── Pin 9
VCC (rouge)    ─── 5V
GND (marron)   ─── GND
```

### LED Verte
```
Pin 7 ──[220Ω]──|>|── GND
                LED
```

### LED Rouge
```
Pin 6 ──[220Ω]──|>|── GND
                LED
```

### Buzzer
```
Pin 5 ────── (+) Buzzer
GND   ────── (-) Buzzer
```

---

## Etapes d'Installation

### 1. Preparer l'Arduino IDE
- Ouvrir Arduino IDE
- Aller dans Outils > Gerer les bibliotheques
- Installer "MFRC522" par GithubCommunity
- Installer "LiquidCrystal I2C" par Frank de Brabander

### 2. Telecharger le code
- Ouvrir le fichier `arduino_pointage/arduino_pointage.ino`
- Selectionner la carte "Arduino Uno"
- Selectionner le bon port COM
- Cliquer sur Televerser

### 3. Tester sans Qt
- Ouvrir le Moniteur Serie (9600 bauds)
- Presenter une carte RFID
- Vous devez voir "UID:XXXXXXXX" s'afficher
- Taper "GRANTED:Ali:E" et appuyer Entree
- Le LCD doit afficher "Bienvenue Ali", LED verte, servo ouvre

### 4. Tester avec Qt
- Lancer l'application CUIREA
- S'assurer que l'Arduino est connecte
- Presenter une carte RFID enregistree dans la base
- Le systeme complet doit fonctionner

---

## Depannage

### Le LCD n'affiche rien
- Verifier l'adresse I2C (peut etre 0x3F au lieu de 0x27)
- Ajuster le potentiometre de contraste a l'arriere du module I2C

### Le RFID ne detecte pas les cartes
- Verifier que le RC522 est alimente en 3.3V (pas 5V!)
- Verifier les connexions SPI

### Le servo ne bouge pas
- Verifier l'alimentation 5V
- Tester avec un autre servo

### Qt ne recoit pas les donnees
- Verifier le port COM dans l'application
- Verifier que le Moniteur Serie Arduino est ferme

---

## Commandes de Test (Moniteur Serie)

Taper ces commandes pour tester la partie OUTPUT :

```
GRANTED:Ali:E      -> Entree autorisee pour Ali
GRANTED:Sonia:S    -> Sortie autorisee pour Sonia
DENIED             -> Acces refuse
```

---

## Fichiers du Projet

| Fichier | Description |
|---------|-------------|
| `arduino_pointage/arduino_pointage.ino` | Code Arduino complet (INPUT + OUTPUT) |
| `pointage.cpp` | Code Qt pour verification base de donnees |
| `mainwindow.cpp` | Integration dans l'application |
| `script_pointage_rfid.sql` | Script SQL pour la base Oracle |

---

Bonne chance pour demain!
