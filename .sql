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
    statut            VARCHAR2(50) DEFAULT 'ACTIF',
    mot_de_passe      VARCHAR2(255),
    role_systeme      VARCHAR2(50) DEFAULT 'EMPLOYE',
    actif             NUMBER(1) DEFAULT 1,
    derniere_connexion DATE
);

/* =========================
   2. COMMANDES
========================= */
CREATE TABLE Commandes (
    id_commande      NUMBER PRIMARY KEY,
    reference        VARCHAR2(50) UNIQUE NOT NULL,
    produit          VARCHAR2(50),
    date_creation    DATE,
    date_livraison   DATE,
    statut           VARCHAR2(50),
    priorite         VARCHAR2(50),
    montant          NUMBER(10,2),
    etat             VARCHAR2(50),
    id_employe       NUMBER,
    mail_client      VARCHAR2(100),

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

    modele_3d          VARCHAR2(100),

    couleur_r          NUMBER,
    couleur_g          NUMBER,
    couleur_b          NUMBER,

    largeur            NUMBER,
    hauteur            NUMBER,
    profondeur         NUMBER,

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
CREATE TABLE Fournisseurs (
    id_fournisseur      NUMBER PRIMARY KEY,
    nom_entreprise      VARCHAR2(150),
    email               VARCHAR2(100),
    telephone           VARCHAR2(20),
    type_produit        VARCHAR2(100),
    condition_paiement  VARCHAR2(100),
    matricule_fiscal    VARCHAR2(50),
    statut              VARCHAR2(50),
    adresse             VARCHAR2(255)
);

/* =========================
   5. MATIERES PREMIERES
========================= */
CREATE TABLE Matieres_premieres (
    id_matiere          NUMBER PRIMARY KEY,
    nom                 VARCHAR2(100),
    type_matiere        VARCHAR2(100),
    quantite_actuelle   NUMBER,
    seuil               NUMBER,
    date_expiration     DATE,
    reference           VARCHAR2(50),
    photo_url           VARCHAR2(255)
);
ALTER TABLE Matieres_premieres ADD photo_url VARCHAR2(255);
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
CREATE TABLE Fournir (
    id_fournisseur   NUMBER,
    id_matiere       NUMBER,
    CONSTRAINT pk_fournir
        PRIMARY KEY (id_fournisseur, id_matiere),
    CONSTRAINT fk_fournir_fournisseur
        FOREIGN KEY (id_fournisseur)
        REFERENCES Fournisseurs(id_fournisseur),
    CONSTRAINT fk_fournir_matiere
        FOREIGN KEY (id_matiere)
        REFERENCES Matieres_premieres(id_matiere)
);

/* =========================
   9. ARDUINO_TEMP_READINGS
   Historique des lectures de température du stock cuir
========================= */
CREATE TABLE ARDUINO_TEMP_READINGS (
    id_reading    NUMBER PRIMARY KEY,
    temperature   NUMBER(5,2)   NOT NULL,
    timestamp     TIMESTAMP     DEFAULT CURRENT_TIMESTAMP,
    is_alert      NUMBER(1)     DEFAULT 0
);

CREATE SEQUENCE seq_temp_readings START WITH 1 INCREMENT BY 1;

CREATE OR REPLACE TRIGGER trg_temp_readings_id
BEFORE INSERT ON ARDUINO_TEMP_READINGS
FOR EACH ROW
BEGIN
    IF :NEW.id_reading IS NULL THEN
        SELECT seq_temp_readings.NEXTVAL INTO :NEW.id_reading FROM DUAL;
    END IF;
END;
/

/* =========================
   10. ARDUINO_DELIVERIES
   Historique des livraisons validées par balance Arduino
========================= */
CREATE TABLE ARDUINO_DELIVERIES (
    id_delivery       NUMBER PRIMARY KEY,
    id_fournisseur    NUMBER,
    id_matiere        NUMBER,
    ordered_qty       NUMBER(10,3),
    measured_weight   NUMBER(10,3),
    validated         NUMBER(1)     DEFAULT 0,
    operator_note     VARCHAR2(500),
    timestamp         TIMESTAMP     DEFAULT CURRENT_TIMESTAMP
);

ALTER TABLE ARDUINO_DELIVERIES ADD CONSTRAINT fk_delivery_fournisseur
    FOREIGN KEY (id_fournisseur) REFERENCES Fournisseurs(id_fournisseur);

ALTER TABLE ARDUINO_DELIVERIES ADD CONSTRAINT fk_delivery_matiere
    FOREIGN KEY (id_matiere) REFERENCES Matieres_premieres(id_matiere);

CREATE SEQUENCE seq_deliveries START WITH 1 INCREMENT BY 1;

CREATE OR REPLACE TRIGGER trg_deliveries_id
BEFORE INSERT ON ARDUINO_DELIVERIES
FOR EACH ROW
BEGIN
    IF :NEW.id_delivery IS NULL THEN
        SELECT seq_deliveries.NEXTVAL INTO :NEW.id_delivery FROM DUAL;
    END IF;
END;
/
