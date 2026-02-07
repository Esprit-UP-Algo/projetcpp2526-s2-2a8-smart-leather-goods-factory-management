# Spécification: Restructuration du Projet CUIREA selon le Guide d'Intégration

## 1. Vue d'ensemble

Restructurer le projet CUIREA pour qu'il respecte complètement le guide d'intégration fourni, en implémentant une architecture avec QStackedWidget principal et QStackedWidgets internes pour chaque module.

## 2. Objectifs

- Respecter strictement le guide d'intégration Qt fourni
- Implémenter une architecture modulaire avec stackedWidgets imbriqués
- Maintenir toutes les fonctionnalités existantes
- Améliorer la navigation et la structure du code
- Utiliser des noms d'objets significatifs selon les conventions

## 3. User Stories et Critères d'Acceptation

### 3.1 Structure Principale

**US-1: En tant qu'utilisateur, je veux une interface unique avec navigation modulaire**

**Critères d'acceptation:**
- ✅ Un seul fichier mainwindow.ui (déjà fait)
- ✅ QStackedWidget principal nommé `stackedWidget` (déjà fait)
- ⚠️ Chaque module doit avoir son propre QStackedWidget interne (À FAIRE)
- ⚠️ Noms d'objets significatifs selon convention (À VÉRIFIER)

### 3.2 Module Employés

**US-2: En tant qu'utilisateur, je veux un module Employés complet avec sous-pages**

**Critères d'acceptation:**
- ⚠️ QStackedWidget interne nommé `stackedWidgetEmpl` (À CRÉER)
- ⚠️ Page 0: Login Employé (À CRÉER)
- ⚠️ Page 1: Menu principal de l'application (À CRÉER)
- ✅ Page 2: CRUD Employés (existe déjà, à déplacer vers page 3)
- ⚠️ Page 3: Mot de passe oublié (optionnel, À CRÉER)

**Sous-pages du menu (Page 1):**
- Bouton vers CRUD Employés (page 3 du stackedWidgetEmpl)
- Bouton vers module Clients (page 1 du stackedWidget principal)
- Bouton vers module Produits (page 2 du stackedWidget principal)
- Bouton Quitter → retour login (page 0)

### 3.3 Module Clients

**US-3: En tant qu'utilisateur, je veux un module Clients complet avec sous-pages**

**Critères d'acceptation:**
- ⚠️ QStackedWidget interne nommé `stackedWidgetClient` (À CRÉER)
- ⚠️ Page 0: Login Client (optionnel, À CRÉER)
- ⚠️ Page 1: Menu ou accès direct au CRUD (À DÉCIDER)
- ✅ Page 2: CRUD Clients (existe déjà, à réorganiser)

### 3.4 Module Produits

**US-4: En tant qu'utilisateur, je veux un module Produits complet avec sous-pages**

**Critères d'acceptation:**
- ⚠️ QStackedWidget interne nommé `stackedWidgetProduit` (À CRÉER)
- ⚠️ Page 0: Login Produit (optionnel, À CRÉER)
- ⚠️ Page 1: Menu ou accès direct au CRUD (À DÉCIDER)
- ✅ Page 2: CRUD Produits (existe déjà, à réorganiser)

### 3.5 Navigation et Slots

**US-5: En tant que développeur, je veux des slots auto-générés selon la convention Qt**

**Critères d'acceptation:**
- ✅ Utiliser la méthode `on_<objectName>_clicked()` (déjà fait)
- ✅ Slots créés via Qt Designer (déjà fait)
- ⚠️ Ajouter slots pour navigation entre stackedWidgets internes (À FAIRE)
- ⚠️ Slots pour retour au menu principal (À FAIRE)

### 3.6 Nommage des Objets

**US-6: En tant que développeur, je veux des noms d'objets significatifs**

**Critères d'acceptation:**
- ⚠️ Boutons: `btnAjouterEmploye`, `btnModifierClient`, `btnSupprimerProduit` (À RENOMMER)
- ⚠️ Tables: `tblEmploye`, `tblClient`, `tblProduit` (À RENOMMER)
- ⚠️ Champs texte: `txtNomEmploye`, `txtPrenomClient` (À VÉRIFIER dans dialogues)
- ⚠️ StackedWidgets: `stackedWidgetEmpl`, `stackedWidgetClient`, `stackedWidgetProduit` (À CRÉER)

## 4. État Actuel vs État Cible

### 4.1 Structure Actuelle

```
MainWindow
└── stackedWidget (principal)
    ├── Page 0: employeePage (CRUD direct)
    ├── Page 1: clientPage (CRUD direct)
    └── Page 2: productPage (CRUD direct)
```

### 4.2 Structure Cible (selon guide)

```
MainWindow
└── stackedWidget (principal)
    ├── Page 0: employeePage
    │   └── stackedWidgetEmpl
    │       ├── Page 0: Login Employé
    │       ├── Page 1: Menu Principal
    │       ├── Page 2: Mot de passe oublié (optionnel)
    │       └── Page 3: CRUD Employés
    │
    ├── Page 1: clientPage
    │   └── stackedWidgetClient
    │       ├── Page 0: Login Client (optionnel)
    │       └── Page 1: CRUD Clients
    │
    └── Page 2: productPage
        └── stackedWidgetProduit
            ├── Page 0: Login Produit (optionnel)
            └── Page 1: CRUD Produits
```

## 5. Modules Futurs

### 5.1 Modules à Ajouter

- Module Commandes
- Module Matières Premières
- Module Fournisseurs

**Critères pour chaque nouveau module:**
- Créer une nouvelle page dans stackedWidget principal
- Créer un stackedWidget interne pour le module
- Implémenter les sous-pages nécessaires
- Ajouter bouton de navigation dans sidebar
- Créer les slots de navigation

## 6. Bonnes Pratiques à Respecter

### 6.1 Nommage

- **Boutons**: `btn<Action><Module>` (ex: `btnAjouterEmploye`)
- **Tables**: `tbl<Module>` (ex: `tblEmploye`)
- **Champs texte**: `txt<Champ><Module>` (ex: `txtNomEmploye`)
- **ComboBox**: `cmb<Champ><Module>` (ex: `cmbStatutProduit`)
- **StackedWidget**: `stackedWidget<Module>` (ex: `stackedWidgetEmpl`)

### 6.2 Navigation

- Chaque module doit avoir un bouton "Retour" vers le menu principal
- Le menu principal doit avoir des boutons vers tous les modules
- Utiliser `ui->stackedWidget->setCurrentIndex(n)` pour navigation principale
- Utiliser `ui->stackedWidgetEmpl->setCurrentIndex(n)` pour navigation interne

### 6.3 Code

- Copier-coller le code existant des slots
- Adapter les noms d'objets si nécessaire
- Tester chaque page individuellement
- Tester la navigation complète

## 7. Plan de Migration

### Phase 1: Préparation (Analyse)
- ✅ Analyser la structure actuelle
- ✅ Créer ce document de spécification
- ⚠️ Identifier tous les widgets à renommer

### Phase 2: Restructuration UI
- ⚠️ Créer stackedWidgetEmpl dans employeePage
- ⚠️ Créer stackedWidgetClient dans clientPage
- ⚠️ Créer stackedWidgetProduit dans productPage
- ⚠️ Créer pages de login (optionnel)
- ⚠️ Créer page menu principal
- ⚠️ Déplacer CRUD existants vers bonnes pages

### Phase 3: Renommage
- ⚠️ Renommer tous les widgets selon convention
- ⚠️ Mettre à jour les références dans le code

### Phase 4: Navigation
- ⚠️ Créer slots pour navigation interne
- ⚠️ Créer slots pour retour menu
- ⚠️ Tester toutes les navigations

### Phase 5: Tests et Validation
- ⚠️ Tester chaque module individuellement
- ⚠️ Tester navigation complète
- ⚠️ Vérifier toutes les fonctionnalités CRUD
- ⚠️ Valider conformité avec le guide

## 8. Risques et Contraintes

### 8.1 Risques

- **Risque élevé**: Perte de fonctionnalités existantes lors de la restructuration
- **Risque moyen**: Erreurs de navigation entre stackedWidgets imbriqués
- **Risque faible**: Noms d'objets en conflit

### 8.2 Contraintes

- Maintenir toutes les fonctionnalités existantes
- Conserver le design visuel actuel (CUIREA theme)
- Respecter strictement le guide d'intégration
- Utiliser la méthode auto-slot (convention de nommage)

## 9. Questions à Résoudre

1. **Pages de login**: Faut-il créer des vraies pages de login fonctionnelles ou juste des pages de démonstration?
2. **Menu principal**: Doit-il être identique pour tous les modules ou spécifique à chaque module?
3. **Sidebar**: Doit-elle rester visible ou être cachée dans certaines pages (login)?
4. **Profile panel**: Doit-il être visible dans toutes les pages ou seulement dans CRUD Employés?

## 10. Prochaines Étapes

1. Valider cette spécification avec l'utilisateur
2. Répondre aux questions ci-dessus
3. Créer un document de design détaillé
4. Créer une liste de tâches d'implémentation
5. Commencer la restructuration par phases

---

**Note**: Ce document servira de base pour la restructuration complète du projet CUIREA selon le guide d'intégration Qt fourni.
