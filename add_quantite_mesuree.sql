-- Ajouter la colonne quantite_mesuree dans la table Fournisseurs
ALTER TABLE Fournisseurs ADD quantite_mesuree NUMBER(10,3) DEFAULT 0;

-- Vérifier la structure
DESC Fournisseurs;
