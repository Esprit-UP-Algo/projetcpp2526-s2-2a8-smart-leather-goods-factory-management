-- ============================================================
-- CONFIGURATION DE LA TABLE FOURNISSEURS
-- Exécute ce script UNE SEULE FOIS dans SQL Developer
-- ============================================================

-- 1. Créer la séquence pour l'auto-incrémentation de l'ID
CREATE SEQUENCE FOURNISSEURS_SEQ
START WITH 1
INCREMENT BY 1
NOCACHE
NOCYCLE;

-- 2. Créer le trigger pour l'auto-incrémentation de l'ID
CREATE OR REPLACE TRIGGER FOURNISSEURS_TRG
BEFORE INSERT ON FOURNISSEURS
FOR EACH ROW
BEGIN
    IF :NEW.ID_FOURNISSEUR IS NULL THEN
        SELECT FOURNISSEURS_SEQ.NEXTVAL INTO :NEW.ID_FOURNISSEUR FROM DUAL;
    END IF;
END;
/

-- 3. Vérifier que la table est vide
SELECT COUNT(*) AS "Nombre de fournisseurs" FROM FOURNISSEURS;

-- 4. Test: Insérer un fournisseur de test
INSERT INTO FOURNISSEURS (NOM_ENTREPRISE, EMAIL, TELEPHONE, MATRICULE_FISCAL, TYPE_PRODUIT_FOURNIS, CONDITION_PAIEMENT, STATUT)
VALUES ('Test Company', 'test@company.com', '0612345678', 'MF/TEST/001', 'Cuir', 'Virement', 'Actif');

COMMIT;

-- 5. Vérifier l'insertion
SELECT * FROM FOURNISSEURS ORDER BY ID_FOURNISSEUR DESC;

-- ============================================================
-- Si tout fonctionne, tu verras le fournisseur de test avec ID = 1
-- Tu peux maintenant utiliser ton application Qt pour ajouter des fournisseurs
-- ============================================================
