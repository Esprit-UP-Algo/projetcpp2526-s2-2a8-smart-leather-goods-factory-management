# Design: Restructuration CUIREA - Phase 1 et 5

## 1. Vue d'ensemble

Ce document détaille les modifications pour:
- **Phase 1**: Créer les StackedWidgets internes pour chaque module
- **Phase 5**: Renommer tous les objets selon la convention du guide

## 2. Architecture Cible

### 2.1 Structure des StackedWidgets

```
MainWindow
└── stackedWidget (principal - EXISTE)
    ├── Page 0: employeePage
    │   └── stackedWidgetEmpl (À CRÉER)
    │       └── Page 0: CRUD Employés (contenu actuel)
    │
    ├── Page 1: clientPage  
    │   └── stackedWidgetClient (À CRÉER)
    │       └── Page 0: CRUD Clients (contenu actuel)
    │
    └── Page 2: productPage
        └── stackedWidgetProduit (À CRÉER)
            └── Page 0: CRUD Produits (contenu actuel)
```

## 3. Renommage des Objets

### 3.1 Module Employés

**Boutons:**
- `btnAdd` → `btnAjouterEmploye`
- `btnEdit` → `btnModifierEmploye`
- `btnDelete` → `btnSupprimerEmploye`
- `btnExport` → `btnExporterEmploye`

**Tables:**
- `employeeTable` → `tblEmploye`

**Champs de recherche:**
- `searchBox` → `txtRechercheEmploye`

### 3.2 Module Clients

**Boutons:**
- `btnAddClient` → `btnAjouterClient`
- `btnEditClient` → `btnModifierClient`
- `btnDeleteClient` → `btnSupprimerClient`
- `btnExportClient` → `btnExporterClient`
- `btnRefreshClient` → `btnActualiserClient`

**Tables:**
- `clientTable` → `tblClient`

**Champs de recherche:**
- `searchBoxClient` → `txtRechercheClient`

### 3.3 Module Produits

**Boutons:**
- `btnAddProduct` → `btnAjouterProduit`
- `btnEditProduct` → `btnModifierProduit`
- `btnDeleteProduct` → `btnSupprimerProduit`
- `btnViewProduct` → `btnConsulterProduit`
- `btnStatisticsProduct` → `btnStatistiquesProduit`

**Tables:**
- `productTable` → `tblProduit`

**Champs de recherche:**
- `searchBoxProduct` → `txtRechercheProduit`

## 4. Modifications du Code C++

### 4.1 Fichiers à Modifier

- `mainwindow.h` - Déclarations des slots
- `mainwindow.cpp` - Implémentation des slots

### 4.2 Renommage des Slots

**Employés:**
```cpp
// Ancien → Nouveau
on_btnAdd_clicked() → on_btnAjouterEmploye_clicked()
on_btnEdit_clicked() → on_btnModifierEmploye_clicked()
on_btnDelete_clicked() → on_btnSupprimerEmploye_clicked()
on_btnExport_clicked() → on_btnExporterEmploye_clicked()
```

**Clients:**
```cpp
on_btnAddClient_clicked() → on_btnAjouterClient_clicked()
on_btnEditClient_clicked() → on_btnModifierClient_clicked()
on_btnDeleteClient_clicked() → on_btnSupprimerClient_clicked()
on_btnExportClient_clicked() → on_btnExporterClient_clicked()
on_btnRefreshClient_clicked() → on_btnActualiserClient_clicked()
```

**Produits:**
```cpp
on_btnAddProduct_clicked() → on_btnAjouterProduit_clicked()
on_btnEditProduct_clicked() → on_btnModifierProduit_clicked()
on_btnDeleteProduct_clicked() → on_btnSupprimerProduit_clicked()
on_btnViewProduct_clicked() → on_btnConsulterProduit_clicked()
on_btnStatisticsProduct_clicked() → on_btnStatistiquesProduit_clicked()
```

### 4.3 Références aux Widgets

Toutes les références `ui->employeeTable` deviennent `ui->tblEmploye`, etc.

## 5. Plan d'Implémentation

### Étape 1: Créer StackedWidgets Internes (UI)
1. Ouvrir mainwindow.ui dans Qt Designer
2. Pour chaque page (employeePage, clientPage, productPage):
   - Ajouter un QStackedWidget
   - Le nommer selon convention
   - Déplacer tout le contenu actuel dans page 0 du nouveau stackedWidget

### Étape 2: Renommer les Objets (UI)
1. Renommer tous les boutons selon la table ci-dessus
2. Renommer toutes les tables
3. Renommer tous les champs de recherche

### Étape 3: Mettre à Jour le Code C++
1. Renommer les slots dans mainwindow.h
2. Renommer les slots dans mainwindow.cpp
3. Mettre à jour toutes les références aux widgets

## 6. Tests de Validation

- [ ] Tous les boutons fonctionnent après renommage
- [ ] Les tables s'affichent correctement
- [ ] La recherche fonctionne
- [ ] La navigation entre modules fonctionne
- [ ] Les stackedWidgets internes sont créés
- [ ] Aucune régression fonctionnelle
