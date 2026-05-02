# Intégration Arduino CUIREA - Résumé Complet

## ✅ Travail Réalisé

### 1. Système Arduino - Monitoring Température
- **2 capteurs DHT11**:
  - DHT11 #1 (pin 2): Température matière première
  - DHT11 #2 (pin 3): Température ambiante/météo
- **LED RGB** (pins 6,7,8): Indication visuelle état
- **Buzzer** (pin 9): Alerte sonore
- **Logique d'alerte**: Si T_matière > T_ambiance → Alerte

### 2. Système Arduino - Balance Livraison
- **Capteur HX711** (pins 4,5): Balance de précision
- **Validation automatique**: Tolérance 5%
- **Feedback visuel/sonore**: LED + Buzzer

### 3. Base de Données
**Nouvelles tables créées:**
- `ARDUINO_TEMP_READINGS`: Historique températures
- `ARDUINO_DELIVERY_LOG`: Historique validations livraison

**Colonnes ajoutées à FOURNISSEURS:**
- `QUANTITE_COMMANDEE`: Quantité commandée (kg)
- `QUANTITE_MESUREE`: Quantité mesurée par balance (kg)

### 4. Classes C++ Créées

#### ArduinoMonitor (arduinomonitor.h/.cpp)
- Gestion connexion Arduino
- Lecture température en temps réel
- Validation livraison fournisseur
- Sauvegarde automatique en BDD
- Envoi emails d'alerte

#### EmailAlerte (emailalerte.h/.cpp)
- Intégration API Brevo
- Email alerte température
- Email livraison rejetée
- Templates HTML professionnels

### 5. Sketch Arduino (cuirea_arduino.ino)
- Lecture 2x DHT11 simultanée
- Gestion HX711 avec calibration
- Protocole série robuste
- États LED RGB
- Alertes buzzer

### 6. Modifications Fournisseurs
**Classe FournisseurData:**
- Ajout `quantiteCommandee` (double)
- Ajout `quantiteMesuree` (double)
- Getters/Setters correspondants

**Interface (à implémenter):**
- Nouveau bouton "Livraison"
- Dialog validation avec Arduino
- Colonnes quantités dans tableau
- Coloration selon conformité

### 7. Statistiques Matières (à implémenter)
- Graphique température 24h (QtCharts)
- Courbes matière + ambiance
- Zones d'alerte colorées
- Export PDF avec section Arduino

---

## 📁 Fichiers Créés

```
integ/
├── arduinomonitor.h              # Classe monitoring Arduino
├── arduinomonitor.cpp
├── emailalerte.h                 # Classe envoi emails Brevo
├── emailalerte.cpp
├── cuirea_arduino/
│   └── cuirea_arduino.ino        # Sketch Arduino complet
├── arduino_migration.sql         # Script BDD
├── ARDUINO_CUIREA_SCENARIO.md    # Documentation complète
└── INTEGRATION_COMPLETE.md       # Ce fichier
```

---

## 🔧 Fichiers à Modifier

### mainwindow.h
```cpp
#include "arduinomonitor.h"

private:
    ArduinoMonitor *m_arduinoMonitor;
    QLabel *m_tempIndicator;  // Indicateur température
```

### mainwindow.cpp - Constructeur
```cpp
// Initialiser Arduino Monitor
m_arduinoMonitor = new ArduinoMonitor(this);

// Connecter signaux
connect(m_arduinoMonitor, &ArduinoMonitor::temperatureUpdated,
        this, &MainWindow::onTemperatureUpdated);
connect(m_arduinoMonitor, &ArduinoMonitor::temperatureAlert,
        this, &MainWindow::onTemperatureAlert);
connect(m_arduinoMonitor, &ArduinoMonitor::deliveryValidated,
        this, &MainWindow::onDeliveryValidated);
connect(m_arduinoMonitor, &ArduinoMonitor::deliveryRejected,
        this, &MainWindow::onDeliveryRejected);

// Démarrer monitoring
if (m_arduinoMonitor->isConnected()) {
    m_arduinoMonitor->startTemperatureMonitoring();
}
```

### mainwindow.cpp - Nouveaux slots
```cpp
void MainWindow::onTemperatureUpdated(double tempMat, double tempAmb)
{
    m_tempIndicator->setText(
        QString("🌡️ Matière: %1°C | Ambiance: %2°C")
        .arg(tempMat, 0, 'f', 1)
        .arg(tempAmb, 0, 'f', 1)
    );
    
    if (tempMat > tempAmb) {
        m_tempIndicator->setStyleSheet("color: red; font-weight: bold;");
    } else {
        m_tempIndicator->setStyleSheet("color: green;");
    }
}

void MainWindow::onTemperatureAlert(double tempMat, double tempAmb)
{
    QMessageBox::warning(this, "⚠️ Alerte Température",
        QString("Température matière (%1°C) supérieure à l'ambiance (%2°C)\n\n"
                "Vérifiez les conditions de stockage!")
        .arg(tempMat, 0, 'f', 1)
        .arg(tempAmb, 0, 'f', 1)
    );
    
    // Navigation automatique vers Matières
    on_btnRawMaterials_clicked();
}

void MainWindow::onDeliveryValidated(int idFournisseur, double quantiteMesuree)
{
    // Mettre à jour BDD
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("UPDATE FOURNISSEURS SET QUANTITE_MESUREE = :qte "
                  "WHERE ID_FOURNISSEUR = :id");
    query.bindValue(":qte", quantiteMesuree);
    query.bindValue(":id", idFournisseur);
    query.exec();
    
    // Log validation
    query.prepare("INSERT INTO ARDUINO_DELIVERY_LOG "
                  "(ID_FOURNISSEUR, QUANTITE_COMMANDEE, QUANTITE_MESUREE, STATUT) "
                  "VALUES (:id, :cmd, :mes, 'VALIDE')");
    query.bindValue(":id", idFournisseur);
    query.bindValue(":cmd", m_arduinoMonitor->getExpectedQuantity());
    query.bindValue(":mes", quantiteMesuree);
    query.exec();
    
    // Refresh tableau
    setupFournisseurTable();
    
    QMessageBox::information(this, "✅ Livraison Validée",
        QString("Quantité mesurée: %1 kg\n\nLivraison conforme!")
        .arg(quantiteMesuree, 0, 'f', 2)
    );
}

void MainWindow::onDeliveryRejected(int idFournisseur, double quantiteMesuree, double quantiteCommandee)
{
    // Log rejet
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("INSERT INTO ARDUINO_DELIVERY_LOG "
                  "(ID_FOURNISSEUR, QUANTITE_COMMANDEE, QUANTITE_MESUREE, STATUT) "
                  "VALUES (:id, :cmd, :mes, 'REJETE')");
    query.bindValue(":id", idFournisseur);
    query.bindValue(":cmd", quantiteCommandee);
    query.bindValue(":mes", quantiteMesuree);
    query.exec();
    
    double diff = qAbs(quantiteMesuree - quantiteCommandee);
    double diffPercent = (diff / quantiteCommandee) * 100.0;
    
    QMessageBox::critical(this, "❌ Livraison Rejetée",
        QString("Quantité commandée: %1 kg\n"
                "Quantité mesurée: %2 kg\n"
                "Différence: %3 kg (%4%)\n\n"
                "La livraison dépasse la tolérance de 5%!")
        .arg(quantiteCommandee, 0, 'f', 2)
        .arg(quantiteMesuree, 0, 'f', 2)
        .arg(diff, 0, 'f', 2)
        .arg(diffPercent, 0, 'f', 1)
    );
}
```

### fournisseurdialog.cpp - Bouton Livraison
```cpp
void FournisseurDialog::on_btnLivraison_clicked()
{
    if (!m_arduinoMonitor->isConnected()) {
        QMessageBox::warning(this, "Arduino", 
            "Arduino non connecté. Impossible de valider la livraison.");
        return;
    }
    
    bool ok;
    double quantite = QInputDialog::getDouble(this, 
        "Validation Livraison",
        "Quantité commandée (kg):",
        spinQuantiteCommandee->value(),
        0.0, 10000.0, 2, &ok
    );
    
    if (ok) {
        spinQuantiteCommandee->setValue(quantite);
        m_arduinoMonitor->startDeliveryCheck(
            txtId->text().toInt(),
            quantite
        );
        
        QMessageBox::information(this, "Balance Active",
            "Placez le colis sur la balance...\n\n"
            "La validation se fera automatiquement."
        );
    }
}
```

### statscharts.cpp - Graphique Température
```cpp
void StatsCharts::showTemperatureChart()
{
    // Récupérer données 24h
    QSqlQuery query(Connection::instance()->getDatabase());
    query.exec("SELECT DATE_LECTURE, TEMP_MATIERE, TEMP_AMBIANTE "
               "FROM ARDUINO_TEMP_READINGS "
               "WHERE DATE_LECTURE >= SYSDATE - 1 "
               "ORDER BY DATE_LECTURE");
    
    QLineSeries *seriesMatiere = new QLineSeries();
    seriesMatiere->setName("Température Matière");
    seriesMatiere->setColor(QColor("#2196F3"));
    
    QLineSeries *seriesAmbiance = new QLineSeries();
    seriesAmbiance->setName("Température Ambiance");
    seriesAmbiance->setColor(QColor("#4CAF50"));
    
    while (query.next()) {
        QDateTime dt = query.value(0).toDateTime();
        double tempMat = query.value(1).toDouble();
        double tempAmb = query.value(2).toDouble();
        
        qint64 timestamp = dt.toMSecsSinceEpoch();
        seriesMatiere->append(timestamp, tempMat);
        seriesAmbiance->append(timestamp, tempAmb);
    }
    
    QChart *chart = new QChart();
    chart->addSeries(seriesMatiere);
    chart->addSeries(seriesAmbiance);
    chart->setTitle("Monitoring Température - 24 Dernières Heures");
    
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("HH:mm");
    axisX->setTitleText("Heure");
    chart->addAxis(axisX, Qt::AlignBottom);
    seriesMatiere->attachAxis(axisX);
    seriesAmbiance->attachAxis(axisX);
    
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Température (°C)");
    chart->addAxis(axisY, Qt::AlignLeft);
    seriesMatiere->attachAxis(axisY);
    seriesAmbiance->attachAxis(axisY);
    
    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    
    // Afficher dans dialog
    QDialog dlg(this);
    dlg.setWindowTitle("Historique Température");
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->addWidget(chartView);
    dlg.resize(800, 600);
    dlg.exec();
}
```

---

## 📋 Checklist Intégration

### Base de Données
- [ ] Exécuter `arduino_migration.sql`
- [ ] Vérifier tables créées
- [ ] Tester INSERT dans ARDUINO_TEMP_READINGS
- [ ] Tester INSERT dans ARDUINO_DELIVERY_LOG

### Arduino
- [ ] Installer bibliothèques: DHT, HX711
- [ ] Câbler 2x DHT11 (pins 2, 3)
- [ ] Câbler HX711 (pins 4, 5)
- [ ] Câbler LED RGB (pins 6, 7, 8)
- [ ] Câbler Buzzer (pin 9)
- [ ] Upload sketch `cuirea_arduino.ino`
- [ ] Calibrer balance HX711
- [ ] Tester lecture température
- [ ] Tester pesée

### Code Qt
- [ ] Ajouter `arduinomonitor.h/.cpp` au projet
- [ ] Ajouter `emailalerte.h/.cpp` au projet
- [ ] Modifier `mainwindow.h` (includes + membres)
- [ ] Modifier `mainwindow.cpp` (init + slots)
- [ ] Modifier `fournisseurdialog.cpp` (bouton Livraison)
- [ ] Modifier `statscharts.cpp` (graphique température)
- [ ] Compiler et tester

### Configuration
- [ ] Ajouter `BREVO_API_KEY` dans `.env`
- [ ] Configurer email destinataire alertes
- [ ] Tester envoi email

### Tests
- [ ] Test alerte température (souffler air chaud)
- [ ] Test validation livraison (poids conforme)
- [ ] Test rejet livraison (poids non conforme)
- [ ] Test sauvegarde BDD
- [ ] Test email Brevo
- [ ] Test graphique température
- [ ] Test export PDF avec section Arduino

---

## 🚀 Prochaines Étapes

1. **Exécuter migration BDD**
2. **Compiler et tester le code**
3. **Câbler et tester Arduino**
4. **Implémenter interface Livraison**
5. **Implémenter graphique température**
6. **Tester bout en bout**

---

**Version**: 1.0  
**Date**: 2026-05-01  
**Status**: ✅ Code créé, prêt pour intégration
