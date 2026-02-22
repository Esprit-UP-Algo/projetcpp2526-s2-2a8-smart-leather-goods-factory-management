-- ============================================================
-- SCRIPT DE MONITORING FOURNISSEURS
-- Garde ce fichier ouvert et appuie sur F9 après chaque action
-- ============================================================

-- 🔍 VUE RAPIDE: Les 10 derniers fournisseurs
SELECT 
    ID_FOURNISSEUR AS "ID",
    NOM_ENTREPRISE AS "Entreprise",
    EMAIL,
    TELEPHONE AS "Tél",
    TYPE_PRODUIT_FOURNIS AS "Type Produit",
    STATUT
FROM FOURNISSEURS 
ORDER BY ID_FOURNISSEUR DESC 
FETCH FIRST 10 ROWS ONLY;

-- 📊 STATISTIQUES RAPIDES
SELECT 
    COUNT(*) AS "Total Fournisseurs",
    SUM(CASE WHEN STATUT = 'Actif' THEN 1 ELSE 0 END) AS "Actifs",
    SUM(CASE WHEN STATUT = 'Suspendu' THEN 1 ELSE 0 END) AS "Suspendus",
    SUM(CASE WHEN STATUT = 'Inactif' THEN 1 ELSE 0 END) AS "Inactifs"
FROM FOURNISSEURS;

-- ============================================================
-- REQUÊTES DÉTAILLÉES (décommenter si besoin)
-- ============================================================

-- 1. Afficher TOUS les fournisseurs
-- SELECT * FROM FOURNISSEURS ORDER BY ID_FOURNISSEUR DESC;

-- 2. Rechercher un fournisseur par nom
-- SELECT * FROM FOURNISSEURS WHERE UPPER(NOM_ENTREPRISE) LIKE UPPER('%nom%');

-- 3. Statistiques par type de produit
-- SELECT TYPE_PRODUIT_FOURNIS, COUNT(*) AS "Nombre" 
-- FROM FOURNISSEURS 
-- GROUP BY TYPE_PRODUIT_FOURNIS
-- ORDER BY COUNT(*) DESC;

-- 4. Vérifier les doublons de matricule fiscal
-- SELECT MATRICULE_FISCAL, COUNT(*) AS "Nombre"
-- FROM FOURNISSEURS
-- GROUP BY MATRICULE_FISCAL
-- HAVING COUNT(*) > 1;
