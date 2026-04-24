#include "productionview.h"
#include "connection.h"
#include "notification.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QDateEdit>
#include <QSpinBox>
#include <QScrollArea>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QBrush>
#include <QColor>
#include <QFrame>
#include <QGroupBox>
#include <QTime>
#include <QRegularExpression>

// Styles de boutons r�utilisables � d�finis une seule fois pour �viter la duplication
// dans les dialogues onDetailsClicked() et onPlanificationClicked()
static const QString BTN_STYLE_CLOSE =
    "QPushButton{background-color:#757575;color:white;border:none;border-radius:8px;"
    "padding:12px 35px;font-size:14px;font-weight:bold;min-height:45px;}"
    "QPushButton:hover{background-color:#616161;}";

static const QString BTN_STYLE_SAVE =
    "QPushButton{background-color:#4CAF50;color:white;border:none;border-radius:8px;"
    "padding:12px 30px;font-size:14px;font-weight:bold;min-height:45px;min-width:150px;}"
    "QPushButton:hover{background-color:#45A049;}"
    "QPushButton:pressed{background-color:#3D8B40;}";

// ---------------------------------------------------------------------------
// IMPL�MENTATION: ProductionDialog
// ---------------------------------------------------------------------------

ProductionDialog::ProductionDialog(QWidget *parent, DialogMode mode)
    : QDialog(parent), m_mode(mode)
{
    setupUI();
    setMinimumSize(620, 520);
    loadEmployes(); // Charge la liste des employ�s depuis la DB
    loadClients();  // Charge la liste des clients depuis la DB
    loadArticles(); // Charge la liste des articles depuis la DB

    // En mode suppression, tous les champs sont en lecture seule
    bool readOnly = (mode == DeleteMode);
    spnPrix->setEnabled(!readOnly);
    for (auto *w : {cmbProduit, cmbStatut, cmbResponsable, cmbPriorite}) w->setEnabled(!readOnly);
    dateDebut->setEnabled(!readOnly);
    dateFin->setEnabled(!readOnly);
    txtReference->setEnabled(false); // R�f�rence toujours en lecture seule (auto-g�n�r�e)

    // Adapter le titre, les boutons et le message selon le mode
    switch (mode) {
    case AddMode:
        setWindowTitle("Cr�er une Commande");
        lblTitle->setText("+ Cr�er une Nouvelle Commande");
        generateAutoReference(); // G�n�re CMD-YYYY-NNNN automatiquement
        txtId->setText("Auto");
        cmbResponsable->setCurrentIndex(0);
        break;
    case EditMode:
        setWindowTitle("Modifier une Commande");
        lblTitle->setText("? Modifier la Commande");
        btnSave->setText("Mettre � Jour");
        break;
    case DeleteMode:
        setWindowTitle("Supprimer une Commande");
        lblTitle->setText("? Confirmer la Suppression");
        btnSave->setVisible(false);
        btnDelete->setVisible(true); // Afficher le bouton rouge de confirmation
        lblDeleteWarning->setText("? ATTENTION : Vous �tes sur le point de supprimer cette commande.\nCette action est irr�versible.");
        lblDeleteWarning->setVisible(true);
        break;
    }
}

ProductionDialog::~ProductionDialog() {}

void ProductionDialog::setupUI()
{
    setStyleSheet(
        "QDialog{background:#FAF5F0;}"
        "QLabel{color:#291C0E;font-size:12px;font-weight:bold;}"
        "QLineEdit,QComboBox,QDateEdit{background:white;border:2px solid #BCAAA4;"
        "border-radius:6px;padding:8px;font-size:12px;color:#291C0E;}"
        "QLineEdit:focus,QComboBox:focus,QDateEdit:focus{border-color:#8D6E63;}");

    auto *lay = new QVBoxLayout(this);
    lay->setSpacing(15); lay->setContentsMargins(25,25,25,25);

    lblTitle = new QLabel(this);
    lblTitle->setStyleSheet("font-size:17px;font-weight:bold;color:#8D6E63;");
    lblTitle->setAlignment(Qt::AlignCenter);
    lay->addWidget(lblTitle);

    // Grille de formulaire : colonne 0 = labels, colonne 1 = champs (extensible)
    auto *form = new QGridLayout(); form->setSpacing(12); form->setColumnStretch(1,1);
    int r = 0;
    auto addRow = [&](const QString &lbl, QWidget *w){
        form->addWidget(new QLabel(lbl,this), r, 0);
        form->addWidget(w, r++, 1);
    };

    txtId        = new QLineEdit(this); txtId->setReadOnly(true);
    txtReference = new QLineEdit(this); txtReference->setPlaceholderText("Ex: PROD-2024-001");
    cmbProduit   = new QComboBox(this); // Rempli dynamiquement par loadArticles()
    spnPrix = new QDoubleSpinBox(this);
    spnPrix->setRange(0.01, 999999.99);
    spnPrix->setDecimals(2);
    spnPrix->setSuffix(" DT");
    spnPrix->setSingleStep(1.0);
    spnPrix->setValue(1.0);
    spnPrix->setGroupSeparatorShown(false);
    spnPrix->setStyleSheet(
        "QDoubleSpinBox { background:white; border:2px solid #BCAAA4; border-radius:6px; "
        "padding:8px; font-size:12px; color:#291C0E; }"
        "QDoubleSpinBox:focus { border-color:#8D6E63; }"
        "QDoubleSpinBox::up-button, QDoubleSpinBox::down-button { width:18px; }");
    cmbStatut    = new QComboBox(this);
    cmbStatut->addItems({"En Attente","Planifi�","En Cours","En Production","Suspendu","Termin�","Annul�"});
    dateDebut    = new QDateEdit(this); dateDebut->setCalendarPopup(true);
    dateDebut->setDate(QDate::currentDate()); dateDebut->setDisplayFormat("dd/MM/yyyy");
    dateFin      = new QDateEdit(this); dateFin->setCalendarPopup(true);
    dateFin->setDate(QDate::currentDate().addDays(7)); dateFin->setDisplayFormat("dd/MM/yyyy");
    cmbResponsable = new QComboBox(this); // Rempli dynamiquement par loadEmployes()
    cmbPriorite  = new QComboBox(this);
    cmbPriorite->addItems({"Basse","Normale","Haute","Urgente"});
    cmbPriorite->setCurrentIndex(1); // "Normale" par d�faut

    txtId->setVisible(false);
    addRow("R�f�rence * :",      txtReference);
    addRow("Produit * :",        cmbProduit);
    addRow("Prix * :",           spnPrix);
    addRow("Statut :",           cmbStatut);
    addRow("Date D�but * :",     dateDebut);
    addRow("Date Fin Pr�vue * :",dateFin);
    addRow("Employ� * :",        cmbResponsable);
    addRow("Priorit� :",         cmbPriorite);

    cmbClient = new QComboBox(this);
    cmbClient->setPlaceholderText("-- S�lectionner un client --");
    addRow("Client :",           cmbClient);

    lay->addLayout(form);

    // Bandeau d'avertissement rouge � visible uniquement en mode DeleteMode
    lblDeleteWarning = new QLabel(this);
    lblDeleteWarning->setStyleSheet("background:#f7d9d9;padding:12px;border-radius:6px;color:#8B0000;font-weight:bold;");
    lblDeleteWarning->setWordWrap(true); lblDeleteWarning->setVisible(false);
    lay->addWidget(lblDeleteWarning);
    lay->addStretch();

    static const QString BTN_PRIMARY = "QPushButton{background:#8D6E63;color:white;border:none;border-radius:7px;padding:9px 22px;font-size:13px;font-weight:bold;}QPushButton:hover{background:#A0826D;}";
    static const QString BTN_DANGER  = "QPushButton{background:#D32F2F;color:white;border:none;border-radius:7px;padding:9px 22px;font-size:13px;font-weight:bold;}QPushButton:hover{background:#F44336;}";
    static const QString BTN_CANCEL  = "QPushButton{background:#BCAAA4;color:white;border:none;border-radius:7px;padding:9px 22px;font-size:13px;font-weight:bold;}QPushButton:hover{background:#A78D78;}";

    btnSave   = new QPushButton("Enregistrer", this); btnSave->setStyleSheet(BTN_PRIMARY);
    btnDelete = new QPushButton("Confirmer Suppression", this); btnDelete->setStyleSheet(BTN_DANGER); btnDelete->setVisible(false);
    btnCancel = new QPushButton("Annuler", this); btnCancel->setStyleSheet(BTN_CANCEL);

    connect(btnSave,   &QPushButton::clicked, this, &ProductionDialog::onSaveClicked);
    connect(btnDelete, &QPushButton::clicked, this, &ProductionDialog::onDeleteConfirmed);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    auto *btnRow = new QHBoxLayout(); btnRow->addStretch();
    btnRow->addWidget(btnSave); btnRow->addWidget(btnDelete); btnRow->addWidget(btnCancel);
    lay->addLayout(btnRow);
}

void ProductionDialog::setProductionData(const QString &id, const QString &reference, const QString &produit,
                                         const QString &quantite, const QString &statut, const QString &dDebut,
                                         const QString &dFin, const QString &responsable, const QString &priorite,
                                         const QString &mailClient)
{
    txtId->setText(id); txtReference->setText(reference);
    spnPrix->setValue(quantite.toDouble());
    auto setCombo = [](QComboBox *c, const QString &v){
        // Chercher d'abord par userData, puis par texte
        for (int i = 0; i < c->count(); ++i) {
            if (c->itemData(i).toString() == v) { c->setCurrentIndex(i); return; }
        }
        int i = c->findText(v, Qt::MatchContains);
        if (i >= 0) c->setCurrentIndex(i);
    };
    setCombo(cmbProduit, produit); setCombo(cmbStatut, statut);
    setCombo(cmbResponsable, responsable); setCombo(cmbPriorite, priorite);
    dateDebut->setDate(QDate::fromString(dDebut,"dd/MM/yyyy"));
    dateFin->setDate(QDate::fromString(dFin,"dd/MM/yyyy"));
    if (!mailClient.isEmpty()) {
        for (int i = 0; i < cmbClient->count(); ++i) {
            if (cmbClient->itemData(i).toString() == mailClient) {
                cmbClient->setCurrentIndex(i); break;
            }
        }
    }
}

QString ProductionDialog::getId()          const { return txtId->text(); }
QString ProductionDialog::getReference()   const { return txtReference->text(); }
QString ProductionDialog::getProduit()     const { return cmbProduit->currentData().toString(); }
QString ProductionDialog::getQuantite()    const { return QString::number(spnPrix->value(), 'f', 2); }
QString ProductionDialog::getStatut()      const { return cmbStatut->currentText(); }
QString ProductionDialog::getDateDebut()   const { return dateDebut->date().toString("dd/MM/yyyy"); }
QString ProductionDialog::getDateFin()     const { return dateFin->date().toString("dd/MM/yyyy"); }
QString ProductionDialog::getResponsable() const { return cmbResponsable->currentText(); }
QString ProductionDialog::getPriorite()    const { return cmbPriorite->currentText(); }
QString ProductionDialog::getMailClient()  const { return cmbClient ? cmbClient->currentData().toString() : QString(); }

int ProductionDialog::getEmployeId() const
{
    int id = cmbResponsable->currentData().toInt();
    qDebug() << "getEmployeId() ->" << id << cmbResponsable->currentText();
    return id;
}

void ProductionDialog::onSaveClicked()
{
    auto setError  = [](QWidget *w){ w->setStyleSheet("border: 2px solid red; border-radius:6px;"); };
    auto setNormal = [](QWidget *w){ w->setStyleSheet(""); };
    bool valid = true;

    if (txtReference->text().trimmed().isEmpty()) { setError(txtReference); valid = false; }
    else setNormal(txtReference);

    if (spnPrix->value() <= 0) { spnPrix->setStyleSheet("border: 2px solid red; border-radius:6px;"); valid = false; }
    else spnPrix->setStyleSheet("");

    if (dateDebut->date() > dateFin->date()) { setError(dateDebut); setError(dateFin); valid = false; }
    else { setNormal(dateDebut); setNormal(dateFin); }

    if (!valid) return;

    NotificationWidget::show(
        m_mode == AddMode ? "Commande cr��e" : "Commande mise � jour",
        txtReference->text() + " � " + (m_mode == AddMode ? "enregistr�e avec succ�s." : "modifi�e avec succ�s."),
        NotificationWidget::Success
        );
    accept();
}

void ProductionDialog::onDeleteConfirmed()
{
    if (QMessageBox::question(this,"Confirmation","�tes-vous s�r de vouloir supprimer cette commande ?",
                              QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
        NotificationWidget::show(
            "Commande supprim�e",
            txtReference->text() + " � supprim�e d�finitivement.",
            NotificationWidget::Warning
            );
        accept();
    }
}

void ProductionDialog::generateAutoReference()
{
    // Interroge le MAX(ID_COMMANDE) pour g�n�rer la prochaine r�f�rence unique
    // Format : CMD-YYYY-NNNN (ex: CMD-2026-0042)
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT NVL(MAX(ID_COMMANDE), 0) + 1 FROM COMMANDES");
    int nextId = 1;
    if (query.exec() && query.next()) nextId = query.value(0).toInt();
    txtReference->setText(QString("CMD-%1-%2").arg(QDate::currentDate().year()).arg(nextId, 4, 10, QChar('0')));
}

void ProductionDialog::loadEmployes()
{
    QSqlDatabase db = Connection::instance()->getDatabase();
    if (!db.isOpen()) {
        QMessageBox::warning(this, "Erreur", "Base de donn�es non connect�e.");
        return;
    }
    QSqlQuery query(db);
    // Essai en minuscules d'abord (certaines configs Oracle), puis en majuscules
    if (!query.exec("SELECT id_employe, nom, prenom FROM employes ORDER BY nom, prenom")) {
        query.prepare("SELECT ID_EMPLOYE, NOM, PRENOM FROM EMPLOYES ORDER BY NOM, PRENOM");
        if (!query.exec()) {
            QMessageBox::warning(this, "Erreur", "Impossible de charger les employ�s.\n" + query.lastError().text());
            return;
        }
    }
    cmbResponsable->clear(); m_employeMap.clear();
    int count = 0;
    while (query.next()) {
        int id = query.value(0).toInt();
        QString nomComplet = query.value(1).toString() + " " + query.value(2).toString();
        // Stocker l'ID comme userData pour le r�cup�rer via getEmployeId()
        cmbResponsable->addItem(nomComplet, id);
        m_employeMap[id] = nomComplet;
        count++;
    }
    if (count == 0) {
        // Aucun employ� en base ? ajouter un item factice pour �viter un index -1
        QMessageBox::warning(this, "Attention", "? Aucun employ� trouv� dans la base de donn�es!");
        cmbResponsable->addItem("(Aucun employ� disponible)", 0);
    }
}

void ProductionDialog::loadClients()
{
    QSqlDatabase db = Connection::instance()->getDatabase();
    if (!db.isOpen()) return;
    QSqlQuery query(db);
    query.prepare("SELECT EMAIL, PRENOM || ' ' || NOM FROM CLIENTS "
                  "WHERE EMAIL IS NOT NULL ORDER BY NOM, PRENOM");
    cmbClient->clear();
    cmbClient->addItem("-- Aucun client --", "");
    if (query.exec()) {
        while (query.next()) {
            QString email     = query.value(0).toString();
            QString nomComplet = query.value(1).toString();
            cmbClient->addItem(nomComplet + "  <" + email + ">", email);
        }
    }
}

void ProductionDialog::loadArticles()
{
    QSqlDatabase db = Connection::instance()->getDatabase();
    if (!db.isOpen()) return;
    QSqlQuery query(db);
    query.prepare("SELECT NOM FROM ARTICLES ORDER BY NOM");
    cmbProduit->clear();
    if (query.exec()) {
        while (query.next()) {
            QString nom = query.value(0).toString();
            cmbProduit->addItem(nom, nom);
        }
    }
    if (cmbProduit->count() == 0)
        cmbProduit->addItem("(Aucun article disponible)", "");
}

// ---------------------------------------------------------------------------
// IMPL�MENTATION: ProductionViewModel
// ---------------------------------------------------------------------------

ProductionViewModel::ProductionViewModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int ProductionViewModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_commandes.count();
}

int ProductionViewModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return COL_COUNT;
}

QVariant ProductionViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_commandes.count())
        return QVariant();

    const ProductionCommande &cmd = m_commandes.at(index.row());

    // Qt::DisplayRole : texte affich� dans chaque cellule
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case COL_REFERENCE: return cmd.reference;
        case COL_EMPLOYE: return cmd.employe;
        case COL_TYPE: return cmd.type;
        case COL_QUANTITE: return cmd.quantite;
        case COL_DATE_CREATION: return cmd.dateCreation.toString("dd/MM/yyyy");
        case COL_STATUT: return cmd.etatProduction;
        case COL_PRIORITE: return cmd.priorite;
        case COL_MONTANT: return QString::number(cmd.montant, 'f', 2) + " DT";
        }
    }

    // Qt::BackgroundRole : couleur de fond de la cellule Statut selon l'�tat
    if (role == Qt::BackgroundRole) {
        if (index.column() == COL_STATUT) {
            if (cmd.etatProduction == "Termin�") return QBrush(QColor("#27AE60")); // vert
            if (cmd.etatProduction == "En cours") return QBrush(QColor("#F39C12")); // orange
            if (cmd.etatProduction == "Bloqu�") return QBrush(QColor("#E74C3C"));  // rouge
            if (cmd.etatProduction == "Planifi�") return QBrush(QColor("#3498DB")); // bleu
        }
    }

    // Qt::ForegroundRole : texte blanc sur les cellules color�es du statut
    if (role == Qt::ForegroundRole) {
        if (index.column() == COL_STATUT) {
            return QBrush(Qt::white);
        }
    }

    // Qt::TextAlignmentRole : montant align� � droite, reste � gauche
    if (role == Qt::TextAlignmentRole) {
        if (index.column() == COL_MONTANT) {
            return int(Qt::AlignRight | Qt::AlignVCenter);
        }
        return int(Qt::AlignLeft | Qt::AlignVCenter);
    }

    return QVariant();
}

QVariant ProductionViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return getColumnName(section);
    }

    return section + 1;
}

QString ProductionViewModel::getColumnName(int column) const
{
    switch (column) {
    case COL_REFERENCE: return "R�f�rence";
    case COL_EMPLOYE: return "Employ�";
    case COL_TYPE: return "Produit";
    case COL_QUANTITE: return "Quantit�";
    case COL_DATE_CREATION: return "Date Cr�ation";
    case COL_STATUT: return "Statut";
    case COL_PRIORITE: return "Priorit�";
    case COL_MONTANT: return "Montant";
    default: return QString();
    }
}

void ProductionViewModel::setCommandes(const QList<ProductionCommande> &commandes)
{
    beginResetModel();
    m_commandes = commandes;
    endResetModel();
}

void ProductionViewModel::addCommande(const ProductionCommande &commande)
{
    beginInsertRows(QModelIndex(), m_commandes.count(), m_commandes.count());
    m_commandes.append(commande);
    endInsertRows();
}

void ProductionViewModel::updateCommande(int row, const ProductionCommande &commande)
{
    if (row >= 0 && row < m_commandes.count()) {
        m_commandes[row] = commande;
        // Notifie la vue que toute la ligne a chang� (pour rafra�chir les couleurs d'alerte)
        emit dataChanged(index(row, 0), index(row, COL_COUNT - 1));
    }
}

ProductionCommande ProductionViewModel::getCommande(int row) const
{
    if (row >= 0 && row < m_commandes.count()) {
        return m_commandes.at(row);
    }
    return ProductionCommande();
}

void ProductionViewModel::clear()
{
    beginResetModel();
    m_commandes.clear();
    endResetModel();
}

// -- Charge toutes les commandes depuis la DB et notifie la vue ---------------
void ProductionViewModel::loadFromDatabase()
{
    beginResetModel(); // Signale � la vue que les donn�es vont changer
    m_commandes.clear();

    // Jointure avec EMPLOYES pour afficher le nom complet de l'employ� responsable
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT C.REFERENCE, (E.NOM || ' ' || E.PRENOM) AS EMPLOYE, "
                  "C.PRODUIT, C.MONTANT, C.DATE_CREATION, C.STATUT, C.PRIORITE, C.MONTANT, C.ID_COMMANDE "
                  "FROM COMMANDES C "
                  "LEFT JOIN EMPLOYES E ON C.ID_EMPLOYE = E.ID_EMPLOYE "
                  "ORDER BY C.DATE_CREATION DESC");

    if (query.exec()) {
        while (query.next()) {
            ProductionCommande cmd;
            cmd.reference      = query.value(0).toString();
            cmd.employe        = query.value(1).toString();
            cmd.type           = query.value(2).toString();
            cmd.quantite       = query.value(3).toInt();
            cmd.dateCreation   = query.value(4).toDate();
            cmd.etatProduction = query.value(5).toString();
            cmd.priorite       = query.value(6).toString();
            cmd.montant        = query.value(7).toDouble();
            cmd.idCommande     = query.value(8).toInt();
            m_commandes.append(cmd);
        }
    } else {
        qDebug() << "? Erreur chargement donn�es:" << query.lastError().text();
    }

    endResetModel(); // Signale � la vue que les donn�es sont pr�tes
}

// ---------------------------------------------------------------------------
// IMPL�MENTATION: ProductionView
// ---------------------------------------------------------------------------


// -- ProductionSortProxy : tri type par colonne ---------------------------------
bool ProductionSortProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    int col = left.column();
    if (col == ProductionViewModel::COL_DATE_CREATION) {
        QDate l = QDate::fromString(sourceModel()->data(left).toString(), "dd/MM/yyyy");
        QDate r = QDate::fromString(sourceModel()->data(right).toString(), "dd/MM/yyyy");
        return l < r;
    }
    if (col == ProductionViewModel::COL_MONTANT) {
        double l = sourceModel()->data(left).toString().remove(" DT").toDouble();
        double r = sourceModel()->data(right).toString().remove(" DT").toDouble();
        return l < r;
    }
    if (col == ProductionViewModel::COL_QUANTITE) {
        return sourceModel()->data(left).toInt() < sourceModel()->data(right).toInt();
    }
    if (col == ProductionViewModel::COL_PRIORITE) {
        auto rang = [](const QString &p) -> int {
            if (p == "Urgente") return 3;
            if (p == "Haute")   return 2;
            if (p == "Normale") return 1;
            return 0;
        };
        return rang(sourceModel()->data(left).toString()) < rang(sourceModel()->data(right).toString());
    }
    return QSortFilterProxyModel::lessThan(left, right);
}
ProductionView::ProductionView(QWidget *parent)
    : QMainWindow(parent)
    , m_timerAlertes(nullptr)
    , m_joursAlerte(3)           // Seuil par d�faut : alerte 3 jours avant la date limite
    , m_seuilAvancement(50)      // Seuil par d�faut : alerte si avancement < 50%
    , m_intervalleVerification(5) // V�rification toutes les 5 minutes
{
    setWindowTitle("Vue Production Compl�te - Planification, Suivi & Livraison");
    setMinimumSize(1200, 650);
    resize(1200, 650);

    setupUI();          // Construit tous les widgets
    setupConnections(); // Connecte les signaux/slots
    setupTimer();       // D�marre le timer de v�rification des alertes
    loadData();         // Charge les donn�es depuis la DB
}

ProductionView::~ProductionView()
{
}

void ProductionView::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setStyleSheet("background-color: #F8F9FA;");  // Gris tr�s clair moderne

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    // Titre moderne avec d�grad� bleu
    QWidget *titleWidget = new QWidget(this);
    titleWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #2C3E50, stop:1 #3498DB); "
        "border-radius: 8px; padding: 12px;"
        );
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    QLabel *title = new QLabel("?? GESTION DE LA PRODUCTION");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: white; letter-spacing: 1px;");
    title->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(title);
    mainLayout->addWidget(titleWidget);

    // Barre de filtres moderne
    QWidget *filterWidget = new QWidget(this);
    filterWidget->setStyleSheet(
        "QWidget { background-color: white; border-radius: 8px; padding: 12px; "
        "border: 1px solid #E0E0E0; }"
        "QLabel { color: #2C3E50; font-weight: 600; font-size: 12px; }"
        );
    QHBoxLayout *filterLayout = new QHBoxLayout(filterWidget);
    filterLayout->setSpacing(15);

    m_searchBox = new QLineEdit(this);
    m_searchBox->setPlaceholderText("?? Rechercher...");
    m_searchBox->setMinimumWidth(250);
    m_searchBox->setMinimumHeight(36);
    m_searchBox->setStyleSheet(
        "QLineEdit { "
        "background-color: #F8F9FA; "
        "border: 2px solid #E0E0E0; "
        "border-radius: 6px; "
        "padding: 8px 12px; "
        "font-size: 13px; "
        "color: #2C3E50; "
        "}"
        "QLineEdit:focus { "
        "border-color: #3498DB; "
        "background-color: white; "
        "}"
        );

    m_etatFilter = new QComboBox(this);
    m_etatFilter->addItems({"Tous les �tats", "Planifi�", "En cours", "Bloqu�", "Termin�"});
    m_etatFilter->setMinimumWidth(150);
    m_etatFilter->setMinimumHeight(36);
    m_etatFilter->setStyleSheet(
        "QComboBox { "
        "background-color: #F8F9FA; "
        "border: 2px solid #E0E0E0; "
        "border-radius: 6px; "
        "padding: 6px 12px; "
        "font-size: 13px; "
        "color: #2C3E50; "
        "font-weight: 500; "
        "}"
        "QComboBox:hover { "
        "border-color: #3498DB; "
        "}"
        "QComboBox::drop-down { "
        "border: none; "
        "width: 25px; "
        "}"
        "QComboBox::down-arrow { "
        "image: none; "
        "border-left: 4px solid transparent; "
        "border-right: 4px solid transparent; "
        "border-top: 5px solid #3498DB; "
        "}"
        );

    m_statutLivraisonFilter = new QComboBox(this);
    m_statutLivraisonFilter->addItems({"Tous les statuts", "Non exp�di�e", "En livraison", "Livr�e"});
    m_statutLivraisonFilter->setMinimumWidth(150);
    m_statutLivraisonFilter->setMinimumHeight(36);
    m_statutLivraisonFilter->setStyleSheet(m_etatFilter->styleSheet());

    QLabel *lblRecherche = new QLabel("Recherche:", this);
    QLabel *lblEtat = new QLabel("�tat:", this);
    QLabel *lblStatut = new QLabel("Livraison:", this);

    filterLayout->addWidget(lblRecherche);
    filterLayout->addWidget(m_searchBox);
    filterLayout->addWidget(lblEtat);
    filterLayout->addWidget(m_etatFilter);
    filterLayout->addWidget(lblStatut);
    filterLayout->addWidget(m_statutLivraisonFilter);
    filterLayout->addStretch();

    mainLayout->addWidget(filterWidget);

    // Boutons d'action modernes
    QWidget *buttonWidget = new QWidget(this);
    buttonWidget->setStyleSheet("background-color: transparent;");
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonWidget);
    buttonLayout->setSpacing(12);

    QString btnStyle =
        "QPushButton { "
        "background-color: #3498DB; "
        "color: white; "
        "border: none; "
        "border-radius: 6px; "
        "padding: 10px 20px; "
        "font-size: 13px; "
        "font-weight: 600; "
        "min-height: 38px; "
        "}"
        "QPushButton:hover { "
        "background-color: #2980B9; "
        "}"
        "QPushButton:pressed { "
        "background-color: #21618C; "
        "}";

    m_btnPlanification = new QPushButton("?? Planification", this);
    m_btnPlanification->setStyleSheet(btnStyle);
    m_btnPlanification->setCursor(Qt::PointingHandCursor);

    m_btnDetails = new QPushButton("?? D�tails", this);
    m_btnDetails->setStyleSheet(btnStyle);
    m_btnDetails->setCursor(Qt::PointingHandCursor);

    m_btnRefresh = new QPushButton("?? Actualiser", this);
    m_btnRefresh->setStyleSheet(
        "QPushButton { "
        "background-color: #27AE60; "
        "color: white; "
        "border: none; "
        "border-radius: 6px; "
        "padding: 10px 20px; "
        "font-size: 13px; "
        "font-weight: 600; "
        "min-height: 38px; "
        "}"
        "QPushButton:hover { "
        "background-color: #229954; "
        "}"
        "QPushButton:pressed { "
        "background-color: #1E8449; "
        "}"
        );
    m_btnRefresh->setCursor(Qt::PointingHandCursor);

    buttonLayout->addWidget(m_btnPlanification);
    buttonLayout->addWidget(m_btnDetails);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_btnRefresh);

    mainLayout->addWidget(buttonWidget);

    // TableView avec design moderne et professionnel
    m_tableView = new QTableView(this);
    m_tableView->setStyleSheet(
        "QTableView { "
        "background-color: white; "
        "border: 1px solid #E0E0E0; "
        "border-radius: 8px; "
        "gridline-color: #F0F0F0; "
        "selection-background-color: #3498DB; "
        "selection-color: white; "
        "font-size: 12px; "
        "font-family: 'Segoe UI', Arial, sans-serif; "
        "}"
        "QTableView::item { "
        "padding: 8px 6px; "
        "border-bottom: 1px solid #F5F5F5; "
        "}"
        "QTableView::item:selected { "
        "background-color: #3498DB; "
        "color: white; "
        "}"
        "QTableView::item:hover { "
        "background-color: #EBF5FB; "
        "}"
        "QHeaderView::section { "
        "background-color: #34495E; "
        "color: white; "
        "padding: 10px 8px; "
        "border: none; "
        "border-right: 1px solid #2C3E50; "
        "font-weight: 600; "
        "font-size: 11px; "
        "text-transform: uppercase; "
        "letter-spacing: 0.5px; "
        "}"
        "QHeaderView::section:hover { "
        "background-color: #2C3E50; "
        "}"
        "QTableView QTableCornerButton::section { "
        "background-color: #34495E; "
        "border: none; "
        "}"
        );
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSortingEnabled(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->horizontalHeader()->setStretchLastSection(false);
    m_tableView->setShowGrid(true);
    m_tableView->setGridStyle(Qt::SolidLine);
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);

    // Mod�le source + proxy pour filtrage/tri sans modifier les donn�es originales
    m_model = new ProductionViewModel(this);
    m_proxyModel = new ProductionSortProxy(this);
    m_proxyModel->setSourceModel(m_model);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(ProductionViewModel::COL_REFERENCE); // Filtre par d�faut sur la r�f�rence

    m_tableView->setModel(m_proxyModel);

    mainLayout->addWidget(m_tableView);

    // L�gende moderne et compacte
    QWidget *legendWidget = new QWidget(this);
    legendWidget->setStyleSheet(
        "background: white; "
        "border: 1px solid #E0E0E0; "
        "border-radius: 8px; "
        "padding: 10px;"
        );
    QHBoxLayout *legendLayout = new QHBoxLayout(legendWidget);

    QLabel *legend = new QLabel(
        "<b style='color:#2C3E50; font-size:12px;'>�tats:</b> "
        "<span style='background:#27AE60;color:white;padding:4px 10px;border-radius:4px;font-weight:600;margin-left:8px;font-size:11px;'>Termin�</span> "
        "<span style='background:#F39C12;color:white;padding:4px 10px;border-radius:4px;font-weight:600;margin-left:6px;font-size:11px;'>En cours</span> "
        "<span style='background:#E74C3C;color:white;padding:4px 10px;border-radius:4px;font-weight:600;margin-left:6px;font-size:11px;'>Bloqu�</span> "
        "<span style='background:#3498DB;color:white;padding:4px 10px;border-radius:4px;font-weight:600;margin-left:6px;font-size:11px;'>Planifi�</span>"
        );
    legend->setStyleSheet("font-size: 12px;");
    legendLayout->addWidget(legend);
    legendLayout->addStretch();

    mainLayout->addWidget(legendWidget);
}

void ProductionView::setupConnections()
{
    connect(m_searchBox, &QLineEdit::textChanged, this, &ProductionView::onSearchTextChanged);
    connect(m_etatFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ProductionView::onEtatFilterChanged);
    connect(m_statutLivraisonFilter, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ProductionView::onStatutLivraisonFilterChanged);
    connect(m_btnDetails, &QPushButton::clicked, this, &ProductionView::onDetailsClicked);
    connect(m_btnPlanification, &QPushButton::clicked, this, &ProductionView::onPlanificationClicked);
    connect(m_btnRefresh, &QPushButton::clicked, this, &ProductionView::onRefreshClicked);
}

void ProductionView::loadData()
{
    qDebug() << "ProductionView::loadData() appel�e";
    m_model->loadFromDatabase();
    qDebug() << "Nombre de lignes dans le mod�le:" << m_model->rowCount();

    // Largeurs de colonnes optimis�es pour la lisibilit�
    m_tableView->setColumnWidth(ProductionViewModel::COL_REFERENCE, 150);
    m_tableView->setColumnWidth(ProductionViewModel::COL_EMPLOYE, 180);
    m_tableView->setColumnWidth(ProductionViewModel::COL_TYPE, 150);
    m_tableView->setColumnWidth(ProductionViewModel::COL_QUANTITE, 80);
    m_tableView->setColumnWidth(ProductionViewModel::COL_DATE_CREATION, 120);
    m_tableView->setColumnWidth(ProductionViewModel::COL_STATUT, 120);
    m_tableView->setColumnWidth(ProductionViewModel::COL_PRIORITE, 100);
    m_tableView->setColumnWidth(ProductionViewModel::COL_MONTANT, 120);

    // V�rifier les alertes d�s le chargement (sans attendre le timer)
    verifierAlertes();

    if (m_model->rowCount() == 0) {
        qDebug() << "? Aucune donn�e charg�e!";
        QMessageBox::information(this, "Information",
                                 "Aucune commande trouv�e dans la base de donn�es.\n\n"
                                 "Ajoutez d'abord des commandes depuis l'onglet Production.");
    }
}

void ProductionView::onSearchTextChanged(const QString &text)
{
    m_proxyModel->setFilterFixedString(text);
}

void ProductionView::onEtatFilterChanged(int index)
{
    if (index == 0) {
        // "Tous les �tats" ? revenir au filtre par r�f�rence (barre de recherche)
        m_proxyModel->setFilterKeyColumn(ProductionViewModel::COL_REFERENCE);
        m_proxyModel->setFilterFixedString(m_searchBox->text());
    } else {
        // Filtrer sur la colonne Statut avec la valeur s�lectionn�e
        m_proxyModel->setFilterKeyColumn(ProductionViewModel::COL_STATUT);
        m_proxyModel->setFilterFixedString(m_etatFilter->currentText());
    }
}

void ProductionView::onStatutLivraisonFilterChanged(int index)
{
    // Le filtre livraison n'est pas g�r� par le mod�le simplifi� (champ non charg� depuis DB)
    // On r�initialise simplement le filtre sur la r�f�rence si "Tous les statuts" est s�lectionn�
    if (index == 0) {
        m_proxyModel->setFilterKeyColumn(ProductionViewModel::COL_REFERENCE);
        m_proxyModel->setFilterFixedString(m_searchBox->text());
    }
}

void ProductionView::onDetailsClicked()
{
    QModelIndex currentIndex = m_tableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Attention", "Veuillez s�lectionner une commande.");
        return;
    }

    // Convertir l'index proxy ? index source pour acc�der aux donn�es r�elles
    QModelIndex sourceIndex = m_proxyModel->mapToSource(currentIndex);
    ProductionCommande cmd = m_model->getCommande(sourceIndex.row());

    QDialog dlg(this);
    dlg.setWindowTitle("D�tails Complets - " + cmd.reference);
    dlg.setMinimumSize(750, 650);
    dlg.setStyleSheet(
        "QDialog { background-color: #F5F5F5; }"
        "QLabel { color: #424242; }"
        );

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(25, 25, 25, 25);
    layout->setSpacing(20);

    // Titre avec fond
    QWidget *titleWidget = new QWidget(&dlg);
    titleWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #6D4C41, stop:1 #8D6E63); "
        "border-radius: 10px; padding: 15px;"
        );
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    QLabel *title = new QLabel("?? D�TAILS COMPLETS DE LA COMMANDE");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: white;");
    title->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(title);
    layout->addWidget(titleWidget);

    // Scroll area pour le contenu
    QScrollArea *scrollArea = new QScrollArea(&dlg);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("QScrollArea { border: none; background-color: transparent; }");

    QWidget *contentWidget = new QWidget();
    contentWidget->setStyleSheet("background-color: transparent;");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(15);

    // Lambda : cr�e une section avec titre color� dans le scroll area
    auto addSection = [&](const QString &sectionTitle, const QString &icon) {
        QWidget *sectionWidget = new QWidget();
        sectionWidget->setStyleSheet(
            "background-color: white; "
            "border-radius: 10px; "
            "border: 2px solid #E0E0E0;"
            );
        QVBoxLayout *sectionLayout = new QVBoxLayout(sectionWidget);
        sectionLayout->setContentsMargins(20, 15, 20, 15);
        sectionLayout->setSpacing(12);

        QLabel *section = new QLabel(icon + " " + sectionTitle);
        section->setStyleSheet(
            "font-size: 16px; "
            "font-weight: bold; "
            "color: #6D4C41; "
            "background-color: transparent; "
            "border: none; "
            "padding-bottom: 10px; "
            "border-bottom: 2px solid #8D6E63;"
            );
        sectionLayout->addWidget(section);

        contentLayout->addWidget(sectionWidget);
        return sectionLayout;
    };

    // Lambda : ajoute une ligne label/valeur dans une section
    auto addField = [&](QVBoxLayout *sectionLayout, const QString &label, const QString &value) {
        QWidget *fieldWidget = new QWidget();
        fieldWidget->setStyleSheet("background-color: transparent;");
        QHBoxLayout *fieldLayout = new QHBoxLayout(fieldWidget);
        fieldLayout->setContentsMargins(0, 5, 0, 5);

        QLabel *labelWidget = new QLabel(label + ":");
        labelWidget->setStyleSheet(
            "font-weight: bold; "
            "color: #616161; "
            "font-size: 13px; "
            "min-width: 180px;"
            );

        QLabel *valueLabel = new QLabel(value);
        valueLabel->setStyleSheet(
            "background: #FAFAFA; "
            "padding: 10px 15px; "
            "border-radius: 6px; "
            "border: 1px solid #E0E0E0; "
            "color: #212121; "
            "font-size: 13px; "
            "font-weight: 600;"
            );
        valueLabel->setWordWrap(true);

        fieldLayout->addWidget(labelWidget);
        fieldLayout->addWidget(valueLabel, 1);
        sectionLayout->addWidget(fieldWidget);
    };

    QVBoxLayout *identSection = addSection("IDENTIFICATION", "??");
    addField(identSection, "ID Commande", QString::number(cmd.idCommande));
    addField(identSection, "R�f�rence", cmd.reference);
    addField(identSection, "Priorit�", cmd.priorite);
    addField(identSection, "Date Livraison Pr�vue", cmd.dateLivraisonPrevue.toString("dd/MM/yyyy"));

    QVBoxLayout *planSection = addSection("PLANIFICATION", "??");
    addField(planSection, "Date D�but Pr�vue", cmd.dateDebutPrevue.toString("dd/MM/yyyy"));
    addField(planSection, "Date Fin Pr�vue", cmd.dateFinPrevue.toString("dd/MM/yyyy"));
    addField(planSection, "Atelier", cmd.atelier);
    addField(planSection, "Ordre de Passage", QString::number(cmd.ordrePassage));

    QVBoxLayout *suiviSection = addSection("SUIVI DE PRODUCTION", "??");
    addField(suiviSection, "�tat Production", cmd.etatProduction);
    addField(suiviSection, "�tape Actuelle", cmd.etapeActuelle);
    addField(suiviSection, "Avancement", cmd.getAvancementText());
    addField(suiviSection, "Retard", cmd.getRetardText());

    QVBoxLayout *livraisonSection = addSection("LIVRAISON", "??");
    addField(livraisonSection, "Soci�t� de Livraison", cmd.societeLivraison);
    addField(livraisonSection, "Num�ro de Suivi", cmd.numeroSuiviColis.isEmpty() ? "Non attribu�" : cmd.numeroSuiviColis);
    addField(livraisonSection, "Date Exp�dition Pr�vue", cmd.dateExpeditionPrevue.toString("dd/MM/yyyy"));
    addField(livraisonSection, "Date Exp�dition R�elle", cmd.dateExpeditionReelle.isValid() ?
                                                             cmd.dateExpeditionReelle.toString("dd/MM/yyyy") : "Non exp�di�e");
    addField(livraisonSection, "Statut Livraison", cmd.statutLivraison);

    scrollArea->setWidget(contentWidget);
    layout->addWidget(scrollArea);

    QPushButton *closeBtn = new QPushButton("? Fermer", &dlg);
    closeBtn->setStyleSheet(BTN_STYLE_CLOSE);
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(closeBtn);
    layout->addLayout(btnLayout);

    dlg.exec();
}

void ProductionView::onPlanificationClicked()
{
    QModelIndex currentIndex = m_tableView->currentIndex();
    if (!currentIndex.isValid()) {
        QMessageBox::warning(this, "Attention", "Veuillez s�lectionner une commande.");
        return;
    }

    // Convertir l'index proxy ? index source pour modifier les donn�es r�elles
    QModelIndex sourceIndex = m_proxyModel->mapToSource(currentIndex);
    ProductionCommande cmd = m_model->getCommande(sourceIndex.row());

    QDialog dlg(this);
    dlg.setWindowTitle("Modifier Planification - " + cmd.reference);
    dlg.setMinimumSize(450, 380);  // Taille r�duite
    dlg.setMaximumSize(500, 420);  // Limite la taille maximale
    dlg.setStyleSheet("QDialog { background-color: #F5F5F5; }");

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->setContentsMargins(25, 25, 25, 25);
    layout->setSpacing(20);

    // Titre avec fond
    QWidget *titleWidget = new QWidget(&dlg);
    titleWidget->setStyleSheet(
        "background: qlineargradient(x1:0, y1:0, x2:1, y2:0, "
        "stop:0 #6D4C41, stop:1 #8D6E63); "
        "border-radius: 10px; padding: 15px;"
        );
    QHBoxLayout *titleLayout = new QHBoxLayout(titleWidget);
    QLabel *title = new QLabel("?? MODIFIER LA PLANIFICATION");
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: white;");
    title->setAlignment(Qt::AlignCenter);
    titleLayout->addWidget(title);
    layout->addWidget(titleWidget);

    // Formulaire dans un widget blanc
    QWidget *formWidget = new QWidget(&dlg);
    formWidget->setStyleSheet(
        "QWidget { "
        "background-color: white; "
        "border-radius: 10px; "
        "border: 2px solid #E0E0E0; "
        "}"
        );
    QVBoxLayout *formContainer = new QVBoxLayout(formWidget);
    formContainer->setContentsMargins(25, 25, 25, 25);

    QFormLayout *form = new QFormLayout();
    form->setSpacing(20);
    form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);

    QString fieldStyle =
        "background: #FAFAFA; "
        "border: 2px solid #BDBDBD; "
        "border-radius: 8px; "
        "padding: 10px 15px; "
        "font-size: 14px; "
        "color: #212121; "
        "min-height: 40px;";

    QString labelStyle =
        "color: #424242; "
        "font-weight: bold; "
        "font-size: 14px;";

    QLabel *lblDateDebut = new QLabel("Date D�but Pr�vue:");
    lblDateDebut->setStyleSheet(labelStyle);
    QDateEdit *dateDebut = new QDateEdit(&dlg);
    dateDebut->setDate(cmd.dateDebutPrevue);
    dateDebut->setCalendarPopup(true);
    dateDebut->setDisplayFormat("dd/MM/yyyy");
    dateDebut->setStyleSheet(fieldStyle);

    QLabel *lblDateFin = new QLabel("Date Fin Pr�vue:");
    lblDateFin->setStyleSheet(labelStyle);
    QDateEdit *dateFin = new QDateEdit(&dlg);
    dateFin->setDate(cmd.dateFinPrevue);
    dateFin->setCalendarPopup(true);
    dateFin->setDisplayFormat("dd/MM/yyyy");
    dateFin->setStyleSheet(fieldStyle);

    QLabel *lblAtelier = new QLabel("Atelier:");
    lblAtelier->setStyleSheet(labelStyle);
    QLineEdit *atelier = new QLineEdit(cmd.atelier, &dlg);
    atelier->setStyleSheet(fieldStyle);

    QLabel *lblOrdre = new QLabel("Ordre de Passage:");
    lblOrdre->setStyleSheet(labelStyle);
    QSpinBox *ordre = new QSpinBox(&dlg);
    ordre->setRange(1, 999);
    ordre->setValue(cmd.ordrePassage);
    ordre->setStyleSheet(fieldStyle);

    form->addRow(lblDateDebut, dateDebut);
    form->addRow(lblDateFin, dateFin);
    form->addRow(lblAtelier, atelier);
    form->addRow(lblOrdre, ordre);

    formContainer->addLayout(form);
    layout->addWidget(formWidget);
    layout->addStretch();

    // Boutons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(15);

    QPushButton *saveBtn = new QPushButton("?? Enregistrer", &dlg);
    saveBtn->setStyleSheet(BTN_STYLE_SAVE);
    saveBtn->setCursor(Qt::PointingHandCursor);

    QPushButton *cancelBtn = new QPushButton("? Annuler", &dlg);
    cancelBtn->setStyleSheet(BTN_STYLE_CLOSE);
    cancelBtn->setCursor(Qt::PointingHandCursor);

    connect(saveBtn, &QPushButton::clicked, [&]() {
        // Mettre � jour les champs de planification dans l'objet cmd
        cmd.dateDebutPrevue = dateDebut->date();
        cmd.dateFinPrevue = dateFin->date();
        cmd.atelier = atelier->text();
        cmd.ordrePassage = ordre->value();

        // Mettre � jour le mod�le en m�moire (pas de persistance DB ici)
        m_model->updateCommande(sourceIndex.row(), cmd);

        NotificationWidget::show(
            "Planification mise � jour",
            cmd.reference + " � dates et atelier enregistr�s.",
            NotificationWidget::Success
            );
        dlg.accept();
    });

    connect(cancelBtn, &QPushButton::clicked, &dlg, &QDialog::reject);

    btnLayout->addStretch();
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    dlg.exec();
}

void ProductionView::onRefreshClicked()
{
    loadData();
    QMessageBox::information(this, "Actualisation", "Donn�es actualis�es avec succ�s!");
}

// ---------------------------------------------------------------------------
// IMPL�MENTATION: Syst�me d'alertes de retard
// ---------------------------------------------------------------------------

void ProductionView::setupTimer()
{
    qDebug() << "ProductionView::setupTimer() - Configuration du timer d'alertes";

    m_timerAlertes = new QTimer(this);

    // Convertir les minutes en millisecondes pour QTimer::setInterval
    int intervalleMs = m_intervalleVerification * 60 * 1000;
    m_timerAlertes->setInterval(intervalleMs);

    // � chaque expiration du timer, v�rifier les alertes de retard
    connect(m_timerAlertes, &QTimer::timeout, this, &ProductionView::verifierAlertes);

    m_timerAlertes->start();

    qDebug() << "Timer configur� avec intervalle de" << m_intervalleVerification << "minutes";
}

// -- V�rification p�riodique des alertes (appel�e par le timer) ---------------
void ProductionView::verifierAlertes()
{
    qDebug() << "verifierAlertes() �" << QTime::currentTime().toString();
    if (!m_model) return;

    int nbAlertes = 0, nbRetards = 0, nbRisques = 0;

    for (int row = 0; row < m_model->rowCount(); ++row) {
        ProductionCommande cmd = m_model->getCommande(row);

        // Recalculer l'alerte avec les seuils configur�s
        ProductionCommande::AlerteRetard ancienneAlerte = cmd.alerteRetard;
        cmd.alerteRetard = cmd.calculerAlerteRetard(m_joursAlerte, m_seuilAvancement);

        // N'afficher la notification que si l'alerte est nouvelle ET pas encore envoy�e
        // Cl� unique = idCommande * 10 + niveau_alerte (�vite les doublons entre Risque et Retard)
        if (cmd.alerteRetard != ancienneAlerte && cmd.alerteRetard != ProductionCommande::Aucune) {
            int cleAlerte = cmd.idCommande * 10 + static_cast<int>(cmd.alerteRetard);
            if (!m_alertesNotifiees.contains(cleAlerte)) {
                afficherNotification(cmd);
                m_alertesNotifiees.insert(cleAlerte); // Marquer comme notifi�e
                nbAlertes++;
            }
        }

        // Mettre � jour le mod�le pour que la couleur de la ligne refl�te la nouvelle alerte
        m_model->updateCommande(row, cmd);

        if (cmd.alerteRetard == ProductionCommande::Retard) nbRetards++;
        else if (cmd.alerteRetard == ProductionCommande::Risque) nbRisques++;
    }

    qDebug() << "V�rification termin�e:" << nbAlertes << "nouvelles alertes,"
             << nbRetards << "retards," << nbRisques << "risques";
}

void ProductionView::afficherNotification(const ProductionCommande &commande)
{
    qDebug() << "ProductionView::afficherNotification() -" << commande.reference;

    if (commande.alerteRetard == ProductionCommande::Retard) {
        NotificationWidget::show(
            "Retard livraison",
            commande.reference + " - " + QString::number(commande.getJoursRetard()) + " jour(s) de retard.",
            NotificationWidget::Critical,
            "Voir", [this, commande]{ Q_UNUSED(commande) loadData(); }
            );
    } else {
        NotificationWidget::show(
            "Risque de retard",
            commande.reference + " - avancement " + commande.getAvancementText() + ", livraison proche.",
            NotificationWidget::Warning
            );
    }
}

void ProductionView::recalculerToutesLesAlertes()
{
    qDebug() << "ProductionView::recalculerToutesLesAlertes() - Recalcul de toutes les alertes";

    if (!m_model) {
        qDebug() << "? Mod�le non initialis�";
        return;
    }

    // R�initialiser l'historique pour permettre de re-notifier toutes les alertes
    m_alertesNotifiees.clear();

    // Recalculer le niveau d'alerte de chaque commande et mettre � jour le mod�le
    for (int row = 0; row < m_model->rowCount(); ++row) {
        ProductionCommande cmd = m_model->getCommande(row);
        cmd.alerteRetard = cmd.calculerAlerteRetard(m_joursAlerte, m_seuilAvancement);
        m_model->updateCommande(row, cmd);
    }

    qDebug() << "Recalcul termin� pour" << m_model->rowCount() << "commandes";
}
