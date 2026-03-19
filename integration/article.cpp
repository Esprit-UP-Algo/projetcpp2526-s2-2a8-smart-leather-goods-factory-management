#include "article.h"
#include "connection.h"
#include <QDebug>
#include <QSqlDatabase>

// Constructeur par défaut
Article::Article()
    : idArticle(0)
    , prixUnitaire(0.0)
    , coutFabrication(0.0)
    , statut("disponible")
    , dateCreation(QDate::currentDate())
{
}

// Constructeur avec paramètres
Article::Article(int id, const QString &ref, const QString &n, const QString &cat,
                 const QString &t, const QString &c, const QString &dim,
                 double prix, double cout, const QString &s, const QDate &date)
    : idArticle(id)
    , reference(ref)
    , nom(n)
    , categorie(cat)
    , type(t)
    , couleur(c)
    , dimensions(dim)
    , prixUnitaire(prix)
    , coutFabrication(cout)
    , statut(s)
    , dateCreation(date)
{
}

// Validation des données
bool Article::valider(QString &erreur) const
{
    if (reference.trimmed().isEmpty()) {
        erreur = "La référence est obligatoire";
        return false;
    }
    if (nom.trimmed().isEmpty()) {
        erreur = "Le nom est obligatoire";
        return false;
    }
    if (categorie.trimmed().isEmpty()) {
        erreur = "La catégorie est obligatoire";
        return false;
    }
    if (prixUnitaire <= 0) {
        erreur = "Le prix unitaire doit être supérieur à 0";
        return false;
    }
    if (coutFabrication < 0) {
        erreur = "Le coût de fabrication ne peut pas être négatif";
        return false;
    }
    if (prixUnitaire < coutFabrication) {
        erreur = "Le prix de vente ne peut pas être inférieur au coût de fabrication";
        return false;
    }
    return true;
}

// Vérifier si une référence existe déjà
bool Article::referenceExiste(const QString &ref, int excludeId)
{
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM ARTICLES WHERE REFERENCE = :ref AND ID_ARTICLE != :id");
    query.bindValue(":ref", ref);
    query.bindValue(":id", excludeId);
    
    if (query.exec() && query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

// AJOUTER un article (Requête préparée)
bool Article::ajouter()
{
    QString erreur;
    if (!valider(erreur)) {
        qDebug() << "Validation échouée:" << erreur;
        return false;
    }
    
    if (referenceExiste(reference)) {
        qDebug() << "La référence existe déjà";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("INSERT INTO ARTICLES (REFERENCE, NOM, CATEGORIE, TYPE, COULEUR, "
                  "DIMENSION, PRIX_UNITAIRE, COUT_FABRICATION, STATUT, DATE_CREATION) "
                  "VALUES (:ref, :nom, :cat, :type, :couleur, :dim, :prix, :cout, :statut, :date)");
    
    query.bindValue(":ref", reference);
    query.bindValue(":nom", nom);
    query.bindValue(":cat", categorie);
    query.bindValue(":type", type);
    query.bindValue(":couleur", couleur);
    query.bindValue(":dim", dimensions);
    query.bindValue(":prix", prixUnitaire);
    query.bindValue(":cout", coutFabrication);
    query.bindValue(":statut", statut);
    query.bindValue(":date", dateCreation);
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur ajout article:" << query.lastError().text();
        qDebug() << "   Driver:" << query.lastError().driverText();
        qDebug() << "   Database:" << query.lastError().databaseText();
        return false;
    }
    
    // Pour Oracle, récupérer l'ID via RETURNING INTO ou une requête séparée
    QSqlQuery idQuery;
    idQuery.prepare("SELECT ID_ARTICLE FROM ARTICLES WHERE REFERENCE = :ref");
    idQuery.bindValue(":ref", reference);
    if (idQuery.exec() && idQuery.next()) {
        idArticle = idQuery.value(0).toInt();
    }
    
    qDebug() << "✅ Article ajouté avec succès, ID:" << idArticle;
    return true;
}

// MODIFIER un article (Requête préparée)
bool Article::modifier()
{
    QString erreur;
    if (!valider(erreur)) {
        qDebug() << "Validation échouée:" << erreur;
        return false;
    }
    
    if (referenceExiste(reference, idArticle)) {
        qDebug() << "La référence existe déjà pour un autre article";
        return false;
    }
    
    QSqlQuery query;
    query.prepare("UPDATE ARTICLES SET REFERENCE = :ref, NOM = :nom, CATEGORIE = :cat, "
                  "TYPE = :type, COULEUR = :couleur, DIMENSION = :dim, "
                  "PRIX_UNITAIRE = :prix, COUT_FABRICATION = :cout, STATUT = :statut "
                  "WHERE ID_ARTICLE = :id");
    
    query.bindValue(":ref", reference);
    query.bindValue(":nom", nom);
    query.bindValue(":cat", categorie);
    query.bindValue(":type", type);
    query.bindValue(":couleur", couleur);
    query.bindValue(":dim", dimensions);
    query.bindValue(":prix", prixUnitaire);
    query.bindValue(":cout", coutFabrication);
    query.bindValue(":statut", statut);
    query.bindValue(":id", idArticle);
    
    if (!query.exec()) {
        qDebug() << "Erreur modification article:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Article modifié avec succès, ID:" << idArticle;
    return true;
}

// SUPPRIMER un article (Requête préparée)
bool Article::supprimer()
{
    QSqlQuery query;
    query.prepare("DELETE FROM ARTICLES WHERE ID_ARTICLE = :id");
    query.bindValue(":id", idArticle);
    
    if (!query.exec()) {
        qDebug() << "Erreur suppression article:" << query.lastError().text();
        return false;
    }
    
    qDebug() << "Article supprimé avec succès, ID:" << idArticle;
    return true;
}

// AFFICHER tous les articles (Requête préparée)
QList<Article> Article::afficher()
{
    QList<Article> articles;
    QSqlQuery query;
    
    query.prepare("SELECT ID_ARTICLE, REFERENCE, NOM, CATEGORIE, TYPE, COULEUR, "
                  "DIMENSION, PRIX_UNITAIRE, COUT_FABRICATION, STATUT, DATE_CREATION "
                  "FROM ARTICLES ORDER BY ID_ARTICLE DESC");
    
    if (!query.exec()) {
        qDebug() << "❌ Erreur affichage articles:" << query.lastError().text();
        return articles;
    }
    
    while (query.next()) {
        Article a;
        a.setIdArticle(query.value(0).toInt());
        a.setReference(query.value(1).toString());
        a.setNom(query.value(2).toString());
        a.setCategorie(query.value(3).toString());
        a.setType(query.value(4).toString());
        a.setCouleur(query.value(5).toString());
        a.setDimensions(query.value(6).toString());
        a.setPrixUnitaire(query.value(7).toDouble());
        a.setCoutFabrication(query.value(8).toDouble());
        a.setStatut(query.value(9).toString());
        a.setDateCreation(query.value(10).toDate());
        articles.append(a);
    }
    
    qDebug() << "✅ Chargé" << articles.size() << "articles";
    return articles;
}

// RECHERCHER un article par ID (Requête préparée)
Article Article::rechercherParId(int id)
{
    Article article;
    QSqlQuery query;
    
    query.prepare("SELECT ID_ARTICLE, REFERENCE, NOM, CATEGORIE, TYPE, COULEUR, "
                  "DIMENSION, PRIX_UNITAIRE, COUT_FABRICATION, STATUT, DATE_CREATION "
                  "FROM ARTICLES WHERE ID_ARTICLE = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        article.setIdArticle(query.value(0).toInt());
        article.setReference(query.value(1).toString());
        article.setNom(query.value(2).toString());
        article.setCategorie(query.value(3).toString());
        article.setType(query.value(4).toString());
        article.setCouleur(query.value(5).toString());
        article.setDimensions(query.value(6).toString());
        article.setPrixUnitaire(query.value(7).toDouble());
        article.setCoutFabrication(query.value(8).toDouble());
        article.setStatut(query.value(9).toString());
        article.setDateCreation(query.value(10).toDate());
    }
    
    return article;
}

// RECHERCHER par référence (Requête préparée)
QList<Article> Article::rechercherParReference(const QString &ref)
{
    QList<Article> articles;
    QSqlQuery query;
    
    query.prepare("SELECT ID_ARTICLE, REFERENCE, NOM, CATEGORIE, TYPE, COULEUR, "
                  "DIMENSION, PRIX_UNITAIRE, COUT_FABRICATION, STATUT, DATE_CREATION "
                  "FROM ARTICLES WHERE UPPER(REFERENCE) LIKE UPPER(:ref)");
    query.bindValue(":ref", "%" + ref + "%");
    
    if (!query.exec()) {
        qDebug() << "Erreur recherche par référence:" << query.lastError().text();
        return articles;
    }
    
    while (query.next()) {
        Article a;
        a.setIdArticle(query.value(0).toInt());
        a.setReference(query.value(1).toString());
        a.setNom(query.value(2).toString());
        a.setCategorie(query.value(3).toString());
        a.setType(query.value(4).toString());
        a.setCouleur(query.value(5).toString());
        a.setDimensions(query.value(6).toString());
        a.setPrixUnitaire(query.value(7).toDouble());
        a.setCoutFabrication(query.value(8).toDouble());
        a.setStatut(query.value(9).toString());
        a.setDateCreation(query.value(10).toDate());
        articles.append(a);
    }
    
    return articles;
}

// RECHERCHER par catégorie (Requête préparée)
QList<Article> Article::rechercherParCategorie(const QString &cat)
{
    QList<Article> articles;
    QSqlQuery query;
    
    query.prepare("SELECT ID_ARTICLE, REFERENCE, NOM, CATEGORIE, TYPE, COULEUR, "
                  "DIMENSION, PRIX_UNITAIRE, COUT_FABRICATION, STATUT, DATE_CREATION "
                  "FROM ARTICLES WHERE UPPER(CATEGORIE) = UPPER(:cat)");
    query.bindValue(":cat", cat);
    
    if (!query.exec()) {
        qDebug() << "Erreur recherche par catégorie:" << query.lastError().text();
        return articles;
    }
    
    while (query.next()) {
        Article a;
        a.setIdArticle(query.value(0).toInt());
        a.setReference(query.value(1).toString());
        a.setNom(query.value(2).toString());
        a.setCategorie(query.value(3).toString());
        a.setType(query.value(4).toString());
        a.setCouleur(query.value(5).toString());
        a.setDimensions(query.value(6).toString());
        a.setPrixUnitaire(query.value(7).toDouble());
        a.setCoutFabrication(query.value(8).toDouble());
        a.setStatut(query.value(9).toString());
        a.setDateCreation(query.value(10).toDate());
        articles.append(a);
    }
    
    return articles;
}

// RECHERCHER par statut (Requête préparée)
QList<Article> Article::rechercherParStatut(const QString &s)
{
    QList<Article> articles;
    QSqlQuery query;
    
    query.prepare("SELECT ID_ARTICLE, REFERENCE, NOM, CATEGORIE, TYPE, COULEUR, "
                  "DIMENSION, PRIX_UNITAIRE, COUT_FABRICATION, STATUT, DATE_CREATION "
                  "FROM ARTICLES WHERE UPPER(STATUT) = UPPER(:statut)");
    query.bindValue(":statut", s);
    
    if (!query.exec()) {
        qDebug() << "Erreur recherche par statut:" << query.lastError().text();
        return articles;
    }
    
    while (query.next()) {
        Article a;
        a.setIdArticle(query.value(0).toInt());
        a.setReference(query.value(1).toString());
        a.setNom(query.value(2).toString());
        a.setCategorie(query.value(3).toString());
        a.setType(query.value(4).toString());
        a.setCouleur(query.value(5).toString());
        a.setDimensions(query.value(6).toString());
        a.setPrixUnitaire(query.value(7).toDouble());
        a.setCoutFabrication(query.value(8).toDouble());
        a.setStatut(query.value(9).toString());
        a.setDateCreation(query.value(10).toDate());
        articles.append(a);
    }
    
    return articles;
}
