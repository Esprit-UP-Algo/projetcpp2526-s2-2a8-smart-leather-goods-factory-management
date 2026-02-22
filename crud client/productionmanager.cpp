#include "productionmanager.h"
#include "productiondialog.h"
#include <QMessageBox>
#include <QMenu>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QPrinter>
#include <QTextDocument>
#include <QScrollArea>
#include <QFrame>
#include <QGroupBox>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QCursor>
#include <QChart>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QPainter>
#include <QDebug>
#include <QTime>
#include <QDate>
#include <QDateTime>

ProductionManager::ProductionManager(QWidget *parent)
    : QObject(parent)
    , m_parent(parent)
    , m_table(nullptr)
    , m_statsTotal(nullptr)
    , m_statsEnProd(nullptr)
    , m_statsTerminees(nullptr)
    , m_statsMontant(nullptr)
{
}

void ProductionManager::setProductionTable(QTableWidget *table)
{
    m_table = table;
    if (m_table) {
        m_table->setColumnCount(9);
        m_table->setHorizontalHeaderLabels({
            "ID", "Référence", "Client", "Type", "Montant",
            "Date Création", "Date Livraison", "Statut", "Priorité"
        });
        m_table->setColumnHidden(0, true);
        m_table->verticalHeader()->setVisible(false);
        m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_table->setSelectionMode(QAbstractItemView::SingleSelection);
        
        int widths[] = {50,120,150,120,100,110,110,100,80};
        for (int i = 0; i < 9; ++i) {
            m_table->setColumnWidth(i, widths[i]);
        }
    }
}

void ProductionManager::setStatsLabels(QLabel *total, QLabel *enProd, QLabel *terminees, QLabel *montant)
{
    m_statsTotal = total;
    m_statsEnProd = enProd;
    m_statsTerminees = terminees;
    m_statsMontant = montant;
}

QString ProductionManager::cellText(int row, int col) const
{
    if (!m_table || row < 0 || row >= m_table->rowCount() || col < 0 || col >= m_table->columnCount())
        return QString();
    auto *item = m_table->item(row, col);
    return item ? item->text() : QString();
}

void ProductionManager::ajouterLigneTable(const QString &ref, const QString &client,
                                          const QString &type, const QString &montant,
                                          const QString &dc, const QString &dl,
                                          const QString &statut, const QString &priorite)
{
    if (!m_table) return;
    
    int row = m_table->rowCount();
    m_table->insertRow(row);
    m_table->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
    m_table->setItem(row, 1, new QTableWidgetItem(ref));
    m_table->setItem(row, 2, new QTableWidgetItem(client));
    m_table->setItem(row, 3, new QTableWidgetItem(type));
    m_table->setItem(row, 4, new QTableWidgetItem(montant));
    m_table->setItem(row, 5, new QTableWidgetItem(dc));
    m_table->setItem(row, 6, new QTableWidgetItem(dl));
    m_table->setItem(row, 7, new QTableWidgetItem(statut));
    m_table->setItem(row, 8, new QTableWidgetItem(priorite));
}

void ProductionManager::loadProductionData()
{
    if (!m_table) return;
    
    m_table->setRowCount(0);
    
    Production prod;
    QSqlQueryModel* model = prod.afficher();
    
    if (!model) {
        qDebug() << "❌ Erreur lors du chargement des commandes";
        return;
    }
    
    for (int i = 0; i < model->rowCount(); ++i) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        
        m_table->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
        m_table->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
        m_table->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
        m_table->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
        m_table->setItem(row, 4, new QTableWidgetItem(model->data(model->index(i, 4)).toString() + " DT"));
        m_table->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 5)).toDate().toString("dd/MM/yyyy")));
        m_table->setItem(row, 6, new QTableWidgetItem(model->data(model->index(i, 6)).toDate().toString("dd/MM/yyyy")));
        m_table->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
        m_table->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 8)).toString()));
    }
    
    delete model;
    updateStatsCards();
}

void ProductionManager::updateStatistics()
{
    updateStatsCards();
}

void ProductionManager::updateStatsCards()
{
    if (!m_table) return;
    
    int total = 0, enProd = 0, terminees = 0;
    double montantTotal = 0;
    
    for (int r = 0; r < m_table->rowCount(); ++r) {
        if (!cellText(r, 1).startsWith("PROD-")) continue;
        ++total;
        QString m = cellText(r, 4);
        montantTotal += m.remove(" DT").remove(",").replace(" ", "").toDouble();
        QString st = cellText(r, 7);
        if (st == "En Production") ++enProd;
        else if (st == "Terminé") ++terminees;
    }
    
    if (m_statsTotal) m_statsTotal->setText(QString::number(total));
    if (m_statsEnProd) m_statsEnProd->setText(QString::number(enProd));
    if (m_statsTerminees) m_statsTerminees->setText(QString::number(terminees));
    if (m_statsMontant) m_statsMontant->setText(QString::number(montantTotal, 'f', 2) + " DT");
}

void ProductionManager::creerProduction()
{
    ProductionDialog dlg(m_parent, ProductionDialog::AddMode);
    if (dlg.exec() == QDialog::Accepted) {
        qDebug() << "========== AJOUT COMMANDE ==========";
        
        Production prod;
        
        QString ref = dlg.getReference();
        if (ref.isEmpty()) {
            ref = QString("PROD-%1-%2")
                .arg(QDate::currentDate().year())
                .arg(QTime::currentTime().toString("HHmmss"));
        }
        
        prod.setReference(ref);
        prod.setType(dlg.getProduit());
        prod.setMontantHT(dlg.getQuantite().toDouble());
        
        QDate dateCreation = QDate::fromString(dlg.getDateDebut(), "dd/MM/yyyy");
        QDate dateLivraison = QDate::fromString(dlg.getDateFin(), "dd/MM/yyyy");
        
        if (!dateCreation.isValid()) dateCreation = QDate::currentDate();
        if (!dateLivraison.isValid()) dateLivraison = QDate::currentDate().addDays(30);
        
        prod.setDateCreation(dateCreation);
        prod.setDateLivraison(dateLivraison);
        prod.setStatut(dlg.getStatut());
        prod.setPriorite(dlg.getPriorite());
        
        if (prod.ajouter()) {
            qDebug() << "✅ Commande ajoutée avec succès";
            QMessageBox::information(m_parent, "Succès", "Commande ajoutée avec succès!");
            loadProductionData();
            emit productionDataChanged();
        } else {
            qDebug() << "❌ Échec de l'ajout";
            QMessageBox::critical(m_parent, "Erreur", "Erreur lors de l'ajout de la commande.");
        }
    }
}

void ProductionManager::modifierProduction()
{
    if (!m_table) return;
    
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(m_parent, "Attention", "Veuillez sélectionner une commande.");
        return;
    }
    
    QDialog d(m_parent);
    d.setWindowTitle("Modifier Commande");
    d.setMinimumSize(500, 450);
    d.setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel { color: #291C0E; font-size: 12px; font-weight: bold; }"
        "QLineEdit, QComboBox, QDateEdit { background-color: white; border: 2px solid #BCAAA4; "
        "border-radius: 6px; padding: 8px; font-size: 12px; color: #291C0E; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; border-radius: 6px; "
        "padding: 8px 20px; font-size: 12px; font-weight: bold; min-width: 80px; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    
    QVBoxLayout lay(&d);
    lay.setContentsMargins(20, 20, 20, 20);
    QFormLayout form;
    
    QLineEdit refE(cellText(row, 1), &d);
    QLineEdit clientE(cellText(row, 2), &d);
    QComboBox typeC(&d);
    typeC.addItems({"Standard", "Express", "Vente", "Devis"});
    typeC.setCurrentText(cellText(row, 3));
    
    QString montantStr = cellText(row, 4);
    montantStr.remove(" DT").remove(",").replace(" ", "");
    QLineEdit montantE(montantStr, &d);
    
    QDateEdit dcE(QDate::fromString(cellText(row, 5), "dd/MM/yyyy"), &d);
    dcE.setCalendarPopup(true);
    dcE.setDisplayFormat("dd/MM/yyyy");
    
    QDateEdit dlE(QDate::fromString(cellText(row, 6), "dd/MM/yyyy"), &d);
    dlE.setCalendarPopup(true);
    dlE.setDisplayFormat("dd/MM/yyyy");
    
    QComboBox statC(&d);
    statC.addItems({"En Attente", "Planifié", "En Production", "Terminé"});
    statC.setCurrentText(cellText(row, 7));
    
    QComboBox prioC(&d);
    prioC.addItems({"Basse", "Normale", "Haute", "Urgente"});
    prioC.setCurrentText(cellText(row, 8));
    
    form.addRow("Référence *:", &refE);
    form.addRow("Client *:", &clientE);
    form.addRow("Type:", &typeC);
    form.addRow("Montant HT *:", &montantE);
    form.addRow("Date Création:", &dcE);
    form.addRow("Date Livraison:", &dlE);
    form.addRow("Statut:", &statC);
    form.addRow("Priorité:", &prioC);
    lay.addLayout(&form);
    
    QHBoxLayout btns;
    QPushButton ok("Enregistrer", &d), cancel("Annuler", &d);
    cancel.setStyleSheet("QPushButton{background:#95877C;}");
    btns.addStretch();
    btns.addWidget(&ok);
    btns.addWidget(&cancel);
    lay.addLayout(&btns);
    
    connect(&ok, &QPushButton::clicked, &d, &QDialog::accept);
    connect(&cancel, &QPushButton::clicked, &d, &QDialog::reject);
    
    if (d.exec() == QDialog::Accepted) {
        if (refE.text().isEmpty() || clientE.text().isEmpty() || montantE.text().isEmpty()) {
            QMessageBox::warning(m_parent, "Attention", "Champs obligatoires manquants.");
            return;
        }
        
        Production prod;
        prod.setId(cellText(row, 0).toInt());
        prod.setReference(refE.text());
        prod.setClient(clientE.text());
        prod.setType(typeC.currentText());
        prod.setMontantHT(montantE.text().toDouble());
        prod.setDateCreation(dcE.date());
        prod.setDateLivraison(dlE.date());
        prod.setStatut(statC.currentText());
        prod.setPriorite(prioC.currentText());
        
        if (prod.modifier()) {
            QMessageBox::information(m_parent, "Succès", "Commande modifiée avec succès!");
            loadProductionData();
            emit productionDataChanged();
        } else {
            QMessageBox::critical(m_parent, "Erreur", "Erreur lors de la modification de la commande.");
        }
    }
}

void ProductionManager::supprimerProduction()
{
    if (!m_table) return;
    
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(m_parent, "Attention", "Sélectionnez une commande.");
        return;
    }
    
    QString ref = cellText(row, 1);
    QString client = cellText(row, 2);
    int id = cellText(row, 0).toInt();
    
    if (QMessageBox::question(m_parent, "Confirmer",
            QString("Supprimer la commande %1 du client %2 ?").arg(ref, client),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
        
        Production prod;
        if (prod.supprimer(id)) {
            QMessageBox::information(m_parent, "Succès", "Commande supprimée avec succès!");
            loadProductionData();
            emit productionDataChanged();
        } else {
            QMessageBox::critical(m_parent, "Erreur", "Erreur lors de la suppression de la commande.");
        }
    }
}

void ProductionManager::rechercherProduction(const QString &text)
{
    if (!m_table) return;
    
    if (text.isEmpty()) {
        loadProductionData();
        return;
    }
    
    m_table->setRowCount(0);
    
    Production prod;
    QSqlQueryModel* model = prod.rechercher(text);
    
    if (!model) {
        qDebug() << "❌ Erreur lors de la recherche des commandes";
        return;
    }
    
    for (int i = 0; i < model->rowCount(); ++i) {
        int row = m_table->rowCount();
        m_table->insertRow(row);
        
        m_table->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
        m_table->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
        m_table->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
        m_table->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
        m_table->setItem(row, 4, new QTableWidgetItem(model->data(model->index(i, 4)).toString() + " DT"));
        m_table->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 5)).toDate().toString("dd/MM/yyyy")));
        m_table->setItem(row, 6, new QTableWidgetItem(model->data(model->index(i, 6)).toDate().toString("dd/MM/yyyy")));
        m_table->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
        m_table->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 8)).toString()));
    }
    
    delete model;
    updateStatsCards();
}

void ProductionManager::trierProduction()
{
    if (!m_table) return;
    
    QMenu menu(m_parent);
    menu.setStyleSheet(
        "QMenu{background:#FAF5F0;border:2px solid #8D6E63;border-radius:6px;padding:8px;}"
        "QMenu::item{padding:8px 25px;color:#291C0E;border-radius:4px;}"
        "QMenu::item:selected{background:#8D6E63;color:white;}"
        "QMenu::separator{height:2px;background:#BCAAA4;margin:5px 10px;}"
    );

    auto addSortOptions = [&](const QString &label, const QString &colName) {
        QMenu *sub = menu.addMenu("📋 " + label);
        sub->setStyleSheet(menu.styleSheet());
        auto *asc = sub->addAction("↑ Croissant (A → Z)");
        auto *desc = sub->addAction("↓ Décroissant (Z → A)");
        
        connect(asc, &QAction::triggered, [=]() {
            m_table->setRowCount(0);
            Production prod;
            QSqlQueryModel* model = prod.trierPar(colName + " ASC");
            if (model) {
                for (int i = 0; i < model->rowCount(); ++i) {
                    int row = m_table->rowCount();
                    m_table->insertRow(row);
                    m_table->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
                    m_table->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
                    m_table->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
                    m_table->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
                    m_table->setItem(row, 4, new QTableWidgetItem(model->data(model->index(i, 4)).toString() + " DT"));
                    m_table->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 5)).toDate().toString("dd/MM/yyyy")));
                    m_table->setItem(row, 6, new QTableWidgetItem(model->data(model->index(i, 6)).toDate().toString("dd/MM/yyyy")));
                    m_table->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
                    m_table->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 8)).toString()));
                }
                delete model;
            }
        });
        
        connect(desc, &QAction::triggered, [=]() {
            m_table->setRowCount(0);
            Production prod;
            QSqlQueryModel* model = prod.trierPar(colName + " DESC");
            if (model) {
                for (int i = 0; i < model->rowCount(); ++i) {
                    int row = m_table->rowCount();
                    m_table->insertRow(row);
                    m_table->setItem(row, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));
                    m_table->setItem(row, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));
                    m_table->setItem(row, 2, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));
                    m_table->setItem(row, 3, new QTableWidgetItem(model->data(model->index(i, 3)).toString()));
                    m_table->setItem(row, 4, new QTableWidgetItem(model->data(model->index(i, 4)).toString() + " DT"));
                    m_table->setItem(row, 5, new QTableWidgetItem(model->data(model->index(i, 5)).toDate().toString("dd/MM/yyyy")));
                    m_table->setItem(row, 6, new QTableWidgetItem(model->data(model->index(i, 6)).toDate().toString("dd/MM/yyyy")));
                    m_table->setItem(row, 7, new QTableWidgetItem(model->data(model->index(i, 7)).toString()));
                    m_table->setItem(row, 8, new QTableWidgetItem(model->data(model->index(i, 8)).toString()));
                }
                delete model;
            }
        });
    };

    addSortOptions("Référence", "REFERENCE");
    addSortOptions("Type", "TYPE");
    addSortOptions("Montant", "MONTANT");
    menu.addSeparator();
    addSortOptions("Date Création", "DATE_CREATION");
    addSortOptions("Date Livraison", "DATE_LIVRAISON");
    menu.addSeparator();
    addSortOptions("Statut", "STATUT");
    addSortOptions("Priorité", "PRIORITE");

    menu.exec(QCursor::pos());
}

void ProductionManager::showContextMenu(const QPoint &pos)
{
    if (!m_table) return;
    
    QMenu menu(m_parent);
    menu.setStyleSheet("QMenu{background:#FAF5F0;border:1px solid #BCAAA4;}"
                       "QMenu::item{padding:7px 22px;color:#291C0E;}"
                       "QMenu::item:selected{background:#8D6E63;color:white;}");
    auto *voir    = menu.addAction("Voir suivi");
    auto *modif   = menu.addAction("Modifier");
    menu.addSeparator();
    auto *supp    = menu.addAction("Supprimer");
    menu.addSeparator();
    auto *fact    = menu.addAction("Générer facture");
    auto *stats   = menu.addAction("Statistiques");

    auto *act = menu.exec(m_table->viewport()->mapToGlobal(pos));
    if      (act == voir)  afficherSuivi();
    else if (act == modif) modifierProduction();
    else if (act == supp)  supprimerProduction();
    else if (act == fact)  genererFacture();
    else if (act == stats) afficherStatistiques();
}

// Les autres méthodes (afficherSuivi, afficherPlanification, genererFacture, etc.)
// seront implémentées dans la partie 2 du fichier...

void ProductionManager::afficherSuivi()
{
    if (!m_table) return;
    
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(m_parent, "", "Sélectionnez une commande.");
        return;
    }
    
    QString ref = cellText(row, 1);
    QString client = cellText(row, 2);
    QString statut = cellText(row, 7);
    QString dc = cellText(row, 5);
    QString dl = cellText(row, 6);
    QString prio = cellText(row, 8);

    QString html = "<html><head><style>"
        "body{font-family:Arial;padding:10px;background:#FAFAFA;}"
        ".item{margin:12px 0;padding:12px;background:#FFF;border-left:4px solid #8D6E63;border-radius:6px;}"
        ".date{color:#8D6E63;font-weight:bold;font-size:12px;}"
        ".status{font-size:15px;font-weight:bold;margin:5px 0;}"
        ".desc{color:#666;font-size:12px;}"
        "</style></head><body>";

    auto step = [&](const QString &date, const QString &status, const QString &desc) {
        html += "<div class='item'><div class='date'>" + date + "</div>"
                "<div class='status'>" + status + "</div>"
                "<div class='desc'>" + desc + "</div></div>";
    };
    
    step(dc, "✓ Commande créée", "La commande a été enregistrée dans le système");
    if (statut == "Planifié" || statut == "En Production" || statut == "Terminé")
        step(QDate::fromString(dc, "dd/MM/yyyy").addDays(1).toString("dd/MM/yyyy"),
             "✓ Planifiée", "La commande a été planifiée pour la production");
    if (statut == "En Production" || statut == "Terminé")
        step(QDate::fromString(dc, "dd/MM/yyyy").addDays(3).toString("dd/MM/yyyy"),
             "✓ En production", "La commande est en cours de fabrication");
    if (statut == "Terminé")
        step(dl, "✓ Terminée", "La commande a été terminée et livrée avec succès");
    else
        step(dl + " (Prévu)", "◷ Livraison prévue", "Date de livraison estimée");
    
    html += "</body></html>";

    QDialog dlg(m_parent);
    dlg.setWindowTitle("Suivi - " + ref);
    dlg.setMinimumSize(650, 600);
    QVBoxLayout lay(&dlg);
    lay.setContentsMargins(20, 20, 20, 20);
    
    auto *info = new QLabel(QString("<b>Ref:</b> %1 | <b>Client:</b> %2 | <b>Priorité:</b> %3")
                            .arg(ref, client, prio));
    info->setStyleSheet("background:#F8F8F8;border-radius:6px;padding:10px;");
    lay.addWidget(info);
    
    auto *view = new QTextEdit(&dlg);
    view->setReadOnly(true);
    view->setHtml(html);
    lay.addWidget(view);
    
    QPushButton close("Fermer", &dlg);
    close.setStyleSheet("QPushButton{background:#8D6E63;color:white;border:none;border-radius:6px;"
                       "padding:8px 20px;font-size:12px;font-weight:bold;}"
                       "QPushButton:hover{background:#A0826D;}");
    connect(&close, &QPushButton::clicked, &dlg, &QDialog::accept);
    
    QHBoxLayout bl;
    bl.addStretch();
    bl.addWidget(&close);
    lay.addLayout(&bl);
    
    dlg.exec();
}

void ProductionManager::afficherPlanification()
{
    QDialog dlg(m_parent);
    dlg.setWindowTitle("Planification & Suivi Production");
    dlg.setMinimumSize(900, 600);
    dlg.setStyleSheet("QDialog { background-color: #FAF5F0; }");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&dlg);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    
    QLabel *title = new QLabel("📅 PLANIFICATION DE LA PRODUCTION");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size:18px;font-weight:bold;color:#8D6E63;padding:10px;");
    mainLayout->addWidget(title);
    
    QLabel *info = new QLabel("Fonctionnalité de planification avancée disponible prochainement.");
    info->setAlignment(Qt::AlignCenter);
    info->setStyleSheet("color:#666;padding:20px;");
    mainLayout->addWidget(info);
    
    QPushButton *btnClose = new QPushButton("Fermer", &dlg);
    btnClose->setStyleSheet(
        "QPushButton { background-color: #8D6E63; color: white; border: none; "
        "border-radius: 6px; padding: 10px 30px; font-size: 13px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A0826D; }"
    );
    connect(btnClose, &QPushButton::clicked, &dlg, &QDialog::accept);
    
    QHBoxLayout *closeLayout = new QHBoxLayout();
    closeLayout->addStretch();
    closeLayout->addWidget(btnClose);
    closeLayout->addStretch();
    mainLayout->addLayout(closeLayout);
    
    dlg.exec();
}

void ProductionManager::genererFacture()
{
    if (!m_table) return;
    
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::warning(m_parent, "", "Sélectionnez une commande.");
        return;
    }
    
    QString ref = cellText(row, 1);
    QString client = cellText(row, 2);
    QString montant = cellText(row, 4);
    QString dc = cellText(row, 5);
    double ht = QString(montant).remove(" DT").remove(",").replace(" ", "").toDouble();
    double tva = ht * 0.19;
    double ttc = ht + tva;

    QString html = QString(
        "<html><body style='font-family:Arial;color:#291C0E;padding:20px;'>"
        "<h1 style='color:#8D6E63;text-align:center;'>FACTURE</h1>"
        "<p><b>Référence:</b> %1</p><p><b>Client:</b> %2</p><p><b>Date:</b> %3</p>"
        "<hr><table width='100%%'><tr><th>Montant HT</th><th>TVA 19%%</th><th>Total TTC</th></tr>"
        "<tr><td>%4 DT</td><td>%5 DT</td><td><b>%6 DT</b></td></tr></table></body></html>"
    ).arg(ref, client, dc,
          QString::number(ht, 'f', 2),
          QString::number(tva, 'f', 2),
          QString::number(ttc, 'f', 2));

    QDialog dlg(m_parent);
    dlg.setWindowTitle("Facture - " + ref);
    dlg.setMinimumSize(700, 500);
    QVBoxLayout lay(&dlg);
    lay.setContentsMargins(20, 20, 20, 20);
    
    auto *view = new QTextEdit(&dlg);
    view->setReadOnly(true);
    view->setHtml(html);
    lay.addWidget(view);

    QHBoxLayout btns;
    QPushButton pdf("Exporter PDF", &dlg), close("Fermer", &dlg);
    close.setStyleSheet("QPushButton{background:#95877C;color:white;border:none;border-radius:6px;"
                       "padding:8px 20px;font-size:12px;font-weight:bold;}"
                       "QPushButton:hover{background:#A78D78;}");
    pdf.setStyleSheet("QPushButton{background:#8D6E63;color:white;border:none;border-radius:6px;"
                     "padding:8px 20px;font-size:12px;font-weight:bold;}"
                     "QPushButton:hover{background:#A0826D;}");
    btns.addStretch();
    btns.addWidget(&pdf);
    btns.addWidget(&close);
    lay.addLayout(&btns);

    connect(&pdf, &QPushButton::clicked, [&]() {
        QString fn = QFileDialog::getSaveFileName(&dlg, "Enregistrer", "Facture_" + ref + ".pdf", "PDF (*.pdf)");
        if (!fn.isEmpty()) {
            QPrinter p(QPrinter::HighResolution);
            p.setOutputFormat(QPrinter::PdfFormat);
            p.setOutputFileName(fn);
            p.setPageSize(QPageSize::A4);
            QTextDocument doc;
            doc.setHtml(html);
            doc.print(&p);
            QMessageBox::information(&dlg, "", "Facture exportée: " + fn);
        }
    });
    connect(&close, &QPushButton::clicked, &dlg, &QDialog::accept);
    
    dlg.exec();
}

void ProductionManager::exporterBilanFinancier()
{
    QMessageBox::information(m_parent, "Export Bilan", 
        "Fonctionnalité d'export du bilan financier disponible prochainement.");
}

void ProductionManager::afficherStatistiques()
{
    if (!m_table) return;
    
    int total = 0;
    double montantTotal = 0;
    QMap<QString, int> statCnt, prioCnt;

    for (int r = 0; r < m_table->rowCount(); ++r) {
        if (!cellText(r, 1).startsWith("PROD-")) continue;
        ++total;
        montantTotal += QString(cellText(r, 4)).remove(" DT").remove(",").replace(" ", "").toDouble();
        statCnt[cellText(r, 7)]++;
        prioCnt[cellText(r, 8)]++;
    }
    
    if (total == 0) {
        QMessageBox::information(m_parent, "", "Aucune donnée de production.");
        return;
    }

    QDialog dlg(m_parent);
    dlg.setWindowTitle("Statistiques de Production");
    dlg.setMinimumSize(1000, 700);
    
    QScrollArea *scroll = new QScrollArea(&dlg);
    scroll->setWidgetResizable(true);
    QWidget *inner = new QWidget;
    QVBoxLayout *lay = new QVBoxLayout(inner);
    lay->setSpacing(15);
    lay->setContentsMargins(20, 20, 20, 20);

    // KPI row
    auto makeKPI = [](const QString &icon, const QString &val, const QString &lbl, const QString &color) {
        auto *w = new QWidget;
        w->setStyleSheet(QString("background:white;border-radius:8px;border:2px solid %1;").arg(color));
        auto *l = new QVBoxLayout(w);
        auto *ic = new QLabel(icon);
        ic->setStyleSheet("font-size:32px;");
        ic->setAlignment(Qt::AlignCenter);
        auto *vl = new QLabel(val);
        vl->setStyleSheet(QString("font-size:28px;font-weight:bold;color:%1;").arg(color));
        vl->setAlignment(Qt::AlignCenter);
        auto *lb = new QLabel(lbl);
        lb->setStyleSheet("font-size:13px;color:#666;");
        lb->setAlignment(Qt::AlignCenter);
        l->addWidget(ic);
        l->addWidget(vl);
        l->addWidget(lb);
        return w;
    };
    
    int terminees = statCnt.value("Terminé", 0);
    double taux = total > 0 ? 100.0 * terminees / total : 0;
    
    auto *kpiRow = new QWidget;
    auto *kpiLay = new QHBoxLayout(kpiRow);
    kpiLay->setSpacing(12);
    kpiLay->addWidget(makeKPI("📦", QString::number(total), "Total Commandes", "#4CAF50"));
    kpiLay->addWidget(makeKPI("💰", QString::number(montantTotal, 'f', 0) + " DT", "Chiffre d'Affaires", "#2196F3"));
    kpiLay->addWidget(makeKPI("📊", QString::number(total > 0 ? montantTotal / total : 0, 'f', 0) + " DT", "Montant Moyen", "#FF9800"));
    kpiLay->addWidget(makeKPI("✅", QString::number(taux, 'f', 1) + "%", "Taux Complétion", "#9C27B0"));
    lay->addWidget(kpiRow);

    // Charts
    auto *chartsRow = new QWidget;
    auto *chartsLay = new QHBoxLayout(chartsRow);
    chartsLay->setSpacing(12);

    auto *pie = new QPieSeries();
    QMap<QString, QColor> clrs;
    clrs["Terminé"] = QColor("#4CAF50");
    clrs["En Production"] = QColor("#FF9800");
    clrs["Planifié"] = QColor("#2196F3");
    clrs["En Attente"] = QColor("#FFC107");
    
    for (auto it = statCnt.begin(); it != statCnt.end(); ++it) {
        auto *sl = pie->append(it.key(), it.value());
        sl->setLabelVisible(true);
        sl->setLabel(QString("%1: %2").arg(it.key()).arg(it.value()));
        if (clrs.contains(it.key())) sl->setColor(clrs[it.key()]);
    }
    
    auto *pc = new QChart();
    pc->addSeries(pie);
    pc->setTitle("Répartition Statuts");
    pc->setAnimationOptions(QChart::SeriesAnimations);
    pc->setBackgroundBrush(Qt::white);
    auto *pv = new QChartView(pc);
    pv->setRenderHint(QPainter::Antialiasing);
    pv->setMinimumHeight(300);
    chartsLay->addWidget(pv);

    auto *bs = new QBarSet("Commandes");
    bs->setColor(QColor("#8D6E63"));
    QStringList cats;
    QStringList priorites = {"Urgente", "Haute", "Moyenne", "Basse"};
    for (const QString &p : priorites)
        if (prioCnt.contains(p)) {
            *bs << prioCnt[p];
            cats << p;
        }
    
    auto *bar = new QBarSeries();
    bar->append(bs);
    auto *bc = new QChart();
    bc->addSeries(bar);
    bc->setTitle("Répartition Priorités");
    bc->setAnimationOptions(QChart::SeriesAnimations);
    bc->setBackgroundBrush(Qt::white);
    auto *axX = new QBarCategoryAxis();
    axX->append(cats);
    bc->addAxis(axX, Qt::AlignBottom);
    bar->attachAxis(axX);
    auto *axY = new QValueAxis();
    axY->setLabelFormat("%d");
    bc->addAxis(axY, Qt::AlignLeft);
    bar->attachAxis(axY);
    bc->legend()->setVisible(false);
    auto *bv = new QChartView(bc);
    bv->setRenderHint(QPainter::Antialiasing);
    bv->setMinimumHeight(300);
    chartsLay->addWidget(bv);

    lay->addWidget(chartsRow);

    QPushButton exportBtn("Exporter CSV", inner), closeBtn("Fermer", inner);
    closeBtn.setStyleSheet("QPushButton{background:#95877C;color:white;border:none;border-radius:6px;"
                          "padding:8px 20px;font-size:12px;font-weight:bold;}"
                          "QPushButton:hover{background:#A78D78;}");
    exportBtn.setStyleSheet("QPushButton{background:#8D6E63;color:white;border:none;border-radius:6px;"
                           "padding:8px 20px;font-size:12px;font-weight:bold;}"
                           "QPushButton:hover{background:#A0826D;}");
    connect(&exportBtn, &QPushButton::clicked, [this]() { exporterBilanFinancier(); });
    connect(&closeBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    
    QHBoxLayout btnRow;
    btnRow.addStretch();
    btnRow.addWidget(&exportBtn);
    btnRow.addWidget(&closeBtn);
    lay->addLayout(&btnRow);

    scroll->setWidget(inner);
    QVBoxLayout *dlay = new QVBoxLayout(&dlg);
    dlay->addWidget(scroll);
    dlg.exec();
}
