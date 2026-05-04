# Récap — Correction déclenchement servomoteurs de livraison

## Problème constaté

Les servomoteurs de la carte Arduino **LIVRAISON** tournaient automatiquement dès que le statut d'une commande passait à **"Terminé"**, sans attendre que l'opérateur clique sur le bouton **"Expédier"** dans l'interface Qt.

---

## Cause racine

Dans `mainwindow.cpp`, le bloc qui traite la réception du message `ID:xxx` depuis le keypad Arduino faisait deux choses en séquence immédiate :

1. `UPDATE COMMANDES SET STATUT = 'Terminé'` → mise à jour BDD ✓
2. `m_arduinoLivraison->write_to_arduino("1")` → envoi du signal de déclenchement servos ✗

Le caractère `'1'` est le signal que l'Arduino interprète comme **"lancer la séquence d'expédition"** (`expedierColis()`). Il était donc envoyé sans intervention humaine.

---

## Correction appliquée

**Fichier modifié :** `mainwindow.cpp` — bloc de traitement `if (msg.startsWith("ID:"))`

### Avant

```cpp
// Trouvé → marquer Terminé + date livraison + envoyer '1' aux servos
upd.exec();

// Envoyer '1' à la carte LIVRAISON (servos)
if (m_arduinoLivraison && m_arduinoLivraison->isConnected())
    m_arduinoLivraison->write_to_arduino("1");  // ← servos tournaient ici

if (m_lcdLigne1) m_lcdLigne1->setText("Commande terminee");
if (m_lcdLigne2) m_lcdLigne2->setText(refTrouvee);
```

### Après

```cpp
// Trouvé → marquer Terminé + date livraison
upd.exec();

// Envoyer '3' → Arduino affiche "Cliquer Expédier" et attend
// Les servos ne tournent QUE quand l'opérateur clique btnExpédier (qui envoie '1')
if (m_arduinoLivraison && m_arduinoLivraison->isConnected())
    m_arduinoLivraison->write_to_arduino("3");  // ← attente opérateur

if (m_lcdLigne1) m_lcdLigne1->setText("ID OK - Termine");
if (m_lcdLigne2) m_lcdLigne2->setText("Cliquer Expedier");
```

---

## Flux correct après correction

```
Opérateur saisit ID sur keypad
        ↓
Arduino envoie "ID:xxx" à Qt
        ↓
Qt trouve la commande en BDD → met statut "Terminé"
        ↓
Qt envoie '3' → Arduino affiche "Cliquer Expédier" sur LCD
        ↓  (attente opérateur)
Opérateur clique btnExpédier dans Qt
        ↓
Qt envoie '1' → Arduino exécute expedierColis()
        ↓
Séquence servos :
  1. barriere : 90° → 0°  (s'ouvre)     800 ms
  2. bras     : 90° → 0°  (pousse colis) 1000 ms
  3. bras     :  0° → 90° (revient)      800 ms
  4. barriere :  0° → 90° (se referme)   500 ms
        ↓
Arduino envoie "1" à Qt → confirmation expédition
```

---

## Signaux série utilisés (Qt → Arduino LIVRAISON)

| Signal | Signification |
|--------|---------------|
| `'1'`  | Lancer les servos (expédition confirmée) |
| `'2'`  | ID invalide — rester en attente de saisie |
| `'3'`  | ID valide, statut Terminé — attendre clic Expédier |
| `'0'`  | Erreur générale |

---

## Fichiers concernés

| Fichier | Modification |
|---------|-------------|
| `mainwindow.cpp` | Remplacement de `write_to_arduino("1")` par `write_to_arduino("3")` dans le bloc `ID:` + mise à jour affichage LCD Qt |
| `arduino_sketch/arduino_sketch.ino` | Inchangé — gère déjà correctement `'3'` (`waitingExpedier = true`) |
