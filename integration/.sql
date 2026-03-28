/* =========================
   1. EMPLOYES
========================= */
CREATE TABLE Employes (
    id_employe        NUMBER PRIMARY KEY,
    matricule         VARCHAR2(50) UNIQUE NOT NULL,
    nom               VARCHAR2(100) NOT NULL,
    prenom            VARCHAR2(100) NOT NULL,
    cin               VARCHAR2(20) UNIQUE NOT NULL,
    date_naissance    DATE,
    sexe              VARCHAR2(10),
    adresse           VARCHAR2(255),
    telephone         VARCHAR2(20),
    email             VARCHAR2(100),
    poste             VARCHAR2(100),
    specialite        VARCHAR2(100),
    departement       VARCHAR2(100),
    date_embauche     DATE,
    photo_url         VARCHAR2(255),
    role              VARCHAR2(50)
);

/* =========================
   2. COMMANDES
========================= */
CREATE TABLE Commandes (
    id_commande      NUMBER PRIMARY KEY,
    reference        VARCHAR2(50) UNIQUE NOT NULL,
    type             VARCHAR2(50),
    date_creation    DATE,
    date_livraison   DATE,
    statut           VARCHAR2(50),
    priorite         VARCHAR2(50),
    montant          NUMBER(10,2),
    etat             VARCHAR2(50),
    id_employe       NUMBER,

    CONSTRAINT fk_commandes_employe
        FOREIGN KEY (id_employe)
        REFERENCES Employes(id_employe)
);

/* =========================
   3. ARTICLES
========================= */
CREATE TABLE Articles (
    id_article         NUMBER PRIMARY KEY,
    reference          VARCHAR2(50) UNIQUE NOT NULL,
    nom                VARCHAR2(100),
    categorie          VARCHAR2(100),
    type               VARCHAR2(50),
    couleur            VARCHAR2(50),
    dimension          VARCHAR2(50),
    prix_unitaire      NUMBER(10,2),
    cout_fabrication   NUMBER(10,2),
    statut             VARCHAR2(50),
    date_creation      DATE,
    id_commande        NUMBER,

    CONSTRAINT fk_articles_commandes
        FOREIGN KEY (id_commande)
        REFERENCES Commandes(id_commande)
);

/* =========================
   4. FOURNISSEURS
========================= */
CREATE TABLE "CUIREA"."FOURNISSEURS" (
    "ID_FOURNISSEUR" NUMBER,
    "NOM_ENTREPRISE" VARCHAR2(150 BYTE),
    "EMAIL" VARCHAR2(100 BYTE),
    "TELEPHONE" VARCHAR2(20 BYTE),
    "TYPE_PRODUIT" VARCHAR2(100 BYTE),
    "CONDITION_PAIEMENT" VARCHAR2(100 BYTE),
    "MATRICULE_FISCAL" VARCHAR2(50 BYTE),
    "STATUT" VARCHAR2(50 BYTE),
    PRIMARY KEY ("ID_FOURNISSEUR")
    USING INDEX PCTFREE 10 INITRANS 2 MAXTRANS 255 COMPUTE STATISTICS
    STORAGE(INITIAL 65536 NEXT 1048576 MINEXTENTS 1 MAXEXTENTS 2147483645
    PCTINCREASE 0 FREELISTS 1 FREELIST GROUPS 1 BUFFER_POOL DEFAULT FLASH_CACHE DEFAULT CELL_FLASH_CACHE DEFAULT)
    TABLESPACE "SYSTEM" ENABLE
) SEGMENT CREATION IMMEDIATE
PCTFREE 10 PCTUSED 40 INITRANS 1 MAXTRANS 255 NOCOMPRESS LOGGING
STORAGE(INITIAL 65536 NEXT 1048576 MINEXTENTS 1 MAXEXTENTS 2147483645
PCTINCREASE 0 FREELISTS 1 FREELIST GROUPS 1 BUFFER_POOL DEFAULT FLASH_CACHE DEFAULT CELL_FLASH_CACHE DEFAULT)
TABLESPACE "SYSTEM";

CREATE SEQUENCE seq_fournisseur
    MINVALUE 1
    MAXVALUE 999999999999999999999999999
    START WITH 1
    INCREMENT BY 1
    NOCACHE
    NOORDER
    NOCYCLE;

CREATE OR REPLACE TRIGGER "CUIREA"."TRG_FOURNISSEUR_ID"
BEFORE INSERT ON Fournisseurs
FOR EACH ROW
BEGIN
    IF :NEW.id_fournisseur IS NULL THEN
        SELECT seq_fournisseur.NEXTVAL INTO :NEW.id_fournisseur FROM DUAL;
    END IF;
END;
/

ALTER TRIGGER "CUIREA"."TRG_FOURNISSEUR_ID" ENABLE;

/* =========================
   5. MATIERES PREMIERES
========================= */

-- Créer la séquence
CREATE SEQUENCE seq_matieres
    MINVALUE 1
    MAXVALUE 999999999999999999999999999
    START WITH 1
    INCREMENT BY 1
    NOCACHE
    NOORDER
    NOCYCLE;

-- Créer la table MATIERES_PREMIERES
CREATE TABLE "CUIREA"."MATIERES_PREMIERES" (
    "ID_MATIERE" NUMBER,
    "NOM" VARCHAR2(100 BYTE),
    "TYPE_MATIERE" VARCHAR2(100 BYTE),
    "QUANTITE_ACTUELLE" NUMBER,
    "SEUIL" NUMBER,
    "DATE_EXPIRATION" DATE,
    "REFERENCE" VARCHAR2(50 BYTE),
    "PHOTO_URL" VARCHAR2(255 BYTE),
    "ID_FOURNISSEUR" NUMBER,
    PRIMARY KEY ("ID_MATIERE")
    USING INDEX PCTFREE 10 INITRANS 2 MAXTRANS 255 COMPUTE STATISTICS
    STORAGE(INITIAL 65536 NEXT 1048576 MINEXTENTS 1 MAXEXTENTS 2147483645
    PCTINCREASE 0 FREELISTS 1 FREELIST GROUPS 1 BUFFER_POOL DEFAULT FLASH_CACHE DEFAULT CELL_FLASH_CACHE DEFAULT)
    TABLESPACE "SYSTEM" ENABLE,
    CONSTRAINT fk_matieres_fournisseur 
        FOREIGN KEY ("ID_FOURNISSEUR") 
        REFERENCES "CUIREA"."FOURNISSEURS"("ID_FOURNISSEUR")
) SEGMENT CREATION IMMEDIATE
PCTFREE 10 PCTUSED 40 INITRANS 1 MAXTRANS 255 NOCOMPRESS LOGGING
STORAGE(INITIAL 65536 NEXT 1048576 MINEXTENTS 1 MAXEXTENTS 2147483645
PCTINCREASE 0 FREELISTS 1 FREELIST GROUPS 1 BUFFER_POOL DEFAULT FLASH_CACHE DEFAULT CELL_FLASH_CACHE DEFAULT)
TABLESPACE "SYSTEM";

-- Créer le trigger pour auto-incrément
CREATE OR REPLACE TRIGGER "CUIREA"."TRG_MATIERE_ID"
BEFORE INSERT ON MATIERES_PREMIERES
FOR EACH ROW
BEGIN
    IF :NEW.id_matiere IS NULL THEN
        SELECT seq_matieres.NEXTVAL INTO :NEW.id_matiere FROM DUAL;
    END IF;
END;
/

ALTER TRIGGER "CUIREA"."TRG_MATIERE_ID" ENABLE;

-- Insérer des données de test (optionnel)
INSERT INTO MATIERES_PREMIERES (nom, type_matiere, quantite_actuelle, seuil, date_expiration, reference, id_fournisseur)
VALUES ('Cuir Pleine Fleur', 'Cuir', 150, 50, TO_DATE('2027-12-31', 'YYYY-MM-DD'), 'REF-001', 1);

INSERT INTO MATIERES_PREMIERES (nom, type_matiere, quantite_actuelle, seuil, date_expiration, reference, id_fournisseur)
VALUES ('Peau de Veau Premium', 'Peau de Veau', 80, 30, TO_DATE('2027-06-30', 'YYYY-MM-DD'), 'REF-002', 1);

INSERT INTO MATIERES_PREMIERES (nom, type_matiere, quantite_actuelle, seuil, date_expiration, reference, id_fournisseur)
VALUES ('Fil de Couture', 'Ficelinée', 200, 100, TO_DATE('2028-12-31', 'YYYY-MM-DD'), 'REF-003', 1);

COMMIT;

-- Si la table existe déjà sans id_fournisseur, ajouter la colonne:
-- ALTER TABLE Matieres_premieres ADD id_fournisseur NUMBER;
-- ALTER TABLE Matieres_premieres ADD CONSTRAINT fk_matieres_fournisseur FOREIGN KEY (id_fournisseur) REFERENCES Fournisseurs(id_fournisseur);
/* =========================
   6. CLIENTS
========================= */

CREATE TABLE Clients (
    id_client          NUMBER PRIMARY KEY,
    prenom             VARCHAR2(100),
    nom                VARCHAR2(100),
    sexe               VARCHAR2(10),
    cin                VARCHAR2(20) UNIQUE,
    pays               VARCHAR2(100),
    ville              VARCHAR2(100),
    adresse            VARCHAR2(255),
    telephone           VARCHAR2(20),
    email              VARCHAR2(100),
    date_inscription   DATE,
    id_employe         NUMBER,
    CONSTRAINT fk_clients_employe
        FOREIGN KEY (id_employe)
        REFERENCES Employes(id_employe)
);

/* =========================
   7. FABRIQUER
   (Article ↔ Matière)
========================= */
CREATE TABLE Fabriquer (
    id_matiere   NUMBER,
    id_article   NUMBER,
    CONSTRAINT pk_fabriquer
        PRIMARY KEY (id_matiere, id_article),
    CONSTRAINT fk_fabriquer_matiere
        FOREIGN KEY (id_matiere)
        REFERENCES Matieres_premieres(id_matiere),
    CONSTRAINT fk_fabriquer_article
        FOREIGN KEY (id_article)
        REFERENCES Articles(id_article)
);

/* =========================
   8. FOURNIR
   (Fournisseur ↔ Matière)
========================= */
CREATE TABLE "CUIREA"."FOURNIR" (
    "ID_FOURNISSEUR" NUMBER,
    "ID_MATIERE" NUMBER,
    CONSTRAINT "PK_FOURNIR" PRIMARY KEY ("ID_FOURNISSEUR", "ID_MATIERE")
    USING INDEX PCTFREE 10 INITRANS 2 MAXTRANS 255 COMPUTE STATISTICS
    STORAGE(INITIAL 65536 NEXT 1048576 MINEXTENTS 1 MAXEXTENTS 2147483645
    PCTINCREASE 0 FREELISTS 1 FREELIST GROUPS 1 BUFFER_POOL DEFAULT FLASH_CACHE DEFAULT CELL_FLASH_CACHE DEFAULT)
    TABLESPACE "SYSTEM" ENABLE,
    CONSTRAINT "FK_FOURNIR_FOURNISSEUR" FOREIGN KEY ("ID_FOURNISSEUR")
        REFERENCES "CUIREA"."FOURNISSEURS" ("ID_FOURNISSEUR") ENABLE,
    CONSTRAINT "FK_FOURNIR_MATIERE" FOREIGN KEY ("ID_MATIERE")
        REFERENCES "CUIREA"."MATIERES_PREMIERES" ("ID_MATIERE") ENABLE
) SEGMENT CREATION IMMEDIATE
PCTFREE 10 PCTUSED 40 INITRANS 1 MAXTRANS 255 NOCOMPRESS LOGGING
STORAGE(INITIAL 65536 NEXT 1048576 MINEXTENTS 1 MAXEXTENTS 2147483645
PCTINCREASE 0 FREELISTS 1 FREELIST GROUPS 1 BUFFER_POOL DEFAULT FLASH_CACHE DEFAULT CELL_FLASH_CACHE DEFAULT)
TABLESPACE "SYSTEM";