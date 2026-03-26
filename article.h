#ifndef ARTICLE_H
#define ARTICLE_H

#include <QString>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

class Article
{
public:
    // Constructeurs
    Article();
    Article(int id, const QString &ref, const QString &nom, const QString &cat,
            const QString &type, const QString &couleur, const QString &dim,
            double prix, double cout, const QString &statut, const QDate &date);
    
    // Getters
    int getIdArticle() const { return idArticle; }
    QString getReference() const { return reference; }
    QString getNom() const { return nom; }
    QString getCategorie() const { return categorie; }
    QString getType() const { return type; }
    QString getCouleur() const { return couleur; }
    QString getDimensions() const { return dimensions; }
    double getPrixUnitaire() const { return prixUnitaire; }
    double getCoutFabrication() const { return coutFabrication; }
    QString getStatut() const { return statut; }
    QDate getDateCreation() const { return dateCreation; }
    
    // Setters
    void setIdArticle(int id) { idArticle = id; }
    void setReference(const QString &ref) { reference = ref; }
    void setNom(const QString &n) { nom = n; }
    void setCategorie(const QString &cat) { categorie = cat; }
    void setType(const QString &t) { type = t; }
    void setCouleur(const QString &c) { couleur = c; }
    void setDimensions(const QString &dim) { dimensions = dim; }
    void setPrixUnitaire(double prix) { prixUnitaire = prix; }
    void setCoutFabrication(double cout) { coutFabrication = cout; }
    void setStatut(const QString &s) { statut = s; }
    void setDateCreation(const QDate &date) { dateCreation = date; }
    
    // Méthodes CRUD (Modèle) - Requêtes préparées
    bool ajouter();
    bool modifier();
    bool supprimer();
    static QList<Article> afficher();
    static Article rechercherParId(int id);
    static QList<Article> rechercherParReference(const QString &ref);
    static QList<Article> rechercherParNom(const QString &nom);
    static QList<Article> rechercherParCategorie(const QString &cat);
    static QList<Article> rechercherParStatut(const QString &statut);
    
    // Méthodes de validation
    bool valider(QString &erreur) const;
    static bool referenceExiste(const QString &ref, int excludeId = -1);
    
    // Prédiction de prix
    static double predirePrix(const QString &categorie, const QString &type,
                               const QString &couleur, double coutFabrication);
    static QMap<QString, double> getMoyennesPrixParCategorie();
    
    // Prédiction avancée - retourne un résultat détaillé
    struct PredictionResult {
        double prixPredit;        // Prix prédit final (moyenne pondérée)
        double prixMin;           // Borne basse (intervalle de confiance)
        double prixMax;           // Borne haute
        double prixCategorie;     // Prédiction par catégorie seule
        double prixType;          // Prédiction par type seul
        double prixCouleur;       // Prédiction par couleur seule
        double prixKNN;           // Prédiction k-NN (3 articles les plus proches)
        double margeEstimee;      // Marge estimée en %
        int    nbArticlesRef;     // Nombre d'articles de référence utilisés
        QString niveauConfiance;  // "Élevé", "Moyen", "Faible"
        QString recommandation;   // Conseil stratégique
        QList<QPair<QString,double>> articlesProches; // k-NN voisins
    };
    static PredictionResult predirePrixAvance(const QString &categorie, const QString &type,
                                               const QString &couleur, double coutFabrication);

private:
    int idArticle;
    QString reference;
    QString nom;
    QString categorie;
    QString type;
    QString couleur;
    QString dimensions;
    double prixUnitaire;
    double coutFabrication;
    QString statut;
    QDate dateCreation;
};

#endif // ARTICLE_H
