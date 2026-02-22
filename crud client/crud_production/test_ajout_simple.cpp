// Fichier de test pour diagnostiquer le problème d'ajout
// À ajouter temporairement dans mainwindow.cpp pour tester

// Test 1: Ajout avec valeurs fixes
void MainWindow::testAjoutSimple()
{
    qDebug() << "=== TEST AJOUT SIMPLE ===";
    
    Production prod;
    prod.setReference("TEST-2026-999");
    prod.setClient("Client Test");
    prod.setType("Type Test");
    prod.setMontantHT(100.0);
    prod.setDateCreation(QDate::currentDate());
    prod.setDateLivraison(QDate::currentDate().addDays(30));
    prod.setStatut("En Attente");
    prod.setPriorite("Normale");
    
    qDebug() << "Référence:" << prod.getReference();
    qDebug() << "Client:" << prod.getClient();
    qDebug() << "Type:" << prod.getType();
    qDebug() << "Montant:" << prod.getMontantHT();
    qDebug() << "Date création:" << prod.getDateCreation();
    qDebug() << "Date livraison:" << prod.getDateLivraison();
    qDebug() << "Statut:" << prod.getStatut();
    qDebug() << "Priorité:" << prod.getPriorite();
    
    if (prod.ajouter()) {
        qDebug() << "✅ TEST RÉUSSI - L'ajout fonctionne!";
        QMessageBox::information(this, "Test", "L'ajout fonctionne! Le problème vient des données du dialogue.");
    } else {
        qDebug() << "❌ TEST ÉCHOUÉ - Problème avec la base de données";
        QMessageBox::critical(this, "Test", "Problème avec la base de données. Vérifiez la console.");
    }
}

// Test 2: Vérifier la connexion
void MainWindow::testConnexion()
{
    qDebug() << "=== TEST CONNEXION ===";
    
    QSqlDatabase db = Connection::instance()->getDatabase();
    
    if (!db.isOpen()) {
        qDebug() << "❌ Base de données NON connectée";
        QMessageBox::critical(this, "Erreur", "La base de données n'est pas connectée!");
        return;
    }
    
    qDebug() << "✅ Base de données connectée";
    qDebug() << "Driver:" << db.driverName();
    qDebug() << "Database:" << db.databaseName();
    
    // Tester une requête simple
    QSqlQuery query(db);
    if (query.exec("SELECT COUNT(*) FROM COMMANDES")) {
        if (query.next()) {
            int count = query.value(0).toInt();
            qDebug() << "✅ Nombre de commandes:" << count;
            QMessageBox::information(this, "Test", 
                QString("Connexion OK!\nNombre de commandes: %1").arg(count));
        }
    } else {
        qDebug() << "❌ Erreur requête:" << query.lastError().text();
        QMessageBox::critical(this, "Erreur", 
            QString("Erreur SQL:\n%1").arg(query.lastError().text()));
    }
}

// Test 3: Vérifier les contraintes de la table
void MainWindow::testContraintes()
{
    qDebug() << "=== TEST CONTRAINTES ===";
    
    QSqlDatabase db = Connection::instance()->getDatabase();
    QSqlQuery query(db);
    
    // Vérifier les contraintes CHECK
    query.exec("SELECT constraint_name, search_condition FROM user_constraints "
               "WHERE table_name = 'COMMANDES' AND constraint_type = 'C'");
    
    qDebug() << "Contraintes CHECK:";
    while (query.next()) {
        qDebug() << "  -" << query.value(0).toString() << ":" << query.value(1).toString();
    }
    
    // Vérifier les colonnes
    query.exec("SELECT column_name, data_type, nullable FROM user_tab_columns "
               "WHERE table_name = 'COMMANDES' ORDER BY column_id");
    
    qDebug() << "\nColonnes:";
    while (query.next()) {
        qDebug() << "  -" << query.value(0).toString() 
                 << "(" << query.value(1).toString() << ")"
                 << (query.value(2).toString() == "N" ? "NOT NULL" : "NULL");
    }
}

/* 
INSTRUCTIONS D'UTILISATION:

1. Ajouter ces fonctions dans mainwindow.h (section private):
   void testAjoutSimple();
   void testConnexion();
   void testContraintes();

2. Ajouter un bouton de test dans l'interface ou appeler depuis le constructeur:
   testConnexion();
   testContraintes();
   testAjoutSimple();

3. Compiler et lancer
4. Regarder la console pour les résultats
*/
