# Tasks: Restructuration CUIREA - Conformité Guide 100%

## Phase 1: Créer StackedWidgets Internes

### 1.1 Module Employés - StackedWidget Interne
- [x] 1.1.1 Créer `stackedWidgetEmpl` dans `employeePage`
- [x] 1.1.2 Créer Page 0 dans `stackedWidgetEmpl` 
- [x] 1.1.3 Déplacer tout le contenu actuel de `employeePage` vers Page 0
- [x] 1.1.4 Tester l'affichage du module Employés

### 1.2 Module Clients - StackedWidget Interne
- [x] 1.2.1 Créer `stackedWidgetClient` dans `clientPage`
- [x] 1.2.2 Créer Page 0 dans `stackedWidgetClient`
- [x] 1.2.3 Déplacer tout le contenu actuel de `clientPage` vers Page 0
- [x] 1.2.4 Tester l'affichage du module Clients

### 1.3 Module Produits - StackedWidget Interne
- [x] 1.3.1 Créer `stackedWidgetProduit` dans `productPage`
- [x] 1.3.2 Créer Page 0 dans `stackedWidgetProduit`
- [x] 1.3.3 Déplacer tout le contenu actuel de `productPage` vers Page 0
- [x] 1.3.4 Tester l'affichage du module Produits

## Phase 2: Renommage des Objets - Module Employés

### 2.1 Boutons Employés
- [x] 2.1.1 Renommer `btnAdd` → `btnAjouterEmploye`
- [x] 2.1.2 Renommer `btnEdit` → `btnModifierEmploye`
- [x] 2.1.3 Renommer `btnDelete` → `btnSupprimerEmploye`
- [x] 2.1.4 Renommer `btnExport` → `btnExporterEmploye`

### 2.2 Tables et Champs Employés
- [x] 2.2.1 Renommer `employeeTable` → `tblEmploye`
- [x] 2.2.2 Renommer `searchBox` → `txtRechercheEmploye`

## Phase 3: Renommage des Objets - Module Clients

### 3.1 Boutons Clients
- [x] 3.1.1 Renommer `btnAddClient` → `btnAjouterClient`
- [x] 3.1.2 Renommer `btnEditClient` → `btnModifierClient`
- [x] 3.1.3 Renommer `btnDeleteClient` → `btnSupprimerClient`
- [x] 3.1.4 Renommer `btnRefreshClient` → `btnActualiserClient`
- [x] 3.1.5 Renommer `btnExportClient` → `btnExporterClient`

### 3.2 Tables et Champs Clients
- [x] 3.2.1 Renommer `clientTable` → `tblClient`
- [x] 3.2.2 Renommer `searchBoxClient` → `txtRechercheClient`

## Phase 4: Renommage des Objets - Module Produits

### 4.1 Boutons Produits
- [x] 4.1.1 Renommer `btnAddProduct` → `btnAjouterProduit`
- [x] 4.1.2 Renommer `btnEditProduct` → `btnModifierProduit`
- [x] 4.1.3 Renommer `btnDeleteProduct` → `btnSupprimerProduit`
- [x] 4.1.4 Renommer `btnViewProduct` → `btnConsulterProduit`
- [x] 4.1.5 Renommer `btnStatisticsProduct` → `btnStatistiquesProduit`

### 4.2 Tables et Champs Produits
- [x] 4.2.1 Renommer `productTable` → `tblProduit`
- [x] 4.2.2 Renommer `searchBoxProduct` → `txtRechercheProduit`

## Phase 5: Mise à Jour du Code C++ - mainwindow.h

### 5.1 Slots Employés
- [x] 5.1.1 Renommer `on_btnAdd_clicked()` → `on_btnAjouterEmploye_clicked()`
- [x] 5.1.2 Renommer `on_btnEdit_clicked()` → `on_btnModifierEmploye_clicked()`
- [x] 5.1.3 Renommer `on_btnDelete_clicked()` → `on_btnSupprimerEmploye_clicked()`
- [x] 5.1.4 Renommer `on_btnExport_clicked()` → `on_btnExporterEmploye_clicked()`

### 5.2 Slots Clients
- [x] 5.2.1 Renommer `on_btnAddClient_clicked()` → `on_btnAjouterClient_clicked()`
- [x] 5.2.2 Renommer `on_btnEditClient_clicked()` → `on_btnModifierClient_clicked()`
- [x] 5.2.3 Renommer `on_btnDeleteClient_clicked()` → `on_btnSupprimerClient_clicked()`
- [x] 5.2.4 Renommer `on_btnRefreshClient_clicked()` → `on_btnActualiserClient_clicked()`
- [x] 5.2.5 Renommer `on_btnExportClient_clicked()` → `on_btnExporterClient_clicked()`

### 5.3 Slots Produits
- [x] 5.3.1 Renommer `on_btnAddProduct_clicked()` → `on_btnAjouterProduit_clicked()`
- [x] 5.3.2 Renommer `on_btnEditProduct_clicked()` → `on_btnModifierProduit_clicked()`
- [x] 5.3.3 Renommer `on_btnDeleteProduct_clicked()` → `on_btnSupprimerProduit_clicked()`
- [x] 5.3.4 Renommer `on_btnViewProduct_clicked()` → `on_btnConsulterProduit_clicked()`
- [x] 5.3.5 Renommer `on_btnStatisticsProduct_clicked()` → `on_btnStatistiquesProduit_clicked()`

## Phase 6: Mise à Jour du Code C++ - mainwindow.cpp

### 6.1 Implémentation Slots Employés
- [x] 6.1.1 Mettre à jour implémentation `on_btnAjouterEmploye_clicked()`
- [x] 6.1.2 Mettre à jour implémentation `on_btnModifierEmploye_clicked()`
- [x] 6.1.3 Mettre à jour implémentation `on_btnSupprimerEmploye_clicked()`
- [x] 6.1.4 Mettre à jour implémentation `on_btnExporterEmploye_clicked()`

### 6.2 Implémentation Slots Clients
- [x] 6.2.1 Mettre à jour implémentation `on_btnAjouterClient_clicked()`
- [x] 6.2.2 Mettre à jour implémentation `on_btnModifierClient_clicked()`
- [x] 6.2.3 Mettre à jour implémentation `on_btnSupprimerClient_clicked()`
- [x] 6.2.4 Mettre à jour implémentation `on_btnActualiserClient_clicked()`
- [x] 6.2.5 Mettre à jour implémentation `on_btnExporterClient_clicked()`

### 6.3 Implémentation Slots Produits
- [x] 6.3.1 Mettre à jour implémentation `on_btnAjouterProduit_clicked()`
- [x] 6.3.2 Mettre à jour implémentation `on_btnModifierProduit_clicked()`
- [x] 6.3.3 Mettre à jour implémentation `on_btnSupprimerProduit_clicked()`
- [x] 6.3.4 Mettre à jour implémentation `on_btnConsulterProduit_clicked()`
- [x] 6.3.5 Mettre à jour implémentation `on_btnStatistiquesProduit_clicked()`

### 6.4 Références aux Widgets
- [x] 6.4.1 Remplacer toutes références `ui->employeeTable` → `ui->tblEmploye`
- [x] 6.4.2 Remplacer toutes références `ui->clientTable` → `ui->tblClient`
- [x] 6.4.3 Remplacer toutes références `ui->productTable` → `ui->tblProduit`
- [x] 6.4.4 Remplacer toutes références `ui->searchBox` → `ui->txtRechercheEmploye`
- [x] 6.4.5 Remplacer toutes références `ui->searchBoxClient` → `ui->txtRechercheClient`
- [x] 6.4.6 Remplacer toutes références `ui->searchBoxProduct` → `ui->txtRechercheProduit`
- [ ] 5.3.5 Renommer `on_btnStatisticsProduct_clicked()` → `on_btnStatistiquesProduit_clicked()`

## Phase 6: Mise à Jour du Code C++ - mainwindow.cpp

### 6.1 Implémentation Slots Employés
- [ ] 6.1.1 Mettre à jour implémentation `on_btnAjouterEmploye_clicked()`
- [ ] 6.1.2 Mettre à jour implémentation `on_btnModifierEmploye_clicked()`
- [ ] 6.1.3 Mettre à jour implémentation `on_btnSupprimerEmploye_clicked()`
- [ ] 6.1.4 Mettre à jour implémentation `on_btnExporterEmploye_clicked()`

### 6.2 Implémentation Slots Clients
- [ ] 6.2.1 Mettre à jour implémentation `on_btnAjouterClient_clicked()`
- [ ] 6.2.2 Mettre à jour implémentation `on_btnModifierClient_clicked()`
- [ ] 6.2.3 Mettre à jour implémentation `on_btnSupprimerClient_clicked()`
- [ ] 6.2.4 Mettre à jour implémentation `on_btnActualiserClient_clicked()`
- [ ] 6.2.5 Mettre à jour implémentation `on_btnExporterClient_clicked()`

### 6.3 Implémentation Slots Produits
- [ ] 6.3.1 Mettre à jour implémentation `on_btnAjouterProduit_clicked()`
- [ ] 6.3.2 Mettre à jour implémentation `on_btnModifierProduit_clicked()`
- [ ] 6.3.3 Mettre à jour implémentation `on_btnSupprimerProduit_clicked()`
- [ ] 6.3.4 Mettre à jour implémentation `on_btnConsulterProduit_clicked()`
- [ ] 6.3.5 Mettre à jour implémentation `on_btnStatistiquesProduit_clicked()`

### 6.4 Références aux Widgets
- [ ] 6.4.1 Remplacer toutes références `ui->employeeTable` → `ui->tblEmploye`
- [ ] 6.4.2 Remplacer toutes références `ui->clientTable` → `ui->tblClient`
- [ ] 6.4.3 Remplacer toutes références `ui->productTable` → `ui->tblProduit`
- [ ] 6.4.4 Remplacer toutes références `ui->searchBox` → `ui->txtRechercheEmploye`
- [ ] 6.4.5 Remplacer toutes références `ui->searchBoxClient` → `ui->txtRechercheClient`
- [ ] 6.4.6 Remplacer toutes références `ui->searchBoxProduct` → `ui->txtRechercheProduit`

## Phase 7: Tests et Validation

### 7.1 Tests Fonctionnels
- [ ] 7.1.1 Tester navigation entre modules
- [ ] 7.1.2 Tester bouton Ajouter pour chaque module
- [ ] 7.1.3 Tester bouton Modifier pour chaque module
- [ ] 7.1.4 Tester bouton Supprimer pour chaque module
- [ ] 7.1.5 Tester bouton Consulter (Produits)
- [ ] 7.1.6 Tester bouton Statistiques (Produits)
- [ ] 7.1.7 Tester bouton Exporter pour chaque module
- [ ] 7.1.8 Tester recherche pour chaque module
- [ ] 7.1.9 Tester sélection dans tables
- [ ] 7.1.10 Tester enable/disable des boutons

### 7.2 Tests Visuels
- [ ] 7.2.1 Vérifier affichage module Employés
- [ ] 7.2.2 Vérifier affichage module Clients
- [ ] 7.2.3 Vérifier affichage module Produits
- [ ] 7.2.4 Vérifier profile panel (Employés)
- [ ] 7.2.5 Vérifier thème CUIREA conservé

### 7.3 Validation Conformité Guide
- [ ] 7.3.1 Vérifier nommage des objets conforme
- [ ] 7.3.2 Vérifier stackedWidgets internes créés
- [ ] 7.3.3 Vérifier slots auto-générés fonctionnels
- [ ] 7.3.4 Vérifier structure modulaire respectée
