# Scénario Arduino CUIREA - Monitoring Température + Validation Livraison

## Vue d'ensemble

Le système Arduino CUIREA intègre deux fonctionnalités critiques:
1. **Monitoring température** des matières premières avec alerte automatique
2. **Validation des livraisons fournisseurs** par pesée automatique

---

## Matériel utilisé

| Composant | Broche | Rôle |
|-----------|--------|------|
| DHT11 #1 | 2 | Température matière première |
| DHT11 #2 | 3 | Température ambiante/météo |
| HX711 DOUT | 4 | Balance - Data Out |
| HX711 SCK | 5 | Balance - Serial Clock |
| LED RGB - Rouge | 6 | Alerte température |
| LED RGB - Vert | 7 | État normal |
| LED RGB - Bleu | 8 | Mode livraison |
| Buzzer | 9 | Alerte sonore |

### Câblage DHT11
```
DHT11 #1 (Matière):
  VCC → 5V
  GND → GND
  DATA → Pin 2 (avec résistance pull-up 10kΩ vers 5V)

DHT11 #2 (Ambiance):
  VCC → 5V
  GND → GND
  DATA → Pin 3 (avec résistance pull-up 10kΩ vers 5V)
```

### Câblage HX711
```
HX711:
  VCC → 5V
  GND → GND
  DOUT → Pin 4
  SCK → Pin 5
  E+ → Cellule de charge (rouge)
  E- → Cellule de charge (noir)
  A+ → Cellule de charge (blanc)
  A- → Cellule de charge (vert)
```

---

## Fonctionnalité 1: Monitoring Température

### Principe
- Lecture automatique toutes les 2 secondes
- Comparaison: Température matière vs Température ambiante
- Si **T_matière > T_ambiance** → ALERTE

### Flux d'alerte

```
Arduino lit DHT11 #1 et DHT11 #2
    │
    ├─ T_matière ≤ T_ambiance
    │   → LED verte
    │   → Envoi "TEMP:25.5,22.3" à Qt
    │   → Sauvegarde en BDD (ARDUINO_TEMP_READINGS)
    │
    └─ T_matière > T_ambiance
        → LED rouge(200ms)
        → Envoi "TEMP:28.5,22.3" à Qt
        → Qt déclenche:
            ├─ Notification système
            ├─ Email Brevo vers admin
            ├─ Navigation automatique vers page Matières
            └─ Sauvegarde en BDD avec flag alerte
```

### Sauvegarde BDD

```sql
INSERT INTO ARDUINO_TEMP_READINGS 
(ID_MATIERE, TEMP_MATIERE, TEMP_AMBIANTE, DATE_LECTURE)
VALUES (3, 28.5, 22.3, SYSDATE);
```

### Email d'alerte (Brevo API)

**Destinataire**: admin@cuirea.com  
**Sujet**: ⚠️ ALERTE TEMPÉRATURE - Matière Première  
**Corps**:
```
Alerte détectée à [HH:MM:SS]

Température matière: 28.5°C
Température ambiante: 22.3°C
Différence: +6.2°C

Action requise: Vérifier les conditions de stockage
```

---

## Fonctionnalité 2: Validation Livraison Fournisseur

### Principe
- Opérateur clique sur "Livraison" dans Gestion Fournisseurs
- Saisit la quantité commandée (ex: 45.5 kg)
- Place le colis sur la balance
- Arduino pèse et compare avec tolérance de 5%

### Flux de validation

```
1. Opérateur sélectionne fournisseur dans tableau
2. Clic sur bouton "Livraison"
3. Dialog Qt:
    ├─ Affiche: Nom fournisseur, Quantité commandée actuelle
    ├─ Champ: Nouvelle quantité commandée (modifiable)
    └─ Bouton "Démarrer pesée"

4. Qt envoie à Arduino: "START_DELIVERY:45.50\n"
5. Arduino:
        ├─ Si différence ≤ 5%:
        │   ├─ Envoi "WEIGHT:45.67" à Qt
        │   └─ Qt:
        │       ├─ UPDATE FOURNISSEURS SET QUANTITE_MESUREE = 45.67
        │       ├─ INSERT INTO ARDUINO_DELIVERY_LOG (STATUT='VALIDE')
        │       ├─ QMessageBox succès
        │       └─ Refresh tableau
        │
        └─ Si différence > 5%:
            ├─ LED rouge(200ms)
            ├─ Envoi "WEIGHT:38.20" à Qt
            └─ Qt:
                ├─ QMessageBox erreur avec détails
                ├─ INSERT INTO ARDUINO_DELIVERY_LOG (STATUT='REJETE')

### Mise à jour BDD

**Cas validation réussie:**
```sql
UPDATE FOURNISSEURS 
SET QUANTITE_COMMANDEE = 45.50,
    QUANTITE_MESUREE = 45.67
WHERE ID_FOURNISSEUR = 5;

INSERT INTO ARDUINO_DELIVERY_LOG 
(ID_FOURNISSEUR, QUANTITE_COMMANDEE, QUANTITE_MESUREE, STATUT)
VALUES (5, 45.50, 45.67, 'VALIDE');
```

**Cas rejet:**
```sql
INSERT INTO ARDUINO_DELIVERY_LOG 
(ID_FOURNISSEUR, QUANTITE_COMMANDEE, QUANTITE_MESUREE, STATUT)
VALUES (5, 45.50, 38.20, 'REJETE');
```

---

## Protocole de communication série

### Arduino → Qt

| Message | Format | Signification |
|---------|--------|---------------|
| `TEMP:25.5,22.3` | `TEMP:<matière>,<ambiance>` | Lecture température |
| `WEIGHT:45.67` | `WEIGHT:<kg>` | Poids mesuré |
| `ERROR:DHT_READ_FAILED` | `ERROR:<code>` | Erreur capteur |
| `DELIVERY_MODE_ACTIVE` | Texte | Confirmation mode livraison |
| `SCALE_TARED` | Texte | Balance remise à zéro |

### Qt → Arduino

| Commande | Format | Action Arduino |
|----------|--------|----------------|
| `GET_TEMP\n` | Texte | Force lecture température immédiate |
| `START_DELIVERY:45.50\n` | `START_DELIVERY:<kg>\n` | Active mode livraison avec poids cible |
| `CANCEL_DELIVERY\n` | Texte | Annule mode livraison |
| `TARE\n` | Texte | Remet balance à zéro |

---

## Interface Qt - Modifications

### Page Fournisseurs

**Nouveau bouton "Livraison"** (à côté de Modifier/Supprimer):
- Visible uniquement si une ligne est sélectionnée
- Ouvre `LivraisonDialog`

**Nouvelles colonnes dans tableau:**
- `Qté Commandée (kg)` - éditable via dialog
- `Qté Mesurée (kg)` - remplie automatiquement par Arduino
- Coloration:
  - Vert si différence ≤ 5%
  - Rouge si différence > 5%
  - Gris si pas encore mesurée

### LivraisonDialog

```
┌─────────────────────────────────────────┐
│  Validation Livraison Fournisseur       │
├─────────────────────────────────────────┤
│  Fournisseur: Tannerie Sfax            │
│  Quantité commandée: [45.50] kg         │
│                                         │
│  [Démarrer pesée]  [Annuler]           │
│                                         │
│  État: En attente du colis...          │
│  ⚖️ Placez le colis sur la balance      │
└─────────────────────────────────────────┘
```

### Page Matières Premières

**Indicateur température en temps réel** (coin supérieur droit):
```
🌡️ Matière: 25.5°C | Ambiance: 22.3°C
```
- Vert si OK
- Rouge clignotant si alerte

**Nouveau bouton "Historique Température"**:
- Ouvre graphique avec courbes des 24 dernières heures
- Utilise QtCharts (QLineSeries)

---

## Statistiques - Modifications

### Page Matières - Nouveau graphique

**Graphique température (24h)**:
- Axe X: Heures (0h → 24h)
- Axe Y: Température (°C)
- 2 courbes:
  - Bleue: Température matière
  - Verte: Température ambiante
- Zone rouge si T_matière > T_ambiance

### Export PDF - Ajout section

**Nouvelle section "Monitoring Arduino"**:
```
═══════════════════════════════════════
MONITORING ARDUINO - DERNIÈRES 24H
═══════════════════════════════════════

Température Matière:
  Moyenne: 24.8°C
  Min: 22.1°C
  Max: 27.3°C

Température Ambiance:
  Moyenne: 22.5°C
  Min: 20.8°C
  Max: 24.1°C

Alertes détectées: 3
Durée totale alerte: 45 minutes

Livraisons validées: 12
Livraisons rejetées: 2
Taux de conformité: 85.7%
```

---

## Fichiers modifiés/créés

### Nouveaux fichiers
- `arduinomonitor.h/.cpp` - Classe de monitoring
- `cuirea_arduino/cuirea_arduino.ino` - Sketch Arduino
- `arduino_migration.sql` - Script BDD
- `ARDUINO_CUIREA_SCENARIO.md` - Ce document

### Fichiers modifiés
- `fournisseur.h/.cpp` - Ajout quantiteCommandee/Mesuree
- `fournisseurdialog.h/.cpp` - Ajout bouton Livraison
- `mainwindow.h/.cpp` - Intégration ArduinoMonitor
- `statscharts.cpp` - Ajout graphique température
- `bilandialog.cpp` - Ajout section Arduino dans PDF

---

## Calibration balance HX711

### Procédure
1. Brancher HX711 sans poids
2. Upload sketch Arduino
3. Ouvrir Serial Monitor
4. Envoyer commande `TARE`
5. Placer poids connu (ex: 1kg)
6. Noter valeur brute affichée
7. Calculer facteur: `valeur_brute / poids_kg`
8. Modifier dans sketch: `scale.set_scale(facteur);`
9. Re-upload et tester

### Facteur typique
- Cellule 5kg: ~420
- Cellule 10kg: ~840
- Cellule 50kg: ~4200

---

## Tests recommandés

### Test température
1. Souffler air chaud sur DHT11 #1
2. Vérifier LED rouge + buzzer
3. Vérifier notification Qt
4. Vérifier email reçu
5. Vérifier sauvegarde BDD

### Test balance
1. Créer fournisseur test
2. Saisir quantité 1.00 kg
3. Placer poids 1kg sur balance
4. Vérifier validation (LED verte)
5. Placer poids 0.5kg
6. Vérifier rejet (LED rouge)

---

## Dépannage

### Arduino non détecté
- Vérifier driver CH340/CP2102 installé
- Vérifier port COM dans Gestionnaire de périphériques
- Essayer autre câble USB

### DHT11 retourne NaN
- Vérifier résistance pull-up 10kΩ
- Attendre 2 secondes entre lectures
- Vérifier alimentation 5V stable

### HX711 valeurs instables
- Vérifier câblage cellule de charge
- Isoler balance des vibrations
- Refaire calibration
- Augmenter nombre de lectures moyennées

---

**Version**: 1.0  
**Date**: 2026-05-01  
**Auteur**: CUIREA Development Team
