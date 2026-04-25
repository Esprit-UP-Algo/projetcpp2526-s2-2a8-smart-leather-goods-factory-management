# Modifications Arduino & Expédier — Documentation complète

---

## 1. Fichiers modifiés

| Fichier | Nature des modifications |
|---------|--------------------------|
| `arduino_sketch/arduino_sketch.ino` | LCD 16x2, keypad 4x4, servos, protocole série complet |
| `arduinoconnection.h` | Suppression VID/PID hardcodés |
| `arduinoconnection.cpp` | Détection multi-cartes (7 VID/PID), logs debug |
| `mainwindow.h` | Nouveaux membres Arduino : indicateur, buffer, labels LCD |
| `mainwindow.cpp` | 4 nouvelles fonctions Arduino, bouton Expédier, indicateur |
| `mainwindow.ui` | Bouton Expédier redesigné, suppression btnSaisir/btnExpedierAction |

---

## 2. arduino_sketch/arduino_sketch.ino — Version finale

### Bibliothèques
```cpp
#include <Servo.h>
#include <Keypad.h>
#include <LiquidCrystal.h>
```

### Brochage LCD 16x2 (mode 4 bits, sans I2C)
```cpp
LiquidCrystal lcd(12, 11, 6, 7, 8, 13);
// RS=12, EN=11, D4=6, D5=7, D6=8, D7=13
```
> Résistance 220Ω obligatoire entre 5V et Pin 15 (A) du LCD

### Servos
```cpp
Servo bras;      // broche 9  — pousse le colis
Servo bloqueur;  // broche 10 — bloque/libère le passage
```

### Keypad 4x4
```cpp
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, 2, 3};
```

### Protocole reçu depuis Qt
| Caractère reçu | Action Arduino |
|----------------|----------------|
| `'1'` | Affiche "Expedition OK", lance `expedierColis()`, retour accueil |
| `'0'` | Affiche "!! ERREUR !!" 2s, retour accueil |
| `'2'` | Affiche "ID invalide / Ressaisir + #", **reste en attente** (pas de retour accueil) |

### Protocole envoyé vers Qt
| Message envoyé | Déclencheur |
|----------------|-------------|
| `"INPUT:xxx"` | Chaque touche saisie (sauf `#` et `*`) |
| `"ID:xxx"` | Touche `#` avec buffer non vide |
| `"CLEAR"` | Touche `*` ou après envoi de `#` |
| `'1'` (char) | Fin de `expedierColis()` — confirmation servos |

### Séquence expedierColis()
```
1. bloqueur.write(180)  → s'ouvre        [800ms]
2. bras.write(0)        → pousse colis   [1000ms]
3. bras.write(90)       → revient        [800ms]
4. bloqueur.write(90)   → se referme     [500ms]
5. Serial.write('1')    → confirmation Qt
```

---

## 3. arduinoconnection.h — Modifications

**Supprimé :**
```cpp
// Ces deux constantes ont été supprimées
static const quint16 arduino_uno_vendor_id  = 9025;
static const quint16 arduino_uno_product_id = 67;
```

**Résultat :** La classe ne dépend plus d'un seul VID/PID hardcodé.

---

## 4. arduinoconnection.cpp — Détection multi-cartes

**Avant :** Détectait uniquement Arduino Uno officiel (VID=9025, PID=67).

**Après :** Détecte 7 types de cartes :
```cpp
static const QList<QPair<quint16,quint16>> knownIds = {
    {0x2341, 0x0043},  // Arduino Uno officiel
    {0x2341, 0x0001},  // Arduino Uno officiel (ancien)
    {0x1A86, 0x7523},  // Clone CH340
    {0x1A86, 0x5523},  // Clone CH340G
    {0x10C4, 0xEA60},  // Clone CP2102
    {0x0403, 0x6001},  // FTDI FT232RL
    {0x0403, 0x6015},  // FTDI FT231X
};
```

**Paramètres série :** 9600 baud, 8 bits, pas de parité, 1 stop bit, pas de flow control.

**Retours de connect_arduino() :**
- `0` → connecté et port ouvert
- `1` → port trouvé mais impossible à ouvrir
- `-1` → aucun Arduino détecté

**Logs debug ajoutés :** Affiche tous les ports disponibles avec VID/PID dans la console Qt.

---

## 5. mainwindow.h — Nouveaux membres Arduino

```cpp
// Arduino — membres ajoutés
Arduino A;                          // instance unique (existait déjà)
QByteArray arduinoData;             // buffer réception (existait déjà)
QLabel *m_arduinoIndicator = nullptr; // indicateur connexion (NOUVEAU)
QString m_keypadBuffer;             // buffer saisie simulateur (NOUVEAU)
QLabel *m_lcdLigne1 = nullptr;      // miroir LCD ligne 1 (NOUVEAU)
QLabel *m_lcdLigne2 = nullptr;      // miroir LCD ligne 2 (NOUVEAU)
```

**Slots Arduino ajoutés :**
```cpp
void expedierActionArduino();           // OUTPUT → envoyer '1' à l'Arduino
void recevoir_donnee();                 // INPUT  ← lit le port série
void setupKeypadSimulator();            // crée le keypad simulateur Qt
void traiterMessageArduino(const QString &msg); // logique partagée
```

**Slots supprimés :**
```cpp
// Supprimé — plus de toggle
void on_btnExpedier_clicked();
```

---

## 6. mainwindow.cpp — Modifications détaillées

### 6.1 Constructeur — Connexion Arduino

**Avant :** Connexion simple, indicateur absent.

**Après :**
```cpp
int ret = A.connection()->connect_arduino();
// Indicateur visuel selon résultat
if (ret == 0) {
    m_arduinoIndicator->setText("● Arduino connecté");
    // vert #27AE60
    ui->btnExpedier->setText("🚚 Expédier");
} else {
    m_arduinoIndicator->setText("● Non connecté");
    // rouge #E74C3C
    ui->btnExpedier->setText("🔌 Brancher Arduino");
}
```

### 6.2 Indicateur de connexion

- `QLabel` arrondi, taille fixe 160x24px
- Positionné en overlay coin supérieur droit
- **Visible uniquement sur la page Production (index 4)**
- Se repositionne automatiquement au `resizeEvent`

### 6.3 Bouton Expédier — UI

**Avant :** 3 boutons (btnExpedier toggle + btnSaisir + btnExpedierAction)

**Après :** 1 seul bouton `btnExpedier`, taille 120x32, style marron identique aux autres boutons production.

### 6.4 expedierActionArduino() — 4 cas

```
CAS 1 : Arduino non connecté
  → Tentative reconnexion automatique
  → Si échec : QMessageBox::warning "Impossible d'expédier. Vérifiez la connexion série."

CAS 2 : Aucune ligne sélectionnée
  → QMessageBox::warning "Veuillez sélectionner une commande."

CAS 3 : Statut != "Terminé"
  → QMessageBox::critical avec Référence + Statut actuel
  → "La commande doit être Terminé."

CAS 4 : Statut == "Terminé"
  → A.write_to_arduino("1")
  → UPDATE COMMANDES SET STATUT='En livraison',
    DATE_LIVRAISON=TO_DATE(date_du_jour, 'DD/MM/YYYY')
  → loadProductionData()
  → QMessageBox::information avec Référence, Client, Statut, Date
```

### 6.5 recevoir_donnee() — Séparation des chemins

**Avant :** Lisait toujours `A.read_from_arduino()` même depuis le simulateur.

**Après :**
```cpp
void MainWindow::recevoir_donnee()
{
    arduinoData = A.read_from_arduino();  // seulement pour le vrai Arduino
    traiterMessageArduino(QString(arduinoData).trimmed());
}
```

Le simulateur appelle `traiterMessageArduino()` directement — pas de lecture port série.

### 6.6 traiterMessageArduino() — Logique partagée

Fonction centrale appelée par le vrai Arduino ET le simulateur :

```
"INPUT:xxx" → met à jour lcdLigne2 (saisie en cours)

"CLEAR"     → remet lcdLigne1 = "Systeme pret"
              remet lcdLigne2 = "Saisir ID + #"
              vide m_keypadBuffer

"1"         → ignoré (confirmation servos déjà traitée)

"ID:xxx"    → cherche REFERENCE ou ID_COMMANDE en BDD
              ├─ Trouvé  → UPDATE STATUT='Terminé'
              │            loadProductionData()
              │            Serial.write('1') → servos
              │            LCD: "Commande terminee / [id]"
              └─ Invalide → Serial.write('2')
                            LCD: "ID invalide / Ressaisir + #"
                            (pas de retour accueil)
```

**Requête SQL :**
```sql
SELECT ID_COMMANDE FROM COMMANDES
WHERE REFERENCE = :id OR TO_CHAR(ID_COMMANDE) = :id2
```
Accepte la REFERENCE (ex: `CMD-2026-001`) **ou** l'ID numérique (ex: `42`).

**UPDATE :**
```sql
UPDATE COMMANDES SET STATUT = 'Terminé'
WHERE REFERENCE = :id OR TO_CHAR(ID_COMMANDE) = :id2
```

### 6.7 setupKeypadSimulator() — Simulateur Qt

Créé entièrement en code (pas dans le UI). Inséré dans le layout de la page Production juste avant `productionTable`.

**Structure :**
```
QGroupBox "Simulateur Keypad Arduino"
  ├── QFrame (miroir LCD, fond noir, texte vert Courier New)
  │     ├── m_lcdLigne1 (bold)
  │     └── m_lcdLigne2
  └── QGridLayout 4x4
        ├── [1][2][3][A]
        ├── [4][5][6][B]
        ├── [7][8][9][C]
        └── [*][0][#][D]
```

**Guard anti-doublon :**
```cpp
if (m_lcdLigne1 != nullptr) return;
```

**Logique touches :**
```cpp
'#' → traiterMessageArduino("ID:" + m_keypadBuffer)  // confirmer
'*' → traiterMessageArduino("CLEAR")                  // effacer
autres → traiterMessageArduino("INPUT:" + buffer)     // saisie
```

### 6.8 switchPage() — Visibilité indicateur

```cpp
// Indicateur Arduino visible uniquement sur la page Production (index 4)
if (m_arduinoIndicator) m_arduinoIndicator->setVisible(index == 4);
```

---

## 7. mainwindow.ui — Modifications

### Bouton Expédier
**Avant :** 3 widgets (btnExpedier 48x32 + btnSaisir + btnExpedierAction)

**Après :** 1 seul widget :
```xml
<widget class="QPushButton" name="btnExpedier">
  <property name="minimumSize"><size><width>120</width><height>32</height></size></property>
  <property name="text"><string>🚚 Expédier</string></property>
</widget>
```
Style hérité du stylesheet global production (marron `#8D6E63`).

---


---

## 10. Flux complet final

### Via Keypad physique Arduino
```
Saisie touches → "INPUT:xxx" → Qt met à jour lcdLigne2
Touche '*'     → "CLEAR"     → Qt remet LCD à l'accueil
Touche '#'     → "ID:xxx"    → Qt cherche en BDD
                    ├─ Trouvé  → STATUT='Terminé' + Serial.write('1')
                    │            Arduino : "Expedition OK" + servos
                    └─ Invalide → Serial.write('2')
                                  Arduino : "ID invalide / Ressaisir + #"
```

### Via Simulateur Qt
```
Clic bouton → traiterMessageArduino(msg) directement
Même logique BDD, même mise à jour LCD miroir
Pas de communication port série
```

### Via Bouton Expédier Qt
```
Clic "Expédier"
  CAS 1 : pas connecté → reconnexion auto ou warning
  CAS 2 : rien sélectionné → warning
  CAS 3 : statut != Terminé → critical avec détails
  CAS 4 : statut == Terminé → write('1') + UPDATE BDD + popup info
```

---

## 11. Résumé des membres privés Arduino dans MainWindow

| Membre | Type | Rôle |
|--------|------|------|
| `A` | `Arduino` | Instance unique, accès au port série |
| `arduinoData` | `QByteArray` | Buffer de réception port série |
| `m_arduinoIndicator` | `QLabel*` | Indicateur vert/rouge connexion |
| `m_keypadBuffer` | `QString` | Buffer saisie simulateur |
| `m_lcdLigne1` | `QLabel*` | Miroir LCD ligne 1 |
| `m_lcdLigne2` | `QLabel*` | Miroir LCD ligne 2 |
