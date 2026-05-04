#include "pointagedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDate>
#include <QMessageBox>

PointageDialog::PointageDialog(int idEmploye,
                               const QString &nomEmploye,
                               QWidget *parent)
    : QDialog(parent)
    , m_idEmploye(idEmploye)
    , m_nomEmploye(nomEmploye)
{
    setupUI();
    applyStyles();
    setWindowTitle("📅 Calendrier Pointage - " + nomEmploye);
    setMinimumSize(700, 500);
    rafraichirCalendrier();
}

PointageDialog::~PointageDialog()
{}

void PointageDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(12);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Titre
    m_lblTitre = new QLabel("📅 Pointage de : " + m_nomEmploye, this);
    m_lblTitre->setObjectName("lblTitre");
    mainLayout->addWidget(m_lblTitre);
    
    // Filtres mois/année
    QHBoxLayout *filterLayout = new QHBoxLayout();
    
    filterLayout->addWidget(new QLabel("Mois :", this));
    m_comboMois = new QComboBox(this);
    QStringList mois = {"Janvier","Février","Mars","Avril","Mai","Juin",
                        "Juillet","Août","Septembre","Octobre","Novembre","Décembre"};
    m_comboMois->addItems(mois);
    m_comboMois->setCurrentIndex(QDate::currentDate().month() - 1);
    filterLayout->addWidget(m_comboMois);
    
    filterLayout->addWidget(new QLabel("Année :", this));
    m_spinAnnee = new QSpinBox(this);
    m_spinAnnee->setRange(2020, 2100);
    m_spinAnnee->setValue(QDate::currentDate().year());
    filterLayout->addWidget(m_spinAnnee);
    
    m_btnRefresh = new QPushButton("🔄 Actualiser", this);
    filterLayout->addWidget(m_btnRefresh);
    
    m_btnSalaire = new QPushButton("💰 Calculer Salaire", this);
    filterLayout->addWidget(m_btnSalaire);
    
    filterLayout->addStretch();
    mainLayout->addLayout(filterLayout);
    
    // Table calendrier
    m_tableView = new QTableView(this);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->setAlternatingRowColors(true);
    m_tableView->setShowGrid(true);
    m_tableView->setWordWrap(false);
    m_tableView->setMinimumHeight(300);
    mainLayout->addWidget(m_tableView);
    
    // Label salaire
    m_lblSalaire = new QLabel("", this);
    m_lblSalaire->setObjectName("lblSalaire");
    mainLayout->addWidget(m_lblSalaire);
    
    // Bouton fermer
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    m_btnFermer = new QPushButton("✖ Fermer", this);
    btnLayout->addWidget(m_btnFermer);
    mainLayout->addLayout(btnLayout);
    
    // Connexions
    connect(m_btnRefresh, &QPushButton::clicked,
            this, &PointageDialog::rafraichirCalendrier);
    connect(m_btnSalaire, &QPushButton::clicked,
            this, &PointageDialog::afficherSalaire);
    connect(m_btnFermer,  &QPushButton::clicked,
            this, &QDialog::accept);
}

void PointageDialog::applyStyles()
{
    setStyleSheet(
        "QDialog { background-color: #FFFFFF; }"
        "QLabel#lblTitre {"
        "  font-size: 18px; font-weight: bold; color: #5D4037; }"
        "QLabel#lblSalaire {"
        "  font-size: 15px; font-weight: bold; color: #2E7D32;"
        "  background: #E8F5E9; border-radius: 6px; padding: 8px; }"
        "QPushButton {"
        "  background-color: #8D6E63; color: white; border: none;"
        "  border-radius: 6px; padding: 8px 16px; font-weight: 600; }"
        "QPushButton:hover { background-color: #A0826D; }"
        "QTableView {"
        "  background-color: white;"
        "  alternate-background-color: #F5F5F5;"
        "  border: 1px solid #D7CCC8;"
        "  border-radius: 6px;"
        "  gridline-color: #E0E0E0;"
        "  selection-background-color: #BCAAA4;"
        "  selection-color: white;"
        "  color: #291C0E;"
        "}"
        "QHeaderView::section {"
        "  background-color: #8D6E63;"
        "  color: white;"
        "  font-weight: bold;"
        "  padding: 8px;"
        "  border: none;"
        "  border-right: 1px solid #6E473B;"
        "}"
    );
}

void PointageDialog::rafraichirCalendrier()
{
    int mois  = m_comboMois->currentIndex() + 1;
    int annee = m_spinAnnee->value();
    
    QSqlQueryModel *model =
        m_pointage.getCalendrierEmploye(m_idEmploye, mois, annee);
    
    if (model) {
        m_tableView->setModel(model);
        m_tableView->resizeColumnsToContents();
    }
    m_lblSalaire->setText("");
}

void PointageDialog::afficherSalaire()
{
    int mois  = m_comboMois->currentIndex() + 1;
    int annee = m_spinAnnee->value();
    
    double salaire = m_pointage.calculerSalaire(m_idEmploye, mois, annee);
    m_lblSalaire->setText(
        QString("💰 Salaire net %1/%2 : %3 DT")
            .arg(mois).arg(annee)
            .arg(salaire, 0, 'f', 2)
    );
}
