# Scénario Arduino — Système d'Expédition CUIREA

## Vue d'ensemble

Le système Arduino gère l'expédition physique des commandes dans l'usine CUIREA.
Quand une commande est validée, l'Arduino actionne deux servos pour pousser le colis.
La communication se fait via port série USB (9600 baud) entre Qt et l'Arduino Uno.

---

## Matériel utilisé

| Composant | Broche | Rôle |
|-----------|--------|------|
| LCD 16x2 (mode 4 bits, sans I2C) | RS=12, EN=11, D4=6, D5=7, D6=8, D7=13 | Affichage messages |
| Servo `bras` | 9 | Pousse le colis |
| Servo `bloqueur` | 10 | Bloque / libère le passage |
| Keypad 4x4 | Lignes: A0,A1,A2,A3 — Colonnes: A4,A5,2,3 | Saisie ID commande |
| LED erreur | 8 | Signalement erreur visuelle |

### Câblage LCD (résistance obligatoire)
```
5V ──[220Ω]── Pin 15 (A = Anode rétroéclairage)
GND ────────── Pin 16 (K = Cathode)
RW ──────────── GND (lecture seule)
```

---

## Protocole de communication série

### Arduino → Qt

| Message | Déclencheur | Signification |
|---------|-------------|---------------|
| `INPUT:xxx` | Chaque touche saisie | Saisie en cours, afficher sur miroir LCD Qt |
| `ID:xxx` | Touche `#` confirmée | ID saisi complet, chercher en BDD |
| `CLEAR` | Touche `*` ou après envoi `#` | Effacer le buffer de saisie |
| `'1'` (char) | Fin de `expedierColis()` | Confirmation que les servos ont terminé |

### Qt → Arduino

| Caractère | Signification | Réaction Arduino |
|-----------|---------------|------------------|
| `'1'` | Commande trouvée, expédier | Lance `expedierColis()`, affiche "Commande terminee" |
| `'0'` | Erreur générale | Affiche "!! ERREUR !!" 2 secondes, retour accueil |
| `'2'` | ID invalide en BDD | Affiche "ID invalide / Ressaisir + #", reste en attente |

---

## Séquence servo — `expedierColis()`

```
1. bloqueur.write(180)  → bloqueur s'ouvre        [800ms]
2. bras.write(0)        → bras pousse le colis     [1000ms]
3. bras.write(90)       → bras revient en position [800ms]
4. bloqueur.write(90)   → bloqueur se referme      [500ms]
5. Serial.write('1')    → confirmation à Qt
```

---

## Flux complet — Phase 1 : Saisie via Keypad Arduino

```
Opérateur saisit chiffres/lettres sur le keypad physique
    │
    ├─ Chaque touche → Arduino envoie "INPUT:xxx"
    │                → Qt met à jour lcdLigne2 (miroir LCD)
    │
    ├─ Touche '*'   → Arduino envoie "CLEAR"
    │                → Qt remet "Systeme pret / Saisir ID + #"
    │
    └─ Touche '#'   → Arduino envoie "ID:xxx" puis "CLEAR"
                     → Qt cherche REFERENCE ou ID_COMMANDE en BDD
                          │
                          ├─ Trouvé → Qt envoie '1'
                          │          → UPDATE COMMANDES SET STATUT='Terminé'
                          │          → loadProductionData()
                          │          → Arduino reçoit '1'
                          │          → LCD: "Commande terminee / [id]"
                          │          → expedierColis() (servos)
                          │
                          └─ Invalide → Qt envoie '2'
                                       → Arduino reçoit '2'
                                       → LCD: "ID invalide / Ressaisir + #"
                                       → Reste en attente (pas de retour accueil)
                                       → Opérateur ressaisit un nouvel ID
```

---

## Flux complet — Phase 2 : Bouton Expédier Qt

```
Clic sur bouton "Expédier" dans Gestion de la Production
    │
    ├─ CAS 1 : Arduino non connecté
    │   → Tentative de reconnexion automatique
    │   → Si échec : QMessageBox::warning "Impossible d'expédier. Vérifiez la connexion série."
    │
    ├─ CAS 2 : Aucune ligne sélectionnée dans productionTable
    │   → QMessageBox::warning "Veuillez sélectionner une commande."
    │
    ├─ CAS 3 : Ligne sélectionnée mais STATUT != "Terminé"
    │   → QMessageBox::critical "Expédition impossible"
    │     Affiche : Référence, Statut actuel, "La commande doit être Terminé."
    │
    └─ CAS 4 : Ligne sélectionnée et STATUT == "Terminé"
        → A.write_to_arduino("1")
        → UPDATE COMMANDES SET STATUT='En livraison',
          DATE_LIVRAISON=TO_DATE(date_du_jour, 'DD/MM/YYYY')
        → loadProductionData()
        → QMessageBox::information avec Référence, Client, Statut, Date livraison
```

---

## Détection Arduino — `ArduinoConnection::connect_arduino()`

Détecte automatiquement tous les types de cartes Arduino par VID/PID :

| VID | PID | Type |
|-----|-----|------|
| 0x2341 | 0x0043 | Arduino Uno officiel |
| 0x2341 | 0x0001 | Arduino Uno officiel (ancien) |
| 0x1A86 | 0x7523 | Clone CH340 |
| 0x1A86 | 0x5523 | Clone CH340G |
| 0x10C4 | 0xEA60 | Clone CP2102 |
| 0x0403 | 0x6001 | FTDI FT232RL |
| 0x0403 | 0x6015 | FTDI FT231X |

Paramètres série : **9600 baud, 8 bits, pas de parité, 1 stop bit, pas de flow control**


Retours de `connect_arduino()` :
- `0` → connecté et port ouvert
- `1` → port trouvé mais impossible à ouvrir
- `-1` → aucun Arduino détecté

---

## Indicateur de connexion Qt

Un `QLabel` arrondi affiché en haut à droite de la fenêtre, **visible uniquement sur la page Production** :

- Vert : "● Arduino connecté"
- Rouge : "● Non connecté"

Si l'Arduino est branché après le démarrage de l'appli, un clic sur le bouton "Expédier" tente une reconnexion automatique et met à jour l'indicateur.

---

## Simulateur Keypad Qt

Un `QGroupBox` créé entièrement en code (pas dans le UI) inséré dans la page Production, juste au-dessus du tableau des commandes. Il contient :

- Un **miroir LCD** (fond noir, texte vert `Courier New`) avec deux lignes :
  - `lcdLigne1` : état général ("Systeme pret", "Recherche...", "Expedie !", "ID introuvable")
  - `lcdLigne2` : saisie en cours ou référence traitée
- Une **grille 4x4** de boutons reproduisant le keypad physique

### Logique du simulateur

Au lieu d'envoyer sur le port série, les boutons appellent directement `traiterMessageArduino(msg)` :

| Touche | Message injecté | Effet |
|--------|-----------------|-------|
| Chiffre/Lettre | `"INPUT:xxx"` | Met à jour lcdLigne2 |
| `#` | `"ID:xxx"` | Cherche en BDD, met à jour statut + date |
| `*` | `"CLEAR"` | Remet LCD à l'état accueil |

La fonction `traiterMessageArduino(const QString &msg)` est partagée entre le vrai Arduino et le simulateur — aucune duplication de logique.

---

## Mise à jour BDD lors d'une expédition

```sql
UPDATE COMMANDES
SET STATUT = 'En livraison',
    DATE_LIVRAISON = TO_DATE(:dl, 'DD/MM/YYYY')
WHERE REFERENCE = :id OR TO_CHAR(ID_COMMANDE) = :id2
```

- La recherche accepte la **REFERENCE** (ex: `CMD-2026-001`) ou l'**ID_COMMANDE** numérique (ex: `42`)
- La date de livraison est mise à jour avec `QDate::currentDate().toString("dd/MM/yyyy")`
- `TO_DATE(..., 'DD/MM/YYYY')` est obligatoire pour Oracle

---

## Fichiers concernés

| Fichier | Rôle |
|---------|------|
| `arduino_sketch/arduino_sketch.ino` | Sketch Arduino : LCD, keypad, servos, protocole série |
| `arduinoconnection.h/.cpp` | Détection port série, connexion/déconnexion, VID/PID |
| `arduino.h/.cpp` | `write_to_arduino()`, `read_from_arduino()`, accès à `ArduinoConnection` |
| `mainwindow.h` | Déclaration `Arduino A`, `arduinoData`, `m_lcdLigne1/2`, `m_keypadBuffer`, slots |
| `mainwindow.cpp` | Init connexion, indicateur, simulateur keypad, `expedierActionArduino()`, `recevoir_donnee()`, `traiterMessageArduino()` |
