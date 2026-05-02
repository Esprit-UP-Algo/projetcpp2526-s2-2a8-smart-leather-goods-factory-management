-- Migration pour ajouter les colonnes de livraison dans la table FOURNISSEURS
-- Date: 2026-05-02
-- Description: Ajout des colonnes pour stocker les quantités commandées et mesurées

-- Vérifier si les colonnes existent déjà avant de les ajouter
BEGIN
    -- Ajouter QTE_COMMANDEE si elle n'existe pas
    BEGIN
        EXECUTE IMMEDIATE 'ALTER TABLE FOURNISSEURS ADD QTE_COMMANDEE NUMBER(10,2) DEFAULT 0';
        DBMS_OUTPUT.PUT_LINE('Colonne QTE_COMMANDEE ajoutée avec succès');
    EXCEPTION
        WHEN OTHERS THEN
            IF SQLCODE = -1430 THEN
                DBMS_OUTPUT.PUT_LINE('Colonne QTE_COMMANDEE existe déjà');
            ELSE
                RAISE;
            END IF;
    END;
    
    -- Ajouter QTE_MESUREE si elle n'existe pas
    BEGIN
        EXECUTE IMMEDIATE 'ALTER TABLE FOURNISSEURS ADD QTE_MESUREE NUMBER(10,2) DEFAULT 0';
        DBMS_OUTPUT.PUT_LINE('Colonne QTE_MESUREE ajoutée avec succès');
    EXCEPTION
        WHEN OTHERS THEN
            IF SQLCODE = -1430 THEN
                DBMS_OUTPUT.PUT_LINE('Colonne QTE_MESUREE existe déjà');
            ELSE
                RAISE;
            END IF;
    END;
    
    -- Ajouter DATE_DERNIERE_LIVRAISON si elle n'existe pas
    BEGIN
        EXECUTE IMMEDIATE 'ALTER TABLE FOURNISSEURS ADD DATE_DERNIERE_LIVRAISON DATE';
        DBMS_OUTPUT.PUT_LINE('Colonne DATE_DERNIERE_LIVRAISON ajoutée avec succès');
    EXCEPTION
        WHEN OTHERS THEN
            IF SQLCODE = -1430 THEN
                DBMS_OUTPUT.PUT_LINE('Colonne DATE_DERNIERE_LIVRAISON existe déjà');
            ELSE
                RAISE;
            END IF;
    END;
    
    COMMIT;
END;
/

-- Afficher la structure de la table après modification
DESC FOURNISSEURS;

-- Afficher un message de confirmation
SELECT 'Migration terminée avec succès!' AS STATUS FROM DUAL;
