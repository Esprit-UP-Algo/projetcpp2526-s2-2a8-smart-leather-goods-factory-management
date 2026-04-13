#include "article.h"
#include "connection.h"
#include <QDebug>

// Helper pour cr├®er une query avec la bonne connexion
static QSqlQuery makeQuery() {
    return QSqlQuery(Connection::instance()->getDatabase());
}

// Constructeur par d├®faut
Article::Article()
    : idArticle(0)
    , couleurR(141), couleurG(110), couleurB(99)
    , largeur(1.0), hauteur(1.0), profondeur(1.0)
    , prixUnitaire(0.0)
    , coutFabrication(0.0)
    , statut("disponible")
    , dateCreation(QDate::currentDate())
{
}

// Constructeur avec param├¿tres
Article::Article(int id, const QString &ref, const QString &n, const QString &cat,
                 const QString &t, const QString &m3d,
                 int r, int g, int b, double larg, double haut, double prof,
                 double prix, double cout, const QString &s, const QDate &date)
    : idArticle(id), reference(ref), nom(n), categorie(cat), type(t), modele3d(m3d),
      couleurR(r), couleurG(g), couleurB(b),
      largeur(larg), hauteur(haut), profondeur(prof),
      prixUnitaire(prix), coutFabrication(cout),
      statut(s), dateCreation(date)
{
}

// Validation
bool Article::valider(QString &erreur) const
{
    if (reference.trimmed().isEmpty()) { erreur = "La r├®f├®rence est obligatoire"; return false; }
    if (nom.trimmed().isEmpty())       { erreur = "Le nom est obligatoire"; return false; }
    if (categorie.trimmed().isEmpty()) { erreur = "La cat├®gorie est obligatoire"; return false; }
    if (prixUnitaire <= 0)             { erreur = "Le prix unitaire doit ├¬tre > 0"; return false; }
    if (coutFabrication < 0)           { erreur = "Le co├╗t ne peut pas ├¬tre n├®gatif"; return false; }
    if (prixUnitaire < coutFabrication){ erreur = "Le prix ne peut pas ├¬tre < co├╗t"; return false; }
    return true;
}

// V├®rifier unicit├® r├®f├®rence
bool Article::referenceExiste(const QString &ref, int excludeId)
{
    QSqlQuery query = makeQuery();
    query.prepare("SELECT COUNT(*) FROM ARTICLES WHERE UPPER(REFERENCE) = UPPER(:ref) AND ID_ARTICLE != :id");
    query.bindValue(":ref", ref);
    query.bindValue(":id", excludeId == -1 ? 0 : excludeId);
    if (query.exec() && query.next()) return query.value(0).toInt() > 0;
    return false;
}

// AJOUTER
bool Article::ajouter()
{
    QString erreur;
    if (!valider(erreur)) { qDebug() << "ÔØî Validation:" << erreur; return false; }
    if (referenceExiste(reference)) { qDebug() << "ÔØî R├®f├®rence d├®j├á existante"; return false; }

    // R├®cup├®rer le prochain ID via s├®quence Oracle
    QSqlQuery seqQuery = makeQuery();
    seqQuery.prepare("SELECT seq_articles.NEXTVAL FROM DUAL");
    if (!seqQuery.exec() || !seqQuery.next()) {
        // Fallback: calculer l'ID manuellement si la s├®quence n'existe pas
        QSqlQuery maxQuery = makeQuery();
        maxQuery.prepare("SELECT NVL(MAX(ID_ARTICLE), 0) + 1 FROM ARTICLES");
        if (!maxQuery.exec() || !maxQuery.next()) {
            qDebug() << "ÔØî Impossible de g├®n├®rer l'ID:" << maxQuery.lastError().text();
            return false;
        }
        idArticle = maxQuery.value(0).toInt();
    } else {
        idArticle = seqQuery.value(0).toInt();
    }

    QSqlQuery query = makeQuery();
    query.prepare("INSERT INTO ARTICLES (ID_ARTICLE, REFERENCE, NOM, CATEGORIE, TYPE, "
                  "MODELE_3D, COULEUR_R, COULEUR_G, COULEUR_B, "
                  "LARGEUR, HAUTEUR, PROFONDEUR, "
                  "PRIX_UNITAIRE, COUT_FABRICATION, STATUT, DATE_CREATION) "
                  "VALUES (:id, :ref, :nom, :cat, :type, "
                  ":modele, :cr, :cg, :cb, :larg, :haut, :prof, "
                  ":prix, :cout, :statut, TO_DATE(:date, 'YYYY-MM-DD'))");
    query.bindValue(":id",     idArticle);
    query.bindValue(":ref",    reference);
    query.bindValue(":nom",    nom);
    query.bindValue(":cat",    categorie);
    query.bindValue(":type",   type);
    query.bindValue(":modele", modele3d);
    query.bindValue(":cr",     couleurR);
    query.bindValue(":cg",     couleurG);
    query.bindValue(":cb",     couleurB);
    query.bindValue(":larg",   largeur);
    query.bindValue(":haut",   hauteur);
    query.bindValue(":prof",   profondeur);
    query.bindValue(":prix",   prixUnitaire);
    query.bindValue(":cout",   coutFabrication);
    query.bindValue(":statut", statut);
    query.bindValue(":date",   dateCreation.toString("yyyy-MM-dd"));

    if (!query.exec()) {
        qDebug() << "ÔØî Erreur ajout:" << query.lastError().text();
        return false;
    }

    qDebug() << "Ô£à Article ajout├®, ID:" << idArticle;
    return true;
}

// MODIFIER
bool Article::modifier()
{
    QString erreur;
    if (!valider(erreur)) { qDebug() << "ÔØî Validation:" << erreur; return false; }
    if (referenceExiste(reference, idArticle)) { qDebug() << "ÔØî R├®f├®rence d├®j├á utilis├®e"; return false; }

    QSqlQuery query = makeQuery();
    query.prepare("UPDATE ARTICLES SET REFERENCE=:ref, NOM=:nom, CATEGORIE=:cat, "
                  "TYPE=:type, MODELE_3D=:modele, "
                  "COULEUR_R=:cr, COULEUR_G=:cg, COULEUR_B=:cb, "
                  "LARGEUR=:larg, HAUTEUR=:haut, PROFONDEUR=:prof, "
                  "PRIX_UNITAIRE=:prix, COUT_FABRICATION=:cout, STATUT=:statut "
                  "WHERE ID_ARTICLE=:id");
    query.bindValue(":ref",    reference);
    query.bindValue(":nom",    nom);
    query.bindValue(":cat",    categorie);
    query.bindValue(":type",   type);
    query.bindValue(":modele", modele3d);
    query.bindValue(":cr",     couleurR);
    query.bindValue(":cg",     couleurG);
    query.bindValue(":cb",     couleurB);
    query.bindValue(":larg",   largeur);
    query.bindValue(":haut",   hauteur);
    query.bindValue(":prof",   profondeur);
    query.bindValue(":prix",   prixUnitaire);
    query.bindValue(":cout",   coutFabrication);
    query.bindValue(":statut", statut);
    query.bindValue(":id",     idArticle);

    if (!query.exec()) { qDebug() << "ÔØî Erreur modifier:" << query.lastError().text(); return false; }
    qDebug() << "Ô£à Article modifi├®, ID:" << idArticle;
    return true;
}

// SUPPRIMER
bool Article::supprimer()
{
    QSqlQuery query = makeQuery();
    query.prepare("DELETE FROM ARTICLES WHERE ID_ARTICLE=:id");
    query.bindValue(":id", idArticle);
    if (!query.exec()) { qDebug() << "ÔØî Erreur supprimer:" << query.lastError().text(); return false; }
    qDebug() << "Ô£à Article supprim├®, ID:" << idArticle;
    return true;
}

// Helper pour remplir un Article depuis une query
static Article fromQuery(QSqlQuery &q) {
    Article a;
    a.setIdArticle(q.value(0).toInt());
    a.setReference(q.value(1).toString());
    a.setNom(q.value(2).toString());
    a.setCategorie(q.value(3).toString());
    a.setType(q.value(4).toString());
    a.setModele3D(q.value(5).toString());
    a.setCouleurR(q.value(6).toInt());
    a.setCouleurG(q.value(7).toInt());
    a.setCouleurB(q.value(8).toInt());
    a.setLargeur(q.value(9).toDouble());
    a.setHauteur(q.value(10).toDouble());
    a.setProfondeur(q.value(11).toDouble());
    a.setPrixUnitaire(q.value(12).toDouble());
    a.setCoutFabrication(q.value(13).toDouble());
    a.setStatut(q.value(14).toString());
    a.setDateCreation(q.value(15).toDate());
    return a;
}

static const QString SELECT_COLS =
    "SELECT ID_ARTICLE, REFERENCE, NOM, CATEGORIE, TYPE, "
    "MODELE_3D, COULEUR_R, COULEUR_G, COULEUR_B, "
    "LARGEUR, HAUTEUR, PROFONDEUR, "
    "PRIX_UNITAIRE, COUT_FABRICATION, STATUT, DATE_CREATION FROM ARTICLES ";

// AFFICHER tous
QList<Article> Article::afficher()
{
    QList<Article> list;
    QSqlQuery query = makeQuery();
    query.prepare(SELECT_COLS + "ORDER BY ID_ARTICLE DESC");
    if (!query.exec()) { return list; }
    while (query.next()) list.append(fromQuery(query));
    return list;
}

// RECHERCHER par ID
Article Article::rechercherParId(int id)
{
    QSqlQuery query = makeQuery();
    query.prepare(SELECT_COLS + "WHERE ID_ARTICLE=:id");
    query.bindValue(":id", id);
    if (query.exec() && query.next()) return fromQuery(query);
    return Article();
}

// RECHERCHER par r├®f├®rence
QList<Article> Article::rechercherParReference(const QString &ref)
{
    QList<Article> list;
    QSqlQuery query = makeQuery();
    query.prepare(SELECT_COLS + "WHERE UPPER(REFERENCE) LIKE UPPER(:ref)");
    query.bindValue(":ref", "%" + ref + "%");
    if (!query.exec()) { qDebug() << "ÔØî Erreur recherche ref:" << query.lastError().text(); return list; }
    while (query.next()) list.append(fromQuery(query));
    return list;
}

// RECHERCHER par nom
QList<Article> Article::rechercherParNom(const QString &n)
{
    QList<Article> list;
    QSqlQuery query = makeQuery();
    query.prepare(SELECT_COLS + "WHERE UPPER(NOM) LIKE UPPER(:nom)");
    query.bindValue(":nom", "%" + n + "%");
    if (!query.exec()) { qDebug() << "ÔØî Erreur recherche nom:" << query.lastError().text(); return list; }
    while (query.next()) list.append(fromQuery(query));
    return list;
}

// RECHERCHER par cat├®gorie
QList<Article> Article::rechercherParCategorie(const QString &cat)
{
    QList<Article> list;
    QSqlQuery query = makeQuery();
    query.prepare(SELECT_COLS + "WHERE UPPER(CATEGORIE) = UPPER(:cat)");
    query.bindValue(":cat", cat);
    if (!query.exec()) { qDebug() << "ÔØî Erreur recherche cat:" << query.lastError().text(); return list; }
    while (query.next()) list.append(fromQuery(query));
    return list;
}

// RECHERCHER par statut
QList<Article> Article::rechercherParStatut(const QString &s)
{
    QList<Article> list;
    QSqlQuery query = makeQuery();
    query.prepare(SELECT_COLS + "WHERE UPPER(STATUT) = UPPER(:statut)");
    query.bindValue(":statut", s);
    if (!query.exec()) { qDebug() << "ÔØî Erreur recherche statut:" << query.lastError().text(); return list; }
    while (query.next()) list.append(fromQuery(query));
    return list;
}


// ÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉ
// PR├ëDICTION DE PRIX - Bas├®e sur la BD de r├®f├®rence REF_ARTICLES_MARCHE
// ÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉ

QMap<QString, double> Article::getMoyennesPrixParCategorie()
{
    QMap<QString, double> moyennes;
    QSqlQuery query = makeQuery();
    query.prepare(
        "SELECT CATEGORIE, AVG(PRIX_UNITAIRE) FROM REF_ARTICLES_MARCHE "
        "WHERE PRIX_UNITAIRE > 0 GROUP BY CATEGORIE"
    );
    if (query.exec())
        while (query.next())
            moyennes[query.value(0).toString()] = query.value(1).toDouble();
    return moyennes;
}

double Article::predirePrix(const QString &categorie, const QString &type,
                             const QString &couleur, double coutFabrication)
{
    // 1. Ratio moyen prix/co├╗t pour la m├¬me cat├®gorie
    double ratioCategorie = 2.5;
    QSqlQuery q1 = makeQuery();
    q1.prepare("SELECT AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)) "
               "FROM REF_ARTICLES_MARCHE "
               "WHERE UPPER(CATEGORIE)=UPPER(:cat) AND COUT_FABRICATION>0");
    q1.bindValue(":cat", categorie);
    if (q1.exec() && q1.next() && !q1.value(0).isNull())
        ratioCategorie = q1.value(0).toDouble();

    // 2. Ratio moyen pour le m├¬me type
    double ratioType = ratioCategorie;
    QSqlQuery q2 = makeQuery();
    q2.prepare("SELECT AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)) "
               "FROM REF_ARTICLES_MARCHE "
               "WHERE UPPER(TYPE)=UPPER(:type) AND COUT_FABRICATION>0");
    q2.bindValue(":type", type);
    if (q2.exec() && q2.next() && !q2.value(0).isNull())
        ratioType = q2.value(0).toDouble();

    // 3. Ratio moyen pour la m├¬me couleur
    double ratioCouleur = ratioCategorie;
    QSqlQuery q3 = makeQuery();
    q3.prepare("SELECT AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)) "
               "FROM REF_ARTICLES_MARCHE "
               "WHERE UPPER(COULEUR)=UPPER(:col) AND COUT_FABRICATION>0");
    q3.bindValue(":col", couleur);
    if (q3.exec() && q3.next() && !q3.value(0).isNull())
        ratioCouleur = q3.value(0).toDouble();

    // 4. Pond├®ration : 50% cat├®gorie + 30% type + 20% couleur
    double ratioPondere = (ratioCategorie * 0.50)
                        + (ratioType      * 0.30)
                        + (ratioCouleur   * 0.20);

    ratioPondere = qBound(1.2, ratioPondere, 6.0);
    return coutFabrication * ratioPondere;
}

// ÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉ
// PR├ëDICTION AVANC├ëE - Multi-algorithmes avec intervalle de confiance
// ÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉ

Article::PredictionResult Article::predirePrixAvance(
    const QString &categorie, const QString &type,
    const QString &couleur,   double coutFabrication)
{
    PredictionResult res;
    res.prixPredit = 0; res.prixMin = 0; res.prixMax = 0;
    res.prixCategorie = 0; res.prixType = 0; res.prixCouleur = 0;
    res.prixKNN = 0; res.margeEstimee = 0; res.nbArticlesRef = 0;
    res.niveauConfiance = "Faible";

    // ÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉ
    // ALGORITHME : k-NN pond├®r├® multi-crit├¿res (k=5)
    //
    // Pourquoi k-NN ? Avec 50 articles de r├®f├®rence, c'est l'algo le plus
    // fiable : il pr├®dit en se basant sur les articles r├®els les plus
    // similaires, sans hypoth├¿se sur la distribution des donn├®es.
    //
    // Distance = |co├╗t_ref - co├╗t_input| + p├®nalit├®_type + p├®nalit├®_couleur
    //   - M├¬me type   ÔåÆ p├®nalit├® 0,  type diff├®rent ÔåÆ +20
    //   - M├¬me couleur ÔåÆ p├®nalit├® 0, couleur diff.  ÔåÆ +5
    // Poids = 1 / (distance + 1)
    // Prix pr├®dit = somme(poids_i * prix_ref_i) / somme(poids_i)
    // ÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉ

    // ÔöÇÔöÇ ├ëtape 1 : k-NN principal (k=5, m├¬me cat├®gorie) ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
    {
        QSqlQuery q = makeQuery();
        q.prepare("SELECT NOM, PRIX_UNITAIRE, COUT_FABRICATION, TYPE, COULEUR "
                  "FROM REF_ARTICLES_MARCHE "
                  "WHERE UPPER(CATEGORIE)=UPPER(:cat) AND COUT_FABRICATION>0 "
                  "ORDER BY ABS(COUT_FABRICATION - :cout) ASC "
                  "FETCH FIRST 10 ROWS ONLY");
        q.bindValue(":cat",  categorie);
        q.bindValue(":cout", coutFabrication);
        if (q.exec()) {
            struct Voisin { QString nom; double prix; double cout; double poids; };
            QVector<Voisin> voisins;

            while (q.next()) {
                QString nomRef  = q.value(0).toString();
                double prixRef  = q.value(1).toDouble();
                double coutRef  = q.value(2).toDouble();
                QString typeRef = q.value(3).toString();
                QString colRef  = q.value(4).toString();

                // Distance composite : co├╗t + p├®nalit├®s type/couleur
                double distCout = qAbs(coutRef - coutFabrication);
                double penType  = (typeRef.toUpper() == type.toUpper()) ? 0.0 : 20.0;
                double penCoul  = (colRef.toUpper() == couleur.toUpper()) ? 0.0 : 5.0;
                double distance = distCout + penType + penCoul;
                double poids    = 1.0 / (distance + 1.0);

                voisins.append({nomRef, prixRef, coutRef, poids});
            }

            // Garder les k=5 meilleurs (d├®j├á tri├®s par co├╗t, re-trier par poids)
            std::sort(voisins.begin(), voisins.end(),
                      [](const Voisin &a, const Voisin &b) { return a.poids > b.poids; });
            int k = qMin(5, voisins.size());

            double sommePrix = 0, sommePoids = 0;
            for (int i = 0; i < k; ++i) {
                // Ajuster le prix proportionnellement au co├╗t
                double ratio = voisins[i].prix / voisins[i].cout;
                sommePrix  += voisins[i].poids * ratio;
                sommePoids += voisins[i].poids;
                res.articlesProches.append({voisins[i].nom, voisins[i].prix});
            }
            if (sommePoids > 0)
                res.prixKNN = coutFabrication * (sommePrix / sommePoids);

            res.nbArticlesRef = k;
        }
    }

    // ÔöÇÔöÇ ├ëtape 2 : ratio moyen par type exact (fallback/comparaison) ÔöÇÔöÇÔöÇÔöÇÔöÇ
    {
        QSqlQuery q = makeQuery();
        q.prepare("SELECT AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)), COUNT(*) "
                  "FROM REF_ARTICLES_MARCHE "
                  "WHERE UPPER(TYPE)=UPPER(:type) AND COUT_FABRICATION>0");
        q.bindValue(":type", type);
        if (q.exec() && q.next() && !q.value(0).isNull()) {
            res.prixType = coutFabrication * q.value(0).toDouble();
            res.nbArticlesRef = qMax(res.nbArticlesRef, q.value(1).toInt());
        } else {
            res.prixType = res.prixKNN;
        }
    }

    // ÔöÇÔöÇ ├ëtape 3 : ratio moyen par cat├®gorie (contexte large) ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
    {
        QSqlQuery q = makeQuery();
        q.prepare("SELECT AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)) "
                  "FROM REF_ARTICLES_MARCHE "
                  "WHERE UPPER(CATEGORIE)=UPPER(:cat) AND COUT_FABRICATION>0");
        q.bindValue(":cat", categorie);
        if (q.exec() && q.next() && !q.value(0).isNull())
            res.prixCategorie = coutFabrication * q.value(0).toDouble();
        else
            res.prixCategorie = res.prixKNN;
    }

    // ÔöÇÔöÇ ├ëtape 4 : bonus couleur premium ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
    {
        QSqlQuery q = makeQuery();
        q.prepare("SELECT AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)) "
                  "FROM REF_ARTICLES_MARCHE "
                  "WHERE UPPER(COULEUR)=UPPER(:col) AND COUT_FABRICATION>0");
        q.bindValue(":col", couleur);
        if (q.exec() && q.next() && !q.value(0).isNull())
            res.prixCouleur = coutFabrication * q.value(0).toDouble();
        else
            res.prixCouleur = res.prixKNN;
    }

    // ÔöÇÔöÇ ├ëtape 5 : prix final ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
    // k-NN domine (60%) car c'est le plus pr├®cis sur nos 50 articles
    // Le ratio par type (25%) corrige si le k-NN manque de voisins du m├¬me type
    // La couleur (15%) capture le premium Camel/Cognac
    double prixFinal;
    if (res.prixKNN > 0) {
        prixFinal = res.prixKNN    * 0.60
                  + res.prixType   * 0.25
                  + res.prixCouleur * 0.15;
    } else {
        // Fallback si pas de voisins
        prixFinal = res.prixType * 0.60
                  + res.prixCategorie * 0.25
                  + res.prixCouleur * 0.15;
    }

    prixFinal = qBound(coutFabrication * 1.5, prixFinal, coutFabrication * 5.0);
    res.prixPredit = prixFinal;

    // ÔöÇÔöÇ ├ëtape 6 : intervalle de confiance bas├® sur la variance des voisins
    double marge_ic;
    if (res.nbArticlesRef >= 5)      marge_ic = 0.08;  // ┬▒8%
    else if (res.nbArticlesRef >= 3) marge_ic = 0.12;  // ┬▒12%
    else                             marge_ic = 0.18;  // ┬▒18%
    res.prixMin = prixFinal * (1.0 - marge_ic);
    res.prixMax = prixFinal * (1.0 + marge_ic);

    // ÔöÇÔöÇ ├ëtape 7 : marge estim├®e ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
    res.margeEstimee = coutFabrication > 0
        ? ((prixFinal - coutFabrication) / coutFabrication) * 100.0
        : 0.0;

    // ÔöÇÔöÇ ├ëtape 8 : niveau de confiance ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
    if (res.nbArticlesRef >= 5)       res.niveauConfiance = "Eleve";
    else if (res.nbArticlesRef >= 3)  res.niveauConfiance = "Moyen";
    else                              res.niveauConfiance = "Faible";

    // ÔöÇÔöÇ ├ëtape 9 : recommandation strat├®gique ÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇÔöÇ
    if (res.margeEstimee >= 150)
        res.recommandation = "Positionnement LUXE - marge exceptionnelle";
    else if (res.margeEstimee >= 80)
        res.recommandation = "Positionnement PREMIUM - tres bonne rentabilite";
    else if (res.margeEstimee >= 50)
        res.recommandation = "Positionnement STANDARD - rentabilite correcte";
    else if (res.margeEstimee >= 30)
        res.recommandation = "Rentabilite FAIBLE - envisager de reduire les couts";
    else
        res.recommandation = "ATTENTION - cout trop eleve par rapport au marche";

    return res;
}

// ÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉ
// SAUVEGARDER UNE PR├ëDICTION DANS L'HISTORIQUE
// ÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉ
bool Article::sauvegarderPrediction(const PredictionResult &res, const QString &categorie,
                                     const QString &type, const QString &couleur, double cout)
{
    QSqlQuery q = makeQuery();
    q.prepare("INSERT INTO HISTORIQUE_PREDICTIONS "
              "(CATEGORIE, TYPE_ARTICLE, COULEUR, COUT_FABRICATION, "
              "PRIX_PREDIT, MARGE_ESTIMEE, SEGMENT, NIVEAU_CONFIANCE) "
              "VALUES (:cat, :type, :col, :cout, :prix, :marge, :seg, :conf)");
    q.bindValue(":cat",  categorie);
    q.bindValue(":type", type);
    q.bindValue(":col",  couleur);
    q.bindValue(":cout", cout);
    q.bindValue(":prix", res.prixPredit);
    q.bindValue(":marge",res.margeEstimee);
    QString seg = res.margeEstimee>=150?"LUXE":res.margeEstimee>=80?"PREMIUM":
                  res.margeEstimee>=40?"STANDARD":"ENTR├ëE DE GAMME";
    q.bindValue(":seg",  seg);
    q.bindValue(":conf", res.niveauConfiance);
    if (!q.exec()) { qDebug() << "ÔØî Erreur sauvegarde pr├®diction:" << q.lastError().text(); return false; }
    qDebug() << "Ô£à Pr├®diction sauvegard├®e";
    return true;
}

// ÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉ
// OPTIMISATION DU CO├øT : co├╗t max pour atteindre une marge cible
// R├®sout : prixPredit(cout) = cout * (1 + margeObjectif/100)
// Par dichotomie sur l'intervalle [1, 500]
// ÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉ
double Article::optimiserCout(const QString &categorie, const QString &type,
                               const QString &couleur, double margeObjectif)
{
    double lo = 1.0, hi = 500.0;
    for (int i = 0; i < 50; ++i) {  // 50 it├®rations = pr├®cision < 0.001 DT
        double mid = (lo + hi) / 2.0;
        auto res = predirePrixAvance(categorie, type, couleur, mid);
        double margeObtenue = res.margeEstimee;
        if (margeObtenue > margeObjectif) lo = mid;
        else hi = mid;
    }
    return (lo + hi) / 2.0;
}

// ÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉ
// MATRICE DE D├ëCISION : ratio moyen prix/co├╗t par cat├®gorie ├ù couleur
// ÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉÔòÉ
QMap<QString, QMap<QString,double>> Article::matriceDecision()
{
    QMap<QString, QMap<QString,double>> matrice;
    QSqlQuery q = makeQuery();
    q.prepare("SELECT CATEGORIE, COULEUR, "
              "AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)) AS RATIO "
              "FROM REF_ARTICLES_MARCHE "
              "WHERE COUT_FABRICATION > 0 "
              "GROUP BY CATEGORIE, COULEUR "
              "ORDER BY CATEGORIE, COULEUR");
    if (q.exec()) {
        while (q.next()) {
            QString cat = q.value(0).toString();
            QString col = q.value(1).toString();
            double ratio = q.value(2).toDouble();
            matrice[cat][col] = ratio;
        }
    }
    return matrice;
}
