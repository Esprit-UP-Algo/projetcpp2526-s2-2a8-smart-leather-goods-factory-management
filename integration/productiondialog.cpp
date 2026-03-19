#include "productiondialog.h"
#include "connection.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QDate>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION: ProductionCommande
// ═══════════════════════════════════════════════════════════════════════════

ProductionCommande::ProductionCommande()
    : idCommande(0)
    , ordrePassage(0)
    , avancement(0)
    , retard(false)
    , alerteRetard(Aucune)
    , montant(0.0)
{
}

int ProductionCommande::getJoursRetard() const
{
    if (!retard || !dateFinPrevue.isValid()) {
        return 0;
    }
    return dateFinPrevue.daysTo(QDate::currentDate());
}

// Calcul de l'alerte de retard
ProductionCommande::AlerteRetard ProductionCommande::calculerAlerteRetard(int joursAlerte, int seuilAvancement) const
{
    // Si la production est terminée, pas d'alerte
    if (etatProduction == "Terminé") {
        return Aucune;
    }
    
    // Si pas de date de livraison prévue, pas d'alerte
    if (!dateLivraisonPrevue.isValid()) {
        return Aucune;
    }
    
    QDate aujourdhui = QDate::currentDate();
    int joursRestants = aujourdhui.daysTo(dateLivraisonPrevue);
    
    // RETARD: Date dépassée et production non terminée
    if (joursRestants < 0) {
        return Retard;
    }
    
    // RISQUE: Proche de la date limite ET avancement insuffisant
    if (joursRestants <= joursAlerte) {
        // Calculer l'avancement attendu
        if (dateDebutPrevue.isValid() && dateFinPrevue.isValid()) {
            int dureeTotal = dateDebutPrevue.daysTo(dateFinPrevue);
            int joursEcoules = dateDebutPrevue.daysTo(aujourdhui);
            
            if (dureeTotal > 0 && joursEcoules > 0) {
                int avancementAttendu = (joursEcoules * 100) / dureeTotal;
                
                // Si l'avancement réel est inférieur au seuil ET à l'avancement attendu
                if (avancement < seuilAvancement && avancement < avancementAttendu) {
                    return Risque;
                }
            }
        }
        
        // Ou simplement si proche de la date et avancement < seuil
        if (avancement < seuilAvancement) {
            return Risque;
        }
    }
    
    return Aucune;
}

QString ProductionCommande::getAlerteRetardText() const
{
    switch (alerteRetard) {
        case Retard: return "⚠ RETARD";
        case Risque: return "⚡ RISQUE";
        case Aucune: return "✓ OK";
        default: return "?";
    }
}

QString ProductionCommande::getAlerteRetardColor() const
{
    switch (alerteRetard) {
        case Retard: return "#E74C3C";  // Rouge moderne
        case Risque: return "#F39C12";  // Orange moderne
        case Aucune: return "#27AE60";  // Vert moderne
        default: return "#95A5A6";      // Gris moderne
    }
}

// Validation des règles métier - Planification
bool ProductionCommande::validerPlanification() const
{
    // date_debut_prevue ≤ date_fin_prevue
    if (dateDebutPrevue.isValid() && dateFinPrevue.isValid()) {
        if (dateDebutPrevue > dateFinPrevue) {
            return false;
        }
    }
    
    // ordre_passage > 0
    if (ordrePassage <= 0) {
        return false;
    }
    
    return true;
}

// Validation des règles métier - Suivi
bool ProductionCommande::validerSuivi() const
{
    // avancement ∈ [0 ; 100]
    if (avancement < 0 || avancement > 100) {
        return false;
    }
    
    // si etat_production = Terminé → avancement = 100
    if (etatProduction == "Terminé" && avancement != 100) {
        return false;
    }
    
    // si etat_production = Planifié → avancement = 0
    if (etatProduction == "Planifié" && avancement != 0) {
        return false;
    }
    
    return true;
}

// Génération du message de notification
QString ProductionCommande::genererMessageNotification() const
{
    QString message;
    
    switch (alerteRetard) {
        case Retard:
            message = QString("⚠ RETARD - Commande %1\n"
                            "Date de livraison prévue: %2\n"
                            "Retard: %3 jour(s)\n"
                            "Avancement: %4%\n"
                            "État: %5")
                     .arg(reference)
                     .arg(dateLivraisonPrevue.toString("dd/MM/yyyy"))
                     .arg(-QDate::currentDate().daysTo(dateLivraisonPrevue))
                     .arg(avancement)
                     .arg(etatProduction);
            break;
            
        case Risque:
            message = QString("⚡ RISQUE DE RETARD - Commande %1\n"
                            "Date de livraison prévue: %2\n"
                            "Jours restants: %3\n"
                            "Avancement: %4%\n"
                            "État: %5")
                     .arg(reference)
                     .arg(dateLivraisonPrevue.toString("dd/MM/yyyy"))
                     .arg(QDate::currentDate().daysTo(dateLivraisonPrevue))
                     .arg(avancement)
                     .arg(etatProduction);
            break;
            
        default:
            message = QString("✓ Commande %1 - Dans les délais").arg(reference);
            break;
    }
    
    return message;
}

// ═══════════════════════════════════════════════════════════════════════════
// IMPLÉMENTATION: ProductionDialog
// ═══════════════════════════════════════════════════════════════════════════

ProductionDialog::ProductionDialog(QWidget *parent, DialogMode mode)
    : QDialog(parent), m_mode(mode)
{
    setupUI();
    setMinimumSize(620, 520);
    
    // Charger les employés APRÈS setupUI
    loadEmployes();

    bool readOnly = (mode == DeleteMode);
    for (auto *w : {(QWidget*)txtQuantite})
        w->setEnabled(!readOnly);
    for (auto *w : {cmbProduit, cmbStatut, cmbResponsable, cmbPriorite})
        w->setEnabled(!readOnly);
    dateDebut->setEnabled(!readOnly);
    dateFin->setEnabled(!readOnly);
    
    // La référence est toujours en lecture seule (auto-générée)
    txtReference->setEnabled(false);

    switch (mode) {
    case AddMode: {
        setWindowTitle("Créer une Commande");
        lblTitle->setText("+ Créer une Nouvelle Commande");
        // Générer une référence automatique basée sur auto-incrément
        generateAutoReference();
        txtId->setText("Auto");
        // Mettre "Admin Système" par défaut (index 0 après loadEmployes)
        cmbResponsable->setCurrentIndex(0);
        break;
    }
    case EditMode:
        setWindowTitle("Modifier une Commande");
        lblTitle->setText("✎ Modifier la Commande");
        btnSave->setText("Mettre à Jour");
        break;
    case DeleteMode:
        setWindowTitle("Supprimer une Commande");
        lblTitle->setText("⚠ Confirmer la Suppression");
        btnSave->setVisible(false);
        btnDelete->setVisible(true);
        lblDeleteWarning->setText(
            "⚠ ATTENTION : Vous êtes sur le point de supprimer cette commande.\n"
            "Cette action est irréversible.");
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

    auto *form = new QGridLayout(); form->setSpacing(12); form->setColumnStretch(1,1);
    int r = 0;
    auto addRow = [&](const QString &lbl, QWidget *w){
        form->addWidget(new QLabel(lbl,this), r, 0);
        form->addWidget(w, r++, 1);
    };

    txtId          = new QLineEdit(this); txtId->setReadOnly(true);
    txtReference   = new QLineEdit(this); txtReference->setPlaceholderText("Ex: PROD-2024-001");
    cmbProduit     = new QComboBox(this);
    cmbProduit->addItems({"Sac à Main Cuir","Portefeuille","Ceinture","Sacoche","Porte-documents","Sac à Dos"});
    txtQuantite    = new QLineEdit(this); txtQuantite->setPlaceholderText("Ex: 1500.00");
    cmbStatut      = new QComboBox(this);
    cmbStatut->addItems({"En Attente","En Cours","Terminé","Suspendu","Annulé"});
    dateDebut      = new QDateEdit(this);
    dateDebut->setCalendarPopup(true); dateDebut->setDate(QDate::currentDate());
    dateDebut->setDisplayFormat("dd/MM/yyyy");
    dateFin        = new QDateEdit(this);
    dateFin->setCalendarPopup(true); dateFin->setDate(QDate::currentDate().addDays(7));
    dateFin->setDisplayFormat("dd/MM/yyyy");
    cmbResponsable = new QComboBox(this);
    // Les employés seront chargés après l'initialisation complète
    cmbPriorite    = new QComboBox(this);
    cmbPriorite->addItems({"Basse","Normale","Haute","Urgente"});
    cmbPriorite->setCurrentIndex(1);

    addRow("ID Production :",    txtId);
    addRow("Référence * :",      txtReference);
    addRow("Produit * :",        cmbProduit);
    addRow("Prix * :",           txtQuantite);
    addRow("Statut :",           cmbStatut);
    addRow("Date Début * :",     dateDebut);
    addRow("Date Fin Prévue * :",dateFin);
    addRow("Employé * :",        cmbResponsable);
    addRow("Priorité :",         cmbPriorite);
    lay->addLayout(form);

    lblDeleteWarning = new QLabel(this);
    lblDeleteWarning->setStyleSheet("background:#f7d9d9;padding:12px;border-radius:6px;color:#8B0000;font-weight:bold;");
    lblDeleteWarning->setWordWrap(true); lblDeleteWarning->setVisible(false);
    lay->addWidget(lblDeleteWarning);
    lay->addStretch();

    static const QString BTN_PRIMARY =
        "QPushButton{background:#8D6E63;color:white;border:none;border-radius:7px;padding:9px 22px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#A0826D;}";
    static const QString BTN_DANGER  =
        "QPushButton{background:#D32F2F;color:white;border:none;border-radius:7px;padding:9px 22px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#F44336;}";
    static const QString BTN_CANCEL  =
        "QPushButton{background:#BCAAA4;color:white;border:none;border-radius:7px;padding:9px 22px;font-size:13px;font-weight:bold;}"
        "QPushButton:hover{background:#A78D78;}";

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
                                         const QString &dFin, const QString &responsable, const QString &priorite)
{
    txtId->setText(id);
    txtReference->setText(reference);
    txtQuantite->setText(quantite);
    auto setCombo = [](QComboBox *c, const QString &v){ int i=c->findText(v); if(i>=0) c->setCurrentIndex(i); };
    setCombo(cmbProduit, produit); setCombo(cmbStatut, statut);
    setCombo(cmbResponsable, responsable); setCombo(cmbPriorite, priorite);
    dateDebut->setDate(QDate::fromString(dDebut,"dd/MM/yyyy"));
    dateFin->setDate(QDate::fromString(dFin,"dd/MM/yyyy"));
}

QString ProductionDialog::getId()         const { return txtId->text(); }
QString ProductionDialog::getReference()  const { return txtReference->text(); }
QString ProductionDialog::getProduit()    const { return cmbProduit->currentText(); }
QString ProductionDialog::getQuantite()   const { return txtQuantite->text(); }
QString ProductionDialog::getStatut()     const { return cmbStatut->currentText(); }
QString ProductionDialog::getDateDebut()  const { return dateDebut->date().toString("dd/MM/yyyy"); }
QString ProductionDialog::getDateFin()    const { return dateFin->date().toString("dd/MM/yyyy"); }
QString ProductionDialog::getResponsable()const { return cmbResponsable->currentText(); }
QString ProductionDialog::getPriorite()   const { return cmbPriorite->currentText(); }

int ProductionDialog::getEmployeId() const 
{ 
    // Récupérer l'ID de l'employé depuis le currentData du ComboBox
    int id = cmbResponsable->currentData().toInt();
    qDebug() << "🔍 getEmployeId() appelé - ID retourné:" << id;
    qDebug() << "🔍 ComboBox index:" << cmbResponsable->currentIndex();
    qDebug() << "🔍 ComboBox text:" << cmbResponsable->currentText();
    return id;
}

void ProductionDialog::onSaveClicked()
{
    if (txtReference->text().isEmpty()) { QMessageBox::warning(this,"","La référence est obligatoire."); return; }
    if (txtQuantite->text().isEmpty())  { QMessageBox::warning(this,"","Le prix est obligatoire."); return; }
    bool ok; 
    double prix = txtQuantite->text().toDouble(&ok);
    if (!ok || prix <= 0) { QMessageBox::warning(this,"","Le prix doit être un nombre positif."); return; }
    if (dateDebut->date() > dateFin->date()) { QMessageBox::warning(this,"","Date début > date fin."); return; }
    QMessageBox::information(this,"Succès",
        m_mode==AddMode ? "Commande créée avec succès !" : "Commande mise à jour avec succès !");
    accept();
}

void ProductionDialog::onDeleteConfirmed()
{
    if (QMessageBox::question(this,"Confirmation","Êtes-vous sûr de vouloir supprimer cette commande ?",
            QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes) {
        QMessageBox::information(this,"Supprimé","Commande supprimée avec succès.");
        accept();
    }
}


// ═══════════════════════════════════════════════════════════════════════════
// Génération automatique de la référence basée sur auto-incrément
// ═══════════════════════════════════════════════════════════════════════════
void ProductionDialog::generateAutoReference()
{
    // Récupérer le dernier ID de commande depuis la base de données
    QSqlQuery query(Connection::instance()->getDatabase());
    query.prepare("SELECT NVL(MAX(ID_COMMANDE), 0) + 1 FROM COMMANDES");
    
    int nextId = 1;
    if (query.exec() && query.next()) {
        nextId = query.value(0).toInt();
    }
    
    // Générer la référence au format CMD-YYYY-NNNN
    QString ref = QString("CMD-%1-%2")
        .arg(QDate::currentDate().year())
        .arg(nextId, 4, 10, QChar('0'));  // Pad avec des zéros: 0001, 0002, etc.
    
    txtReference->setText(ref);
}

// ═══════════════════════════════════════════════════════════════════════════
// Chargement des employés depuis la base de données
// ═══════════════════════════════════════════════════════════════════════════
void ProductionDialog::loadEmployes()
{
    qDebug() << "=== Chargement des employés ===";
    
    QSqlDatabase db = Connection::instance()->getDatabase();
    if (!db.isOpen()) {
        qDebug() << "❌ Base de données non connectée!";
        QMessageBox::warning(this, "Erreur", "Impossible de charger les employés: base de données non connectée.");
        return;
    }
    
    QSqlQuery query(db);
    
    // Essayer différentes variantes de noms de colonnes
    QString sql = "SELECT id_employe, nom, prenom FROM employes ORDER BY nom, prenom";
    
    qDebug() << "Exécution de la requête:" << sql;
    
    if (!query.exec(sql)) {
        qDebug() << "❌ Erreur SQL:" << query.lastError().text();
        qDebug() << "Tentative avec majuscules...";
        
        // Essayer avec majuscules
        sql = "SELECT ID_EMPLOYE, NOM, PRENOM FROM EMPLOYES ORDER BY NOM, PRENOM";
        query.prepare(sql);
        
        if (!query.exec()) {
            qDebug() << "❌ Erreur SQL (majuscules):" << query.lastError().text();
            QMessageBox::warning(this, "Erreur", 
                "Impossible de charger les employés.\n\nErreur SQL: " + query.lastError().text());
            return;
        }
    }
    
    qDebug() << "✅ Requête exécutée avec succès";
    
    // Vider le ComboBox avant de le remplir
    cmbResponsable->clear();
    m_employeMap.clear();
    
    int count = 0;
    while (query.next()) {
        int id = query.value(0).toInt();
        QString nom = query.value(1).toString();
        QString prenom = query.value(2).toString();
        QString nomComplet = nom + " " + prenom;
        
        qDebug() << "Employé trouvé:" << id << "-" << nomComplet;
        
        // Ajouter au ComboBox avec l'ID comme data
        cmbResponsable->addItem(nomComplet, id);
        m_employeMap[id] = nomComplet;
        count++;
    }
    
    qDebug() << "✅ Total employés chargés:" << count;
    
    if (count == 0) {
        qDebug() << "⚠ Aucun employé trouvé dans la table EMPLOYES";
        QMessageBox::warning(this, "Attention", 
            "⚠ Aucun employé trouvé dans la base de données!\n\n"
            "Veuillez d'abord ajouter des employés avant de créer une commande.\n\n"
            "Exécutez le script SQL: AJOUTER_EMPLOYES_PAR_DEFAUT.sql");
        cmbResponsable->addItem("(Aucun employé disponible)", 0);
    }
}
