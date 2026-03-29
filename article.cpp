#include "article.h"
#include "connection.h"
#include <QDebug>

// Helper pour créer une query avec la bonne connexion
static QSqlQuery makeQuery() {
    return QSqlQuery(Connection::instance()->getDatabase());
}

// Constructeur par défaut
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

// Constructeur avec paramètres
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
    if (reference.trimmed().isEmpty()) { erreur = "La référence est obligatoire"; return false; }
    if (nom.trimmed().isEmpty())       { erreur = "Le nom est obligatoire"; return false; }
    if (categorie.trimmed().isEmpty()) { erreur = "La catégorie est obligatoire"; return false; }
    if (prixUnitaire <= 0)             { erreur = "Le prix unitaire doit être > 0"; return false; }
    if (coutFabrication < 0)           { erreur = "Le coût ne peut pas être négatif"; return false; }
    if (prixUnitaire < coutFabrication){ erreur = "Le prix ne peut pas être < coût"; return false; }
    return true;
}

// Vérifier unicité référence
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
    if (!valider(erreur)) { qDebug() << "❌ Validation:" << erreur; return false; }
    if (referenceExiste(reference)) { qDebug() << "❌ Référence déjà existante"; return false; }

    // Récupérer le prochain ID via séquence Oracle
    QSqlQuery seqQuery = makeQuery();
    seqQuery.prepare("SELECT seq_articles.NEXTVAL FROM DUAL");
    if (!seqQuery.exec() || !seqQuery.next()) {
        // Fallback: calculer l'ID manuellement si la séquence n'existe pas
        QSqlQuery maxQuery = makeQuery();
        maxQuery.prepare("SELECT NVL(MAX(ID_ARTICLE), 0) + 1 FROM ARTICLES");
        if (!maxQuery.exec() || !maxQuery.next()) {
            qDebug() << "❌ Impossible de générer l'ID:" << maxQuery.lastError().text();
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
        qDebug() << "❌ Erreur ajout:" << query.lastError().text();
        return false;
    }

    qDebug() << "✅ Article ajouté, ID:" << idArticle;
    return true;
}

// MODIFIER
bool Article::modifier()
{
    QString erreur;
    if (!valider(erreur)) { qDebug() << "❌ Validation:" << erreur; return false; }
    if (referenceExiste(reference, idArticle)) { qDebug() << "❌ Référence déjà utilisée"; return false; }

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

    if (!query.exec()) { qDebug() << "❌ Erreur modifier:" << query.lastError().text(); return false; }
    qDebug() << "✅ Article modifié, ID:" << idArticle;
    return true;
}

// SUPPRIMER
bool Article::supprimer()
{
    QSqlQuery query = makeQuery();
    query.prepare("DELETE FROM ARTICLES WHERE ID_ARTICLE=:id");
    query.bindValue(":id", idArticle);
    if (!query.exec()) { qDebug() << "❌ Erreur supprimer:" << query.lastError().text(); return false; }
    qDebug() << "✅ Article supprimé, ID:" << idArticle;
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
    if (!query.exec()) { qDebug() << "❌ Erreur afficher:" << query.lastError().text(); return list; }
    while (query.next()) list.append(fromQuery(query));
    qDebug() << "✅ Chargé" << list.size() << "articles";
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

// RECHERCHER par référence
QList<Article> Article::rechercherParReference(const QString &ref)
{
    QList<Article> list;
    QSqlQuery query = makeQuery();
    query.prepare(SELECT_COLS + "WHERE UPPER(REFERENCE) LIKE UPPER(:ref)");
    query.bindValue(":ref", "%" + ref + "%");
    if (!query.exec()) { qDebug() << "❌ Erreur recherche ref:" << query.lastError().text(); return list; }
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
    if (!query.exec()) { qDebug() << "❌ Erreur recherche nom:" << query.lastError().text(); return list; }
    while (query.next()) list.append(fromQuery(query));
    return list;
}

// RECHERCHER par catégorie
QList<Article> Article::rechercherParCategorie(const QString &cat)
{
    QList<Article> list;
    QSqlQuery query = makeQuery();
    query.prepare(SELECT_COLS + "WHERE UPPER(CATEGORIE) = UPPER(:cat)");
    query.bindValue(":cat", cat);
    if (!query.exec()) { qDebug() << "❌ Erreur recherche cat:" << query.lastError().text(); return list; }
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
    if (!query.exec()) { qDebug() << "❌ Erreur recherche statut:" << query.lastError().text(); return list; }
    while (query.next()) list.append(fromQuery(query));
    return list;
}


// ═══════════════════════════════════════════════════════════════
// PRÉDICTION DE PRIX - Basée sur la BD de référence REF_ARTICLES_MARCHE
// ═══════════════════════════════════════════════════════════════

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
    // 1. Ratio moyen prix/coût pour la même catégorie
    double ratioCategorie = 2.5;
    QSqlQuery q1 = makeQuery();
    q1.prepare("SELECT AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)) "
               "FROM REF_ARTICLES_MARCHE "
               "WHERE UPPER(CATEGORIE)=UPPER(:cat) AND COUT_FABRICATION>0");
    q1.bindValue(":cat", categorie);
    if (q1.exec() && q1.next() && !q1.value(0).isNull())
        ratioCategorie = q1.value(0).toDouble();

    // 2. Ratio moyen pour le même type
    double ratioType = ratioCategorie;
    QSqlQuery q2 = makeQuery();
    q2.prepare("SELECT AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)) "
               "FROM REF_ARTICLES_MARCHE "
               "WHERE UPPER(TYPE)=UPPER(:type) AND COUT_FABRICATION>0");
    q2.bindValue(":type", type);
    if (q2.exec() && q2.next() && !q2.value(0).isNull())
        ratioType = q2.value(0).toDouble();

    // 3. Ratio moyen pour la même couleur
    double ratioCouleur = ratioCategorie;
    QSqlQuery q3 = makeQuery();
    q3.prepare("SELECT AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)) "
               "FROM REF_ARTICLES_MARCHE "
               "WHERE UPPER(COULEUR)=UPPER(:col) AND COUT_FABRICATION>0");
    q3.bindValue(":col", couleur);
    if (q3.exec() && q3.next() && !q3.value(0).isNull())
        ratioCouleur = q3.value(0).toDouble();

    // 4. Pondération : 50% catégorie + 30% type + 20% couleur
    double ratioPondere = (ratioCategorie * 0.50)
                        + (ratioType      * 0.30)
                        + (ratioCouleur   * 0.20);

    ratioPondere = qBound(1.2, ratioPondere, 6.0);
    return coutFabrication * ratioPondere;
}

// ═══════════════════════════════════════════════════════════════
// PRÉDICTION AVANCÉE - Multi-algorithmes avec intervalle de confiance
// ═══════════════════════════════════════════════════════════════

Article::PredictionResult Article::predirePrixAvance(
    const QString &categorie, const QString &type,
    const QString &couleur,   double coutFabrication)
{
    PredictionResult res;
    res.prixPredit = 0; res.prixMin = 0; res.prixMax = 0;
    res.prixCategorie = 0; res.prixType = 0; res.prixCouleur = 0;
    res.prixKNN = 0; res.margeEstimee = 0; res.nbArticlesRef = 0;
    res.niveauConfiance = "Faible";

    // ── 1. Régression par CATÉGORIE ──────────────────────────────────────
    {
        QSqlQuery q = makeQuery();
        q.prepare("SELECT AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)), COUNT(*) "
                  "FROM REF_ARTICLES_MARCHE "
                  "WHERE UPPER(CATEGORIE)=UPPER(:cat) AND COUT_FABRICATION>0");
        q.bindValue(":cat", categorie);
        if (q.exec() && q.next() && !q.value(0).isNull()) {
            res.prixCategorie  = coutFabrication * q.value(0).toDouble();
            res.nbArticlesRef += q.value(1).toInt();
        }
    }

    // ── 2. Régression par TYPE ───────────────────────────────────────────
    {
        QSqlQuery q = makeQuery();
        q.prepare("SELECT AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)) "
                  "FROM REF_ARTICLES_MARCHE "
                  "WHERE UPPER(TYPE)=UPPER(:type) AND COUT_FABRICATION>0");
        q.bindValue(":type", type);
        if (q.exec() && q.next() && !q.value(0).isNull())
            res.prixType = coutFabrication * q.value(0).toDouble();
        else
            res.prixType = res.prixCategorie;
    }

    // ── 3. Régression par COULEUR ────────────────────────────────────────
    {
        QSqlQuery q = makeQuery();
        q.prepare("SELECT AVG(PRIX_UNITAIRE / NULLIF(COUT_FABRICATION,0)) "
                  "FROM REF_ARTICLES_MARCHE "
                  "WHERE UPPER(COULEUR)=UPPER(:col) AND COUT_FABRICATION>0");
        q.bindValue(":col", couleur);
        if (q.exec() && q.next() && !q.value(0).isNull())
            res.prixCouleur = coutFabrication * q.value(0).toDouble();
        else
            res.prixCouleur = res.prixCategorie;
    }

    // ── 4. k-NN : 3 articles les plus proches par coût ──────────────────
    {
        QSqlQuery q = makeQuery();
        q.prepare("SELECT NOM, PRIX_UNITAIRE, COUT_FABRICATION, "
                  "ABS(COUT_FABRICATION - :cout) AS DIST "
                  "FROM REF_ARTICLES_MARCHE "
                  "WHERE UPPER(CATEGORIE)=UPPER(:cat) AND COUT_FABRICATION>0 "
                  "ORDER BY DIST ASC FETCH FIRST 3 ROWS ONLY");
        q.bindValue(":cout", coutFabrication);
        q.bindValue(":cat",  categorie);
        if (q.exec()) {
            double somme = 0; int n = 0;
            while (q.next()) {
                double prixRef  = q.value(1).toDouble();
                double coutRef  = q.value(2).toDouble();
                double dist     = q.value(3).toDouble() + 1.0; // éviter /0
                double poids    = 1.0 / dist;
                somme += poids * (prixRef / coutRef);
                n++;
                res.articlesProches.append({q.value(0).toString(), prixRef});
            }
            if (n > 0) res.prixKNN = coutFabrication * (somme / n);
        }
        if (res.prixKNN <= 0) res.prixKNN = res.prixCategorie;
    }

    // ── 5. Prix final : moyenne pondérée des 4 algorithmes ───────────────
    // Poids : catégorie 35%, type 25%, couleur 15%, k-NN 25%
    double prixFinal = res.prixCategorie * 0.35
                     + res.prixType      * 0.25
                     + res.prixCouleur   * 0.15
                     + res.prixKNN       * 0.25;

    prixFinal = qBound(coutFabrication * 1.2, prixFinal, coutFabrication * 6.0);
    res.prixPredit = prixFinal;

    // ── 6. Intervalle de confiance (±15% standard, ±10% si bon échantillon)
    double marge_ic = (res.nbArticlesRef >= 5) ? 0.10 : 0.15;
    res.prixMin = prixFinal * (1.0 - marge_ic);
    res.prixMax = prixFinal * (1.0 + marge_ic);

    // ── 7. Marge estimée ─────────────────────────────────────────────────
    res.margeEstimee = coutFabrication > 0
        ? ((prixFinal - coutFabrication) / coutFabrication) * 100.0
        : 0.0;

    // ── 8. Niveau de confiance ───────────────────────────────────────────
    if (res.nbArticlesRef >= 8)       res.niveauConfiance = "🟢 Élevé";
    else if (res.nbArticlesRef >= 4)  res.niveauConfiance = "🟡 Moyen";
    else                              res.niveauConfiance = "🔴 Faible";

    // ── 9. Recommandation stratégique ────────────────────────────────────
    if (res.margeEstimee >= 150)
        res.recommandation = "💎 Positionnement LUXE recommandé - marge exceptionnelle";
    else if (res.margeEstimee >= 80)
        res.recommandation = "⭐ Positionnement PREMIUM - très bonne rentabilité";
    else if (res.margeEstimee >= 50)
        res.recommandation = "✅ Positionnement STANDARD - rentabilité correcte";
    else if (res.margeEstimee >= 30)
        res.recommandation = "⚠️ Rentabilité FAIBLE - envisager de réduire les coûts";
    else
        res.recommandation = "🚨 ATTENTION - coût trop élevé par rapport au marché";

    return res;
}

// ═══════════════════════════════════════════════════════════════
// SAUVEGARDER UNE PRÉDICTION DANS L'HISTORIQUE
// ═══════════════════════════════════════════════════════════════
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
                  res.margeEstimee>=40?"STANDARD":"ENTRÉE DE GAMME";
    q.bindValue(":seg",  seg);
    q.bindValue(":conf", res.niveauConfiance);
    if (!q.exec()) { qDebug() << "❌ Erreur sauvegarde prédiction:" << q.lastError().text(); return false; }
    qDebug() << "✅ Prédiction sauvegardée";
    return true;
}

// ═══════════════════════════════════════════════════════════════
// OPTIMISATION DU COÛT : coût max pour atteindre une marge cible
// Résout : prixPredit(cout) = cout * (1 + margeObjectif/100)
// Par dichotomie sur l'intervalle [1, 500]
// ═══════════════════════════════════════════════════════════════
double Article::optimiserCout(const QString &categorie, const QString &type,
                               const QString &couleur, double margeObjectif)
{
    double lo = 1.0, hi = 500.0;
    for (int i = 0; i < 50; ++i) {  // 50 itérations = précision < 0.001 DT
        double mid = (lo + hi) / 2.0;
        auto res = predirePrixAvance(categorie, type, couleur, mid);
        double margeObtenue = res.margeEstimee;
        if (margeObtenue > margeObjectif) lo = mid;
        else hi = mid;
    }
    return (lo + hi) / 2.0;
}

// ═══════════════════════════════════════════════════════════════
// MATRICE DE DÉCISION : ratio moyen prix/coût par catégorie × couleur
// ═══════════════════════════════════════════════════════════════
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
