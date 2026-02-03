#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ── Logo CUIRÉA dans la sidebar ──────────────────────────────────────
    logoPixmap = QPixmap(":/logo.png");
    if (!logoPixmap.isNull()) {
        ui->labelLogo->setPixmap(logoPixmap.scaled(180, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui->labelLogo->setAlignment(Qt::AlignCenter);
        // On garde le texte sous le logo via le stylesheet (voir .ui)
    }

    // ── Base de données ──────────────────────────────────────────────────
    if (!connecterBaseDeDonnees()) {
        afficherMessageErreur("Erreur Base de Données",
            "Impossible de se connecter à la base de données.");
        return;
    }

    creerTableProduits();
    chargerProduits();
    miseAJourStatistiques();          // remplir les cartes du dashboard

    // Afficher le tableau de bord par défaut
    ui->stackedWidget->setCurrentIndex(0);

    // ── Configuration du tableau des produits ────────────────────────────
    ui->tableWidgetProduits->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidgetProduits->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidgetProduits->horizontalHeader()->setStretchLastSection(true);

    // Désactiver les boutons qui nécessitent une sélection
    ui->btnModifier->setEnabled(false);
    ui->btnSupprimer->setEnabled(false);
    ui->btnConsulter->setEnabled(false);

    // ── Connexions manuelles des signaux ─────────────────────────────────
    // Connexion pour activer/désactiver les boutons lors de la sélection
    connect(ui->tableWidgetProduits, &QTableWidget::itemSelectionChanged,
            this, &MainWindow::on_tableWidgetProduits_itemSelectionChanged);

    // Connexion pour le double-clic (consultation rapide)
    connect(ui->tableWidgetProduits, &QTableWidget::itemDoubleClicked,
            this, &MainWindow::on_tableWidgetProduits_itemDoubleClicked);
}

MainWindow::~MainWindow()
{
    if (db.isOpen())
        db.close();
    delete ui;
}

// ═══════════════════════════════════════════════════════════════════════════
//  BASE DE DONNÉES
// ═══════════════════════════════════════════════════════════════════════════

bool MainWindow::connecterBaseDeDonnees()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("produits.db");

    if (!db.open()) {
        qDebug() << "Erreur de connexion:" << db.lastError().text();
        return false;
    }

    qDebug() << "Base de données connectée avec succès";
    return true;
}

void MainWindow::creerTableProduits()
{
    QSqlQuery query;
    QString createTable = R"(
        CREATE TABLE IF NOT EXISTS ARTICLE (
            Id_produit INTEGER PRIMARY KEY AUTOINCREMENT,
            Reference TEXT NOT NULL UNIQUE,
            Nom TEXT NOT NULL,
            Categorie TEXT,
            Type TEXT,
            Couleur TEXT,
            Dimensions TEXT,
            Prix_unitaire REAL,
            Cout_fabrication REAL,
            Statut TEXT DEFAULT 'disponible',
            Date_creation TEXT DEFAULT CURRENT_TIMESTAMP
        )
    )";

    if (!query.exec(createTable))
        qDebug() << "Erreur création table:" << query.lastError().text();
    else
        qDebug() << "Table ARTICLE créée ou déjà existante";
}

// ═══════════════════════════════════════════════════════════════════════════
//  CHARGEMENT & FILTRAGE DES PRODUITS
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::chargerProduits()
{
    ui->tableWidgetProduits->setRowCount(0);

    QSqlQuery query("SELECT * FROM ARTICLE ORDER BY Id_produit DESC");

    int row = 0;
    while (query.next()) {
        ui->tableWidgetProduits->insertRow(row);

        ui->tableWidgetProduits->setItem(row, 0, new QTableWidgetItem(
            query.value("Id_produit").toString()));
        ui->tableWidgetProduits->setItem(row, 1, new QTableWidgetItem(
            query.value("Reference").toString()));
        ui->tableWidgetProduits->setItem(row, 2, new QTableWidgetItem(
            query.value("Nom").toString()));
        ui->tableWidgetProduits->setItem(row, 3, new QTableWidgetItem(
            query.value("Categorie").toString()));
        ui->tableWidgetProduits->setItem(row, 4, new QTableWidgetItem(
            query.value("Type").toString()));
        ui->tableWidgetProduits->setItem(row, 5, new QTableWidgetItem(
            query.value("Couleur").toString()));
        ui->tableWidgetProduits->setItem(row, 6, new QTableWidgetItem(
            query.value("Dimensions").toString()));
        ui->tableWidgetProduits->setItem(row, 7, new QTableWidgetItem(
            QString::number(query.value("Prix_unitaire").toDouble(), 'f', 2) + " €"));
        ui->tableWidgetProduits->setItem(row, 8, new QTableWidgetItem(
            QString::number(query.value("Cout_fabrication").toDouble(), 'f', 2) + " €"));
        ui->tableWidgetProduits->setItem(row, 9, new QTableWidgetItem(
            query.value("Statut").toString()));
        ui->tableWidgetProduits->setItem(row, 10, new QTableWidgetItem(
            query.value("Date_creation").toString()));

        row++;
    }

    ui->labelNombreProduits->setText(QString::number(row));
}

/*
 * chargerProduitsFiltres – VERSION SÉCURISÉE
 *
 * La version précédente construit la requête SQL par concaténation de chaînes
 * (ex. " AND Nom LIKE '%" + recherche + "%' "), ce qui permet une injection SQL.
 *
 * Ici on utilise QSqlQuery::prepare() + bindValue() pour chaque paramètre.
 * Le filtre LIKE sur trois colonnes est géré par un bloc OR paramétré
 * qui n'est ajouté que si la chaîne de recherche est non vide.
 */
void MainWindow::chargerProduitsFiltres(const QString &recherche, const QString &statut)
{
    ui->tableWidgetProduits->setRowCount(0);

    QString queryStr = "SELECT * FROM ARTICLE WHERE 1=1";

    if (!recherche.isEmpty())
        queryStr += " AND (Reference LIKE :rech1 OR Nom LIKE :rech2 OR Categorie LIKE :rech3)";

    if (!statut.isEmpty() && statut != "Tous les statuts")
        queryStr += " AND Statut = :statut";

    queryStr += " ORDER BY Id_produit DESC";

    QSqlQuery query;
    query.prepare(queryStr);

    QString pattern = "%" + recherche + "%";
    if (!recherche.isEmpty()) {
        query.bindValue(":rech1", pattern);
        query.bindValue(":rech2", pattern);
        query.bindValue(":rech3", pattern);
    }
    if (!statut.isEmpty() && statut != "Tous les statuts")
        query.bindValue(":statut", statut);

    if (!query.exec()) {
        qDebug() << "Erreur filtre:" << query.lastError().text();
        return;
    }

    int row = 0;
    while (query.next()) {
        ui->tableWidgetProduits->insertRow(row);

        ui->tableWidgetProduits->setItem(row, 0, new QTableWidgetItem(
            query.value("Id_produit").toString()));
        ui->tableWidgetProduits->setItem(row, 1, new QTableWidgetItem(
            query.value("Reference").toString()));
        ui->tableWidgetProduits->setItem(row, 2, new QTableWidgetItem(
            query.value("Nom").toString()));
        ui->tableWidgetProduits->setItem(row, 3, new QTableWidgetItem(
            query.value("Categorie").toString()));
        ui->tableWidgetProduits->setItem(row, 4, new QTableWidgetItem(
            query.value("Type").toString()));
        ui->tableWidgetProduits->setItem(row, 5, new QTableWidgetItem(
            query.value("Couleur").toString()));
        ui->tableWidgetProduits->setItem(row, 6, new QTableWidgetItem(
            query.value("Dimensions").toString()));
        ui->tableWidgetProduits->setItem(row, 7, new QTableWidgetItem(
            QString::number(query.value("Prix_unitaire").toDouble(), 'f', 2) + " €"));
        ui->tableWidgetProduits->setItem(row, 8, new QTableWidgetItem(
            QString::number(query.value("Cout_fabrication").toDouble(), 'f', 2) + " €"));
        ui->tableWidgetProduits->setItem(row, 9, new QTableWidgetItem(
            query.value("Statut").toString()));
        ui->tableWidgetProduits->setItem(row, 10, new QTableWidgetItem(
            query.value("Date_creation").toString()));

        row++;
    }

    ui->labelNombreProduits->setText(QString::number(row));
}

// ═══════════════════════════════════════════════════════════════════════════
//  STATISTIQUES TABLEAU DE BORD
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::miseAJourStatistiques()
{
    // Total
    {
        QSqlQuery q("SELECT COUNT(*) FROM ARTICLE");
        if (q.next())
            ui->labelNombreProduits->setText(q.value(0).toString());
    }
    // Disponible
    {
        QSqlQuery q("SELECT COUNT(*) FROM ARTICLE WHERE Statut = 'disponible'");
        if (q.next())
            ui->labelNombreDisponible->setText(q.value(0).toString());
    }
    // En production
    {
        QSqlQuery q("SELECT COUNT(*) FROM ARTICLE WHERE Statut = 'en_production'");
        if (q.next())
            ui->labelNombreEnProduction->setText(q.value(0).toString());
    }
    // Obsolète
    {
        QSqlQuery q("SELECT COUNT(*) FROM ARTICLE WHERE Statut = 'obsolete'");
        if (q.next())
            ui->labelNombreObsolete->setText(q.value(0).toString());
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  SLOTS – NAVIGATION
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::on_btnTableauBord_clicked()
{
    miseAJourStatistiques();
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_btnProduits_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    chargerProduits();
}

// ═══════════════════════════════════════════════════════════════════════════
//  SLOTS – ACTIONS CRUD
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::on_btnCreer_clicked()
{
    viderFormulaireCreer();
    ui->lineEditCreerReference->setText(genererReferenceAutomatique());
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_btnModifier_clicked()
{
    int idProduit = obtenirIdProduitSelectionne();
    if (idProduit > 0) {
        chargerProduitPourModification(idProduit);
        ui->stackedWidget->setCurrentIndex(3);
    }
}

void MainWindow::on_btnSupprimer_clicked()
{
    int idProduit = obtenirIdProduitSelectionne();
    if (idProduit > 0) {
        chargerProduitPourSuppression(idProduit);
        ui->stackedWidget->setCurrentIndex(4);
    }
}

void MainWindow::on_btnConsulter_clicked()
{
    int idProduit = obtenirIdProduitSelectionne();
    if (idProduit > 0) {
        chargerProduitPourConsultation(idProduit);
        ui->stackedWidget->setCurrentIndex(5);
    }
}

// Double-click sur une ligne → ouvre directement la consultation
void MainWindow::on_tableWidgetProduits_itemDoubleClicked(QTableWidgetItem *item)
{
    int row = item->row();
    int idProduit = ui->tableWidgetProduits->item(row, 0)->text().toInt();
    if (idProduit > 0) {
        chargerProduitPourConsultation(idProduit);
        ui->stackedWidget->setCurrentIndex(5);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  SLOTS – FORMULAIRE CRÉER
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::on_btnCreerSauvegarder_clicked()
{
    if (!validerFormulaire(false))
        return;

    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO ARTICLE (Reference, Nom, Categorie, Type, Couleur,
                           Dimensions, Prix_unitaire, Cout_fabrication, Statut, Date_creation)
        VALUES (:ref, :nom, :cat, :type, :couleur, :dim, :prix, :cout, :statut, :date)
    )");

    query.bindValue(":ref",    ui->lineEditCreerReference->text());
    query.bindValue(":nom",    ui->lineEditCreerNom->text());
    query.bindValue(":cat",    ui->comboBoxCreerCategorie->currentText());
    query.bindValue(":type",   ui->lineEditCreerType->text());
    query.bindValue(":couleur",ui->lineEditCreerCouleur->text());
    query.bindValue(":dim",    ui->lineEditCreerDimensions->text());
    query.bindValue(":prix",   ui->doubleSpinBoxCreerPrix->value());
    query.bindValue(":cout",   ui->doubleSpinBoxCreerCout->value());
    query.bindValue(":statut", ui->comboBoxCreerStatut->currentText());
    query.bindValue(":date",   QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    if (query.exec()) {
        afficherMessageSucces("Succès", "Produit créé avec succès !");
        ui->stackedWidget->setCurrentIndex(1);
        chargerProduits();
        miseAJourStatistiques();
    } else {
        afficherMessageErreur("Erreur", "Erreur lors de la création: " +
                            query.lastError().text());
    }
}

void MainWindow::on_btnCreerAnnuler_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

// ═══════════════════════════════════════════════════════════════════════════
//  SLOTS – FORMULAIRE MODIFIER
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::on_btnModifierSauvegarder_clicked()
{
    if (!validerFormulaire(true))
        return;

    int idProduit = ui->labelModifierIdValue->text().toInt();

    QSqlQuery query;
    query.prepare(R"(
        UPDATE ARTICLE SET
            Reference = :ref,
            Nom = :nom,
            Categorie = :cat,
            Type = :type,
            Couleur = :couleur,
            Dimensions = :dim,
            Prix_unitaire = :prix,
            Cout_fabrication = :cout,
            Statut = :statut
        WHERE Id_produit = :id
    )");

    query.bindValue(":ref",    ui->lineEditModifierReference->text());
    query.bindValue(":nom",    ui->lineEditModifierNom->text());
    query.bindValue(":cat",    ui->comboBoxModifierCategorie->currentText());
    query.bindValue(":type",   ui->lineEditModifierType->text());
    query.bindValue(":couleur",ui->lineEditModifierCouleur->text());
    query.bindValue(":dim",    ui->lineEditModifierDimensions->text());
    query.bindValue(":prix",   ui->doubleSpinBoxModifierPrix->value());
    query.bindValue(":cout",   ui->doubleSpinBoxModifierCout->value());
    query.bindValue(":statut", ui->comboBoxModifierStatut->currentText());
    query.bindValue(":id",     idProduit);

    if (query.exec()) {
        afficherMessageSucces("Succès", "Produit modifié avec succès !");
        ui->stackedWidget->setCurrentIndex(1);
        chargerProduits();
        miseAJourStatistiques();
    } else {
        afficherMessageErreur("Erreur", "Erreur lors de la modification: " +
                            query.lastError().text());
    }
}

void MainWindow::on_btnModifierAnnuler_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

// ═══════════════════════════════════════════════════════════════════════════
//  SLOTS – FORMULAIRE SUPPRIMER
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::on_btnSupprimerConfirmer_clicked()
{
    int idProduit = ui->labelSupprimerIdValue->text().toInt();

    QSqlQuery query;
    query.prepare("DELETE FROM ARTICLE WHERE Id_produit = :id");
    query.bindValue(":id", idProduit);

    if (query.exec()) {
        afficherMessageSucces("Succès", "Produit supprimé avec succès !");
        ui->stackedWidget->setCurrentIndex(1);
        chargerProduits();
        miseAJourStatistiques();
    } else {
        afficherMessageErreur("Erreur", "Erreur lors de la suppression: " +
                            query.lastError().text());
    }
}

void MainWindow::on_btnSupprimerAnnuler_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

// ═══════════════════════════════════════════════════════════════════════════
//  SLOTS – FORMULAIRE CONSULTER
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::on_btnConsulterRetour_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

// ═══════════════════════════════════════════════════════════════════════════
//  SLOTS – RECHERCHE & FILTRAGE
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::on_lineEditRecherche_textChanged(const QString &text)
{
    QString statut = ui->comboBoxFiltreStatut->currentText();
    chargerProduitsFiltres(text, statut);
}

void MainWindow::on_comboBoxFiltreStatut_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    QString recherche = ui->lineEditRecherche->text();
    QString statut    = ui->comboBoxFiltreStatut->currentText();
    chargerProduitsFiltres(recherche, statut);
}

// ═══════════════════════════════════════════════════════════════════════════
//  SLOT – SÉLECTION DANS LE TABLEAU
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::on_tableWidgetProduits_itemSelectionChanged()
{
    bool hasSelection = ui->tableWidgetProduits->selectedItems().count() > 0;
    ui->btnModifier->setEnabled(hasSelection);
    ui->btnSupprimer->setEnabled(hasSelection);
    ui->btnConsulter->setEnabled(hasSelection);
}

// ═══════════════════════════════════════════════════════════════════════════
//  MÉTHODES PRIVÉES – UTILITAIRES
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::afficherMessageErreur(const QString &titre, const QString &message)
{
    QMessageBox::critical(this, titre, message);
}

void MainWindow::afficherMessageSucces(const QString &titre, const QString &message)
{
    QMessageBox::information(this, titre, message);
}

void MainWindow::viderFormulaireCreer()
{
    ui->lineEditCreerReference->clear();
    ui->lineEditCreerNom->clear();
    ui->comboBoxCreerCategorie->setCurrentIndex(0);
    ui->lineEditCreerType->clear();
    ui->lineEditCreerCouleur->clear();
    ui->lineEditCreerDimensions->clear();
    ui->doubleSpinBoxCreerPrix->setValue(0.0);
    ui->doubleSpinBoxCreerCout->setValue(0.0);
    ui->comboBoxCreerStatut->setCurrentIndex(0);
}

void MainWindow::viderFormulaireModifier()
{
    ui->labelModifierIdValue->clear();
    ui->lineEditModifierReference->clear();
    ui->lineEditModifierNom->clear();
    ui->comboBoxModifierCategorie->setCurrentIndex(0);
    ui->lineEditModifierType->clear();
    ui->lineEditModifierCouleur->clear();
    ui->lineEditModifierDimensions->clear();
    ui->doubleSpinBoxModifierPrix->setValue(0.0);
    ui->doubleSpinBoxModifierCout->setValue(0.0);
    ui->comboBoxModifierStatut->setCurrentIndex(0);
}

// ═══════════════════════════════════════════════════════════════════════════
//  MÉTHODES PRIVÉES – CHARGEMENT FORMULAIRES
// ═══════════════════════════════════════════════════════════════════════════

void MainWindow::chargerProduitPourModification(int idProduit)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM ARTICLE WHERE Id_produit = :id");
    query.bindValue(":id", idProduit);

    if (query.exec() && query.next()) {
        ui->labelModifierIdValue->setText(query.value("Id_produit").toString());
        ui->lineEditModifierReference->setText(query.value("Reference").toString());
        ui->lineEditModifierNom->setText(query.value("Nom").toString());
        ui->comboBoxModifierCategorie->setCurrentText(query.value("Categorie").toString());
        ui->lineEditModifierType->setText(query.value("Type").toString());
        ui->lineEditModifierCouleur->setText(query.value("Couleur").toString());
        ui->lineEditModifierDimensions->setText(query.value("Dimensions").toString());
        ui->doubleSpinBoxModifierPrix->setValue(query.value("Prix_unitaire").toDouble());
        ui->doubleSpinBoxModifierCout->setValue(query.value("Cout_fabrication").toDouble());
        ui->comboBoxModifierStatut->setCurrentText(query.value("Statut").toString());
    }
}

void MainWindow::chargerProduitPourSuppression(int idProduit)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM ARTICLE WHERE Id_produit = :id");
    query.bindValue(":id", idProduit);

    if (query.exec() && query.next()) {
        ui->labelSupprimerIdValue->setText(query.value("Id_produit").toString());
        ui->labelSupprimerRefValue->setText(query.value("Reference").toString());
        ui->labelSupprimerNomValue->setText(query.value("Nom").toString());
        ui->labelSupprimerPrixValue->setText(
            QString::number(query.value("Prix_unitaire").toDouble(), 'f', 2) + " €");
    }
}

void MainWindow::chargerProduitPourConsultation(int idProduit)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM ARTICLE WHERE Id_produit = :id");
    query.bindValue(":id", idProduit);

    if (query.exec() && query.next()) {
        ui->labelConsulterIdValue->setText(query.value("Id_produit").toString());
        ui->labelConsulterRefValue->setText(query.value("Reference").toString());
        ui->labelConsulterNomValue->setText(query.value("Nom").toString());
        ui->labelConsulterCategorieValue->setText(query.value("Categorie").toString());
        ui->labelConsulterTypeValue->setText(query.value("Type").toString());
        ui->labelConsulterCouleurValue->setText(query.value("Couleur").toString());
        ui->labelConsulterDimensionsValue->setText(query.value("Dimensions").toString());
        ui->labelConsulterPrixValue->setText(
            QString::number(query.value("Prix_unitaire").toDouble(), 'f', 2) + " €");
        ui->labelConsulterCoutValue->setText(
            QString::number(query.value("Cout_fabrication").toDouble(), 'f', 2) + " €");
        ui->labelConsulterStatutValue->setText(query.value("Statut").toString());
        ui->labelConsulterDateValue->setText(query.value("Date_creation").toString());
    }
}

// ═══════════════════════════════════════════════════════════════════════════
//  MÉTHODES PRIVÉES – VALIDATION & GÉNÉRATION
// ═══════════════════════════════════════════════════════════════════════════

int MainWindow::obtenirIdProduitSelectionne()
{
    QList<QTableWidgetItem*> selectedItems = ui->tableWidgetProduits->selectedItems();
    if (selectedItems.isEmpty())
        return -1;

    int row = selectedItems.first()->row();
    return ui->tableWidgetProduits->item(row, 0)->text().toInt();
}

bool MainWindow::validerFormulaire(bool isModification)
{
    QString reference = isModification ? ui->lineEditModifierReference->text()
                                       : ui->lineEditCreerReference->text();
    QString nom       = isModification ? ui->lineEditModifierNom->text()
                                       : ui->lineEditCreerNom->text();

    if (reference.isEmpty()) {
        afficherMessageErreur("Validation", "La référence est obligatoire !");
        return false;
    }

    if (nom.isEmpty()) {
        afficherMessageErreur("Validation", "Le nom est obligatoire !");
        return false;
    }

    return true;
}

QString MainWindow::genererReferenceAutomatique()
{
    QSqlQuery query("SELECT MAX(Id_produit) FROM ARTICLE");
    int nextId = 1;

    if (query.next())
        nextId = query.value(0).toInt() + 1;

    return QString("PROD-%1-%2")
        .arg(QDate::currentDate().year())
        .arg(nextId, 4, 10, QChar('0'));
}
