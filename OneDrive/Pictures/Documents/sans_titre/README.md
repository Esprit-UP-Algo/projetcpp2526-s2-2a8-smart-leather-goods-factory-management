d# CUIREA - Système de Gestion

Système de gestion complet pour CUIREA (entreprise de maroquinerie) développé avec Qt/C++.

## 📋 Modules

### ✅ Modules Implémentés
- **Employés**: Gestion complète des employés avec profil détaillé
- **Clients**: Gestion des clients avec historique
- **Produits**: Gestion des produits avec statistiques

### 🔄 Modules Prévus
- Commandes
- Matières Premières
- Fournisseurs

## 🏗️ Architecture

Le projet respecte 100% le guide d'intégration Qt avec:
- Structure modulaire avec StackedWidgets internes
- Nommage significatif des objets (btnAjouterEmploye, tblEmploye, etc.)
- Slots auto-générés selon convention Qt
- Interface unique avec navigation fluide

### Structure
```
MainWindow
└── stackedWidget (principal)
    ├── Page 0: employeePage
    │   └── stackedWidgetEmpl
    │       └── Page 0: CRUD Employés
    ├── Page 1: clientPage
    │   └── stackedWidgetClient
    │       └── Page 0: CRUD Clients
    └── Page 2: productPage
        └── stackedWidgetProduit
            └── Page 0: CRUD Produits
```

## 🎨 Design

- Thème CUIREA: #FAF5F0, #8D6E63, #BCAAA4
- Interface moderne et professionnelle
- Navigation intuitive avec sidebar
- Tableaux avec tri et recherche

## 🚀 Compilation

```bash
qmake sans_titre.pro
make
```

## 📁 Fichiers Principaux

- `mainwindow.ui` - Interface graphique principale
- `mainwindow.cpp/h` - Logique principale
- `employeedialog.cpp/h` - Dialogue employés
- `clientmanagerdialog.cpp/h` - Dialogue clients
- `productdialog.cpp/h` - Dialogue produits
- `client.h` - Classe Client
- `produit.h` - Classe Produit

## ✨ Fonctionnalités

### Module Employés
- Ajout/Modification/Suppression
- Recherche et tri
- Export des données
- Profil détaillé avec photo

### Module Clients
- CRUD complet
- Recherche et filtrage
- Actualisation
- Export

### Module Produits
- Gestion complète
- Consultation détaillée
- Statistiques avancées
- Suivi du stock

## 📝 Convention de Nommage

- Boutons: `btn<Action><Module>` (ex: btnAjouterEmploye)
- Tables: `tbl<Module>` (ex: tblEmploye)
- Champs texte: `txt<Champ><Module>` (ex: txtRechercheEmploye)
- StackedWidgets: `stackedWidget<Module>` (ex: stackedWidgetEmpl)

## 🔧 Technologies

- Qt 6.x
- C++17
- Qt Designer pour l'interface

## 📄 Licence

Projet académique - 2A AU 2025-2026
