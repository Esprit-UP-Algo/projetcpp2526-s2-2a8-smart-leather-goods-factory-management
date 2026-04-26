-- ============================================================
-- SCRIPT SQL POUR LE SYSTEME DE POINTAGE RFID - CUIREA
-- ============================================================
-- Ce script ajoute les elements necessaires pour le systeme
-- de pointage par carte RFID.
-- 
-- A executer dans Oracle SQL Developer ou SQL*Plus
-- ============================================================

-- ============================================================
-- 1. AJOUTER LA COLONNE UID_CARTE A LA TABLE EMPLOYES
-- ============================================================
-- Cette colonne stocke l'identifiant unique de la carte RFID
-- de chaque employe (format hexadecimal, ex: A1B2C3D4)

ALTER TABLE EMPLOYES ADD (UID_CARTE VARCHAR2(20));

-- Si erreur "column already exists", c'est normal, continuer.

-- ============================================================
-- 2. CREER LA TABLE POINTAGE
-- ============================================================
-- Cette table enregistre tous les pointages des employes
-- (entrees et sorties)

CREATE TABLE POINTAGE (
    ID_POINTAGE      NUMBER PRIMARY KEY,
    ID_EMPLOYE       NUMBER NOT NULL,
    DATE_POINTAGE    DATE NOT NULL,
    HEURE_ARRIVEE    TIMESTAMP,
    STATUT           VARCHAR2(20),
    CONSTRAINT fk_pointage_employe
        FOREIGN KEY (ID_EMPLOYE)
        REFERENCES EMPLOYES(ID_EMPLOYE)
);

-- Si erreur "name is already used", la table existe deja, continuer.

-- ============================================================
-- 3. CREER LA SEQUENCE POUR AUTO-INCREMENT
-- ============================================================

CREATE SEQUENCE SEQ_POINTAGE START WITH 1 INCREMENT BY 1;

-- Si erreur "name is already used", la sequence existe deja, continuer.

-- ============================================================
-- 4. CREER LE TRIGGER POUR AUTO-INCREMENT
-- ============================================================

CREATE OR REPLACE TRIGGER TRG_POINTAGE_ID
BEFORE INSERT ON POINTAGE
FOR EACH ROW
BEGIN
    IF :NEW.ID_POINTAGE IS NULL THEN
        SELECT SEQ_POINTAGE.NEXTVAL INTO :NEW.ID_POINTAGE FROM DUAL;
    END IF;
END;
/

-- ============================================================
-- 5. EXEMPLE : ASSIGNER UN UID A UN EMPLOYE
-- ============================================================
-- Remplacez ID_EMPLOYE par l'ID de l'employe souhaite
-- Remplacez 'XXXXXXXX' par l'UID de la carte RFID

-- UPDATE EMPLOYES SET UID_CARTE = 'A1B2C3D4' WHERE ID_EMPLOYE = 1;

-- ============================================================
-- 6. VALIDER LES MODIFICATIONS
-- ============================================================

COMMIT;

-- ============================================================
-- VERIFICATION : Afficher la structure
-- ============================================================

-- Verifier que la colonne UID_CARTE existe :
-- SELECT COLUMN_NAME FROM USER_TAB_COLUMNS WHERE TABLE_NAME = 'EMPLOYES' AND COLUMN_NAME = 'UID_CARTE';

-- Verifier que la table POINTAGE existe :
-- SELECT TABLE_NAME FROM USER_TABLES WHERE TABLE_NAME = 'POINTAGE';

-- Voir les employes avec leur UID :
-- SELECT ID_EMPLOYE, NOM, PRENOM, UID_CARTE, ACTIF FROM EMPLOYES;

-- ============================================================
-- FIN DU SCRIPT
-- ============================================================
