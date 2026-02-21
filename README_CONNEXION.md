# Configuration de la Connexion à la Base de Données Oracle

## Informations de Connexion

- **Base de données**: Projet2A
- **Source ODBC**: Projet2A
- **Utilisateur**: cuirea
- **Mot de passe**: 0000
- **Driver**: Oracle ODBC Driver

## Pattern Singleton

La classe Connection utilise le pattern Singleton pour garantir une seule instance de connexion dans toute l'application.

## Prérequis

1. **Oracle Client** doit être installé sur votre machine
2. **Oracle ODBC Driver** doit être configuré dans Windows
3. **Qt SQL Module** est activé dans le projet
4. **Source ODBC "Source_Projet2A"** doit être créée

## Configuration ODBC (Windows)

1. Ouvrir **Panneau de configuration** → **Outils d'administration** → **Sources de données ODBC (64 bits)**
2. Aller dans l'onglet **DSN Système**
3. Cliquer sur **Ajouter**
4. Sélectionner **Oracle in OraClient19Home1** (ou version installée)
5. Configurer:
   - **Data Source Name**: Projet2A
   - **TNS Service Name**: Projet2A
   - **User ID**: cuirea
6. Cliquer sur **Test Connection** pour vérifier
7. Cliquer sur **OK** pour sauvegarder

## Utilisation dans le Code

### Exemple 1: Connexion Simple (Singleton)

```cpp
#include "connection.h"

int main() {
    // Obtenir l'instance unique
    Connection* conn = Connection::instance();
    
    if (conn->createConnect()) {
        qDebug() << "Connexion réussie!";
        
        // Utiliser la connexion
        QSqlDatabase db = conn->getDatabase();
        
        // La connexion sera fermée automatiquement dans le destructeur
    } else {
        qDebug() << "Échec de la connexion";
    }
    
    return 0;
}
```

### Exemple 2: Exécuter une Requête

```cpp
#include "connection.h"
#include <QSqlQuery>

// Obtenir l'instance unique
Connection* conn = Connection::instance();

if (conn->createConnect()) {
    QSqlQuery query(conn->getDatabase());
    
    // SELECT
    if (query.exec("SELECT * FROM FOURNISSEUR")) {
        while (query.next()) {
            QString id = query.value(0).toString();
            QString nom = query.value(1).toString();
            qDebug() << "ID:" << id << "Nom:" << nom;
        }
    } else {
        qDebug() << "Erreur:" << query.lastError().text();
    }
    
    // INSERT
    query.prepare("INSERT INTO FOURNISSEUR (ID, NOM_ENTREPRISE, EMAIL) VALUES (:id, :nom, :email)");
    query.bindValue(":id", "F001");
    query.bindValue(":nom", "Test Company");
    query.bindValue(":email", "test@example.com");
    
    if (query.exec()) {
        qDebug() << "Insertion réussie!";
    } else {
        qDebug() << "Erreur:" << query.lastError().text();
    }
}
```

### Exemple 3: Intégration dans MainWindow

```cpp
// Dans mainwindow.cpp
#include "connection.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    ui->setupUi(this);
    
    // Obtenir l'instance unique de connexion
    Connection* conn = Connection::instance();
    
    if (conn->createConnect()) {
        qDebug() << "Base de données connectée!";
        loadFournisseurs(); // Charger les données
    } else {
        QMessageBox::critical(this, "Erreur de connexion", 
            "Impossible de se connecter à la base de données Projet2A");
    }
}

void MainWindow::loadFournisseurs() {
    Connection* conn = Connection::instance();
    QSqlQuery query(conn->getDatabase());
    
    if (query.exec("SELECT * FROM FOURNISSEUR")) {
        ui->fournisseurTable->setRowCount(0);
        
        while (query.next()) {
            int row = ui->fournisseurTable->rowCount();
            ui->fournisseurTable->insertRow(row);
            
            ui->fournisseurTable->setItem(row, 0, new QTableWidgetItem(query.value("ID").toString()));
            ui->fournisseurTable->setItem(row, 1, new QTableWidgetItem(query.value("NOM_ENTREPRISE").toString()));
            ui->fournisseurTable->setItem(row, 2, new QTableWidgetItem(query.value("EMAIL").toString()));
            // ... autres colonnes
        }
    }
}
```

## Dépannage

### Erreur: "Driver not loaded"
- Vérifier que Oracle Client est installé
- Vérifier que le driver ODBC est configuré
- Essayer de changer "Oracle in OraClient19Home1" par le nom exact de votre driver

### Erreur: "ORA-12154: TNS:could not resolve the connect identifier"
- Vérifier que le service "Projet2A" existe dans tnsnames.ora
- Vérifier la variable d'environnement TNS_ADMIN

### Erreur: "ORA-01017: invalid username/password"
- Vérifier les identifiants: cuirea / 0000
- Vérifier que l'utilisateur existe dans la base de données

## Structure des Tables

### Table FOURNISSEUR
```sql
CREATE TABLE FOURNISSEUR (
    ID VARCHAR2(10) PRIMARY KEY,
    NOM_ENTREPRISE VARCHAR2(100) NOT NULL,
    EMAIL VARCHAR2(100),
    TELEPHONE VARCHAR2(20),
    MATRICULE_FISCAL VARCHAR2(50),
    TYPE_PRODUIT VARCHAR2(50),
    CONDITION_PAIEMENT VARCHAR2(50),
    STATUT VARCHAR2(20)
);
```

## Notes Importantes

- La connexion est automatiquement fermée dans le destructeur
- Toujours vérifier le retour de `createConnection()` avant d'utiliser la base
- Utiliser `getLastError()` pour obtenir les détails des erreurs
- Le module Qt SQL doit être inclus: `QT += sql` dans le fichier .pro
