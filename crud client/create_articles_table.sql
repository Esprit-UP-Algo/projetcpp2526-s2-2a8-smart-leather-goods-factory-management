-- Script de création de la table ARTICLES pour Oracle

-- Supprimer la table si elle existe déjà
DROP TABLE ARTICLES CASCADE CONSTRAINTS;

-- Créer la séquence pour l'auto-incrémentation de l'ID
DROP SEQUENCE seq_articles;
CREATE SEQUENCE seq_articles START WITH 1 INCREMENT BY 1;

-- Créer la table ARTICLES
CREATE TABLE ARTICLES (
    ID_ARTICLE          NUMBER PRIMARY KEY,
    REFERENCE           VARCHAR2(50) NOT NULL UNIQUE,
    NOM                 VARCHAR2(100) NOT NULL,
    CATEGORIE           VARCHAR2(100) NOT NULL,
    TYPE                VARCHAR2(100),
    COULEUR             VARCHAR2(50),
    DIMENSION           VARCHAR2(50),
    PRIX_UNITAIRE       NUMBER(10,2) NOT NULL CHECK (PRIX_UNITAIRE > 0),
    COUT_FABRICATION    NUMBER(10,2) NOT NULL CHECK (COUT_FABRICATION >= 0),
    STATUT              VARCHAR2(50) DEFAULT 'disponible' CHECK (STATUT IN ('disponible', 'en_production', 'obsolete')),
    DATE_CREATION       DATE DEFAULT SYSDATE NOT NULL
);

-- Créer un trigger pour l'auto-incrémentation
CREATE OR REPLACE TRIGGER trg_articles_id
BEFORE INSERT ON ARTICLES
FOR EACH ROW
BEGIN
    IF :NEW.ID_ARTICLE IS NULL THEN
        SELECT seq_articles.NEXTVAL INTO :NEW.ID_ARTICLE FROM DUAL;
    END IF;
END;
/

-- Insérer quelques données de test
INSERT INTO ARTICLES (REFERENCE, NOM, CATEGORIE, TYPE, COULEUR, DIMENSION, PRIX_UNITAIRE, COUT_FABRICATION, STATUT)
VALUES ('ART-2024-0001', 'Sac à Main Classique', 'Sacs', 'Sac à main', 'Noir', '30x25x10cm', 89.99, 45.00, 'disponible');

INSERT INTO ARTICLES (REFERENCE, NOM, CATEGORIE, TYPE, COULEUR, DIMENSION, PRIX_UNITAIRE, COUT_FABRICATION, STATUT)
VALUES ('ART-2024-0002', 'Portefeuille Cuir Premium', 'Portefeuilles', 'Portefeuille long', 'Marron', '19x10x2cm', 45.50, 22.00, 'disponible');

INSERT INTO ARTICLES (REFERENCE, NOM, CATEGORIE, TYPE, COULEUR, DIMENSION, PRIX_UNITAIRE, COUT_FABRICATION, STATUT)
VALUES ('ART-2024-0003', 'Ceinture Homme Élégante', 'Ceintures', 'Ceinture classique', 'Noir', '110x3.5cm', 35.00, 18.00, 'disponible');

INSERT INTO ARTICLES (REFERENCE, NOM, CATEGORIE, TYPE, COULEUR, DIMENSION, PRIX_UNITAIRE, COUT_FABRICATION, STATUT)
VALUES ('ART-2024-0004', 'Sac Bandoulière Femme', 'Sacs', 'Sac bandoulière', 'Beige', '28x20x8cm', 75.00, 38.00, 'en_production');

INSERT INTO ARTICLES (REFERENCE, NOM, CATEGORIE, TYPE, COULEUR, DIMENSION, PRIX_UNITAIRE, COUT_FABRICATION, STATUT)
VALUES ('ART-2024-0005', 'Porte-Cartes Compact', 'Accessoires', 'Porte-cartes', 'Bleu Marine', '11x7x1cm', 25.00, 12.00, 'disponible');

COMMIT;

-- Vérifier les données
SELECT * FROM ARTICLES;
