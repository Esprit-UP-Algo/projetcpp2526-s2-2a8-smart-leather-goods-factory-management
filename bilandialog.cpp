#include "bilandialog.h"
#include "connection.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QLocale>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QHeaderView>
#include <QFont>
#include <QPainter>
#include <QMap>
#include <QDebug>
#include <algorithm>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QHorizontalBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QtCharts/QSplineSeries>

// Qt6: QtCharts n'a plus de namespace, les classes sont directement accessibles
typedef QChart               QCChart;
typedef QChartView           QCChartView;
typedef QPieSeries           QCPieSeries;
typedef QPieSlice            QCPieSlice;
typedef QBarSeries           QCBarSeries;
typedef QHorizontalBarSeries QCHBarSeries;
typedef QBarSet              QCBarSet;
typedef QBarCategoryAxis     QCBarCategoryAxis;
typedef QValueAxis           QCValueAxis;
typedef QSplineSeries        QCSplineSeries;

static const QString BG_PAGE  = "#FBF5F0";
static const QString BG_CARD  = "#FBF5F0";
static const QString ACCENT   = "#C4923A";   // Or chaud
static const QString PRIMARY  = "#6B2737";   // Bordeaux profond
static const QString DARK_TXT = "#6B2737";
static const QString BORDER   = "#C4923A";
static const QString BG_ALT   = "#F5EBE0";

static QString fmt(double v) {
    return QLocale(QLocale::French).toString(v, 'f', 2) + " DT";
}

// Macro helper pour récupérer le QChart* depuis un QWidget* stocké
static QCChart *chartOf(QWidget *w) {
    auto *cv = qobject_cast<QCChartView*>(w);
    return cv ? cv->chart() : nullptr;
}

BilanDialog::BilanDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Bilan Financier — CUIREA");
    setMinimumSize(1100, 720);
    setStyleSheet(QString(
        "QDialog{background:%1}"
        "QGroupBox{background:%2;border:none;border-left:3px solid %3;"
         "border-radius:8px;padding:8px}"
        "QGroupBox::title{color:%4;font-size:10px;font-weight:bold;"
         "subcontrol-origin:margin;left:10px;padding:0 4px}"
        "QComboBox{background:%3;color:white;border:none;border-radius:6px;"
         "padding:6px 12px;font-size:12px;font-weight:bold}"
        "QComboBox::drop-down{border:none}"
        "QComboBox QAbstractItemView{background:white;color:%4;"
         "selection-background-color:%3}"
        "QPushButton{border-radius:7px;padding:8px 22px;font-size:12px;font-weight:bold}"
        "QScrollBar:vertical{width:8px;background:#E8D8C4;border-radius:4px}"
        "QScrollBar::handle:vertical{background:%3;border-radius:4px}"
    ).arg(BG_PAGE, BG_CARD, ACCENT, PRIMARY));
    setupUI();
    loadData();
}

void BilanDialog::setupUI()
{
    auto *rootLay = new QVBoxLayout(this);
    rootLay->setContentsMargins(0,0,0,0);
    rootLay->setSpacing(0);

    // Barre supérieure terracotta
    auto *topBar = new QFrame();
    topBar->setStyleSheet(QString("QFrame{background:%1}").arg(PRIMARY));
    auto *topLay = new QHBoxLayout(topBar);
    topLay->setContentsMargins(20,12,20,12);
    topLay->setSpacing(16);

    auto *titleLbl = new QLabel("BILAN FINANCIER");
    titleLbl->setStyleSheet("font-size:18px;font-weight:bold;color:white;letter-spacing:2px");
    m_lblDate = new QLabel();
    m_lblDate->setStyleSheet(QString("font-size:11px;color:%1;font-weight:bold").arg(ACCENT));
    m_comboPeriod = new QComboBox();
    m_comboPeriod->setFixedWidth(180);
    connect(m_comboPeriod, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &BilanDialog::onPeriodChanged);

    topLay->addWidget(titleLbl);
    topLay->addStretch();
    topLay->addWidget(m_lblDate);
    topLay->addWidget(m_comboPeriod);
    rootLay->addWidget(topBar);

    // Zone scrollable
    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet(QString("QScrollArea{background:%1;border:none}").arg(BG_PAGE));

    auto *content = new QWidget();
    content->setStyleSheet(QString("background:%1").arg(BG_PAGE));
    auto *mainLay = new QVBoxLayout(content);
    mainLay->setContentsMargins(20,20,20,20);
    mainLay->setSpacing(16);

    // KPIs + Charts
    auto *midRow = new QHBoxLayout();
    midRow->setSpacing(16);

    auto *kpiCol = new QVBoxLayout();
    kpiCol->setSpacing(12);
    kpiCol->addWidget(makeKpiBox("Chiffre d'affaires total", m_lblCA));
    kpiCol->addWidget(makeKpiBox("Meilleur produit du mois", m_lblBestProduct, true));
    kpiCol->addWidget(makeKpiBox("Bénéfice net",             m_lblBenefice));
    kpiCol->addWidget(makeKpiBox("Marge brute",              m_lblMarge));
    kpiCol->addWidget(makeKpiBox("Commandes totales",        m_lblCommandes));
    kpiCol->addStretch();
    auto *kpiWidget = new QWidget();
    kpiWidget->setLayout(kpiCol);
    kpiWidget->setFixedWidth(240);

    // make*Chart() retournent QWidget* — pas de problème de type
    m_pieView    = makePieChart();
    m_barView    = makeBarChart();
    m_splineView = makeSplineChart();
    m_produitView = makeHBarChart("CA par produit");
    m_regionView  = makeHBarChart("CA par région");
    m_pieView->setMinimumSize(260,220);
    m_barView->setMinimumSize(260,220);
    m_splineView->setMinimumHeight(200);
    m_produitView->setMinimumHeight(220);
    m_regionView->setMinimumHeight(220);

    auto *topCharts = new QHBoxLayout();
    topCharts->setSpacing(12);
    topCharts->addWidget(m_pieView, 1);
    topCharts->addWidget(m_barView, 1);

    auto *botCharts = new QHBoxLayout();
    botCharts->setSpacing(12);
    botCharts->addWidget(m_produitView, 1);
    botCharts->addWidget(m_regionView, 1);

    auto *chartsCol = new QVBoxLayout();
    chartsCol->setSpacing(12);
    chartsCol->addLayout(topCharts);
    chartsCol->addWidget(m_splineView);
    chartsCol->addLayout(botCharts);

    midRow->addWidget(kpiWidget);
    midRow->addLayout(chartsCol, 1);
    mainLay->addLayout(midRow);



    scroll->setWidget(content);
    rootLay->addWidget(scroll, 1);

    // Boutons
    auto *btnBar = new QFrame();
    btnBar->setStyleSheet(
        QString("QFrame{background:%1;border-top:2px solid %2}").arg(BG_CARD, ACCENT));
    auto *btnLay = new QHBoxLayout(btnBar);
    btnLay->setContentsMargins(20,10,20,10);
    btnLay->setSpacing(10);

    auto *btnExport = new QPushButton("Exporter CSV");
    btnExport->setStyleSheet(
        QString("QPushButton{background:%1;color:white;border:none}"
                "QPushButton:hover{background:#A87730}").arg(ACCENT));
    auto *btnClose = new QPushButton("Fermer");
    btnClose->setStyleSheet(
        QString("QPushButton{background:%1;color:white;border:none}"
                "QPushButton:hover{background:#4E1A27}").arg(PRIMARY));

    connect(btnExport, &QPushButton::clicked, this, &BilanDialog::exportCSV);
    connect(btnClose,  &QPushButton::clicked, this, &QDialog::accept);
    btnLay->addStretch();
    btnLay->addWidget(btnExport);
    btnLay->addWidget(btnClose);
    rootLay->addWidget(btnBar);
}

QGroupBox *BilanDialog::makeKpiBox(const QString &title, QLabel *&valLabel, bool highlight)
{
    auto *box = new QGroupBox(title);
    if (highlight) {
        box->setStyleSheet(
            QString("QGroupBox{background:%1;border:none;border-radius:8px;padding:8px}"
                    "QGroupBox::title{color:%2;font-size:10px;font-weight:bold;"
                    "subcontrol-origin:margin;left:10px;padding:0 4px}").arg(PRIMARY, ACCENT));
    }
    auto *lay = new QVBoxLayout(box);
    lay->setContentsMargins(10,18,10,10);
    valLabel = new QLabel("—");
    valLabel->setAlignment(Qt::AlignCenter);
    valLabel->setWordWrap(true);
    valLabel->setStyleSheet(
        QString("font-size:22px;font-weight:bold;color:%1;border:none")
        .arg(highlight ? ACCENT : PRIMARY));
    lay->addWidget(valLabel);
    return box;
}

// Retourne QWidget* — QChartView hérite de QWidget, cast implicite valide
QWidget *BilanDialog::makePieChart()
{
    auto *series = new QCPieSeries();
    series->append("Basse",   1)->setColor(QColor("#C4923A"));  // Or
    series->append("Normale", 1)->setColor(QColor("#A0485A"));  // Bordeaux rosé
    series->append("Urgente", 1)->setColor(QColor("#6B2737"));  // Bordeaux profond

    auto *chart = new QCChart();
    chart->addSeries(series);
    chart->setTitle("Répartition par priorité");
    chart->setTitleFont(QFont("Arial", 9, QFont::Bold));
    chart->setTitleBrush(QColor(DARK_TXT));
    chart->setBackgroundBrush(QColor(BG_CARD));
    chart->setBackgroundRoundness(8);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setFont(QFont("Arial", 8));
    chart->setMargins(QMargins(8,8,8,8));

    auto *view = new QCChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet(QString("border:1px solid %1;border-radius:8px").arg(BORDER));
    return view;  // QChartView* -> QWidget* implicite
}

QWidget *BilanDialog::makeBarChart()
{
    auto *set = new QCBarSet("CA");
    set->setColor(QColor(ACCENT));
    *set << 0 << 0 << 0;
    auto *series = new QCBarSeries();
    series->append(set);

    auto *chart = new QCChart();
    chart->addSeries(series);
    chart->setTitle("CA par priorité");
    chart->setTitleFont(QFont("Arial", 9, QFont::Bold));
    chart->setTitleBrush(QColor(DARK_TXT));
    chart->setBackgroundBrush(QColor(BG_CARD));
    chart->setBackgroundRoundness(8);

    QStringList cats; cats << "Basse" << "Normale" << "Urgente";
    auto *axisX = new QCBarCategoryAxis();
    axisX->append(cats);
    axisX->setLabelsFont(QFont("Arial", 8));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QCValueAxis();
    axisY->setLabelsFont(QFont("Arial", 8));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->legend()->hide();
    chart->setMargins(QMargins(8,8,8,8));

    auto *view = new QCChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet(QString("border:1px solid %1;border-radius:8px").arg(BORDER));
    return view;
}

QWidget *BilanDialog::makeSplineChart()
{
    auto *series = new QCSplineSeries();
    series->setName("CA quotidien");
    series->setColor(QColor(PRIMARY));
    series->setPen(QPen(QColor(PRIMARY), 2));

    auto *chart = new QCChart();
    chart->addSeries(series);
    chart->setTitle("Évolution quotidienne du CA (30 derniers jours)");
    chart->setTitleFont(QFont("Arial", 9, QFont::Bold));
    chart->setTitleBrush(QColor(DARK_TXT));
    chart->setBackgroundBrush(QColor(BG_CARD));
    chart->setBackgroundRoundness(8);
    chart->legend()->hide();
    chart->setMargins(QMargins(8,8,8,8));
    // Pas de createDefaultAxes ici — fait dans onPeriodChanged après remplissage

    auto *view = new QCChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet(QString("border:1px solid %1;border-radius:8px").arg(BORDER));
    return view;
}



QWidget *BilanDialog::makeHBarChart(const QString &title)
{
    auto *set = new QCBarSet("");
    set->setColor(QColor(ACCENT));
    auto *series = new QCHBarSeries();
    series->append(set);

    auto *chart = new QCChart();
    chart->addSeries(series);
    chart->setTitle(title);
    chart->setTitleFont(QFont("Arial", 9, QFont::Bold));
    chart->setTitleBrush(QColor(DARK_TXT));
    chart->setBackgroundBrush(QColor(BG_CARD));
    chart->setBackgroundRoundness(8);
    chart->legend()->hide();
    chart->setMargins(QMargins(8,8,8,8));

    auto *view = new QCChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet(QString("border:1px solid %1;border-radius:8px").arg(BORDER));
    return view;
}

void BilanDialog::refreshHBar(QWidget *view, const QMap<QString,double> &data)
{
    auto *cv = qobject_cast<QCChartView*>(view);
    if (!cv) return;
    QCChart *chart = cv->chart();

    const auto axisList = chart->axes();
    for (auto *ax : axisList) chart->removeAxis(ax);
    chart->removeAllSeries();

    auto *set = new QCBarSet("");
    set->setColor(QColor(ACCENT));

    QStringList labels;
    // Trier par valeur décroissante
    QList<QPair<QString,double>> sorted;
    for (auto it = data.cbegin(); it != data.cend(); ++it)
        sorted.append({it.key(), it.value()});
    std::sort(sorted.begin(), sorted.end(),
              [](const QPair<QString,double> &a, const QPair<QString,double> &b){ return a.second < b.second; });

    for (const auto &p : sorted) {
        *set << p.second;
        labels << p.first;
    }

    auto *series = new QCHBarSeries();
    series->append(set);
    chart->addSeries(series);

    auto *axisY = new QCBarCategoryAxis();
    axisY->append(labels);
    axisY->setLabelsFont(QFont("Arial", 8));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    auto *axisX = new QCValueAxis();
    axisX->setLabelsFont(QFont("Arial", 8));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
}

void BilanDialog::loadData()
{
    m_comboPeriod->blockSignals(true);
    m_comboPeriod->clear();
    m_comboPeriod->addItem("Tous");

    QSqlQuery q;
    q.exec("SELECT DISTINCT TO_CHAR(date_creation, 'YYYY-MM') AS mois "
           "FROM Commandes ORDER BY mois DESC");
    while (q.next())
        m_comboPeriod->addItem(q.value(0).toString());

    m_comboPeriod->blockSignals(false);
    m_lblDate->setText(QDate::currentDate().toString("dd/MM/yyyy"));
    onPeriodChanged();
}

void BilanDialog::onPeriodChanged()
{
    QString filtre = m_comboPeriod->currentText();
    QString where  = (filtre == "Tous") ? "" :
                     QString(" WHERE TO_CHAR(date_creation,'YYYY-MM') = '%1'").arg(filtre);

    QSqlQuery q;

    // --- KPIs ---
    q.exec("SELECT SUM(montant) FROM Commandes" + where);
    double ca = (q.next() && !q.value(0).isNull()) ? q.value(0).toDouble() : 0.0;
    m_lblCA->setText(fmt(ca));

    double benefice = ca * 0.3;
    m_lblBenefice->setText(fmt(benefice));
    m_lblMarge->setText(QString::number(ca > 0 ? 30.0 : 0.0, 'f', 1) + " %");

    q.exec("SELECT produit, COUNT(*) AS nb FROM Commandes" + where +
           " GROUP BY produit ORDER BY nb DESC");
    m_lblBestProduct->setText(q.next() ? q.value(0).toString() : "—");

    // KPI commandes totales + en attente
    q.exec("SELECT COUNT(*) FROM Commandes" + where);
    int total = q.next() ? q.value(0).toInt() : 0;
    QString whereAtt = where.isEmpty()
        ? " WHERE LOWER(statut)='en attente'"
        : where + " AND LOWER(statut)='en attente'";
    q.exec("SELECT COUNT(*) FROM Commandes" + whereAtt);
    int enAttente = q.next() ? q.value(0).toInt() : 0;
    m_lblCommandes->setText(QString("%1\n(%2 en attente)").arg(total).arg(enAttente));

    // --- Pie chart ---
    if (auto *cv = qobject_cast<QCChartView*>(m_pieView)) {
        auto *series = qobject_cast<QCPieSeries*>(cv->chart()->series().first());
        if (series) {
            QMap<QString,double> prio;
            q.exec("SELECT priorite, SUM(montant) FROM Commandes" + where +
                   " GROUP BY priorite");
            while (q.next()) prio[q.value(0).toString()] = q.value(1).toDouble();
            // Retrouver chaque slice par son label pour éviter les décalages d'index
            for (auto *slice : series->slices())
                slice->setValue(prio.value(slice->label(), 0.01));
        }
    }

    // --- Bar chart ---
    if (auto *cv = qobject_cast<QCChartView*>(m_barView)) {
        auto *series = qobject_cast<QCBarSeries*>(cv->chart()->series().first());
        if (series && !series->barSets().isEmpty()) {
            auto *set = series->barSets().first();
            QMap<QString,double> prio;
            q.exec("SELECT priorite, SUM(montant) FROM Commandes" + where +
                   " GROUP BY priorite");
            while (q.next()) prio[q.value(0).toString()] = q.value(1).toDouble();
            set->replace(0, prio.value("Basse",   0));
            set->replace(1, prio.value("Normale", 0));
            set->replace(2, prio.value("Urgente", 0));
            // Recalcule les axes Y après mise à jour
            if (auto *axisY = qobject_cast<QCValueAxis*>(cv->chart()->axes(Qt::Vertical).first())) {
                double maxVal = std::max({prio.value("Basse",0), prio.value("Normale",0), prio.value("Urgente",0)});
                axisY->setRange(0, maxVal > 0 ? maxVal * 1.15 : 100);
            }
        }
    }

    // --- Spline chart ---
    if (auto *cv = qobject_cast<QCChartView*>(m_splineView)) {
        QCChart *chart = cv->chart();

        // Retirer proprement axes et série sans les détruire
        const auto axisList = chart->axes();
        for (auto *ax : axisList) chart->removeAxis(ax);
        chart->removeAllSeries();

        // Créer une nouvelle série fraîche
        auto *series = new QCSplineSeries();
        series->setColor(QColor(PRIMARY));
        series->setPen(QPen(QColor(PRIMARY), 2));

        q.exec("SELECT EXTRACT(DAY FROM date_creation), SUM(montant) "
               "FROM Commandes" + where +
               " GROUP BY EXTRACT(DAY FROM date_creation) "
               "ORDER BY EXTRACT(DAY FROM date_creation)");
        while (q.next())
            series->append(q.value(0).toDouble(), q.value(1).toDouble());

        chart->addSeries(series);
        chart->createDefaultAxes();
        if (!chart->axes(Qt::Horizontal).isEmpty())
            chart->axes(Qt::Horizontal).first()->setLabelsFont(QFont("Arial", 8));
        if (!chart->axes(Qt::Vertical).isEmpty())
            chart->axes(Qt::Vertical).first()->setLabelsFont(QFont("Arial", 8));
    }

    // --- CA par produit ---
    {
        QMap<QString,double> data;
        q.exec("SELECT produit, SUM(montant) FROM Commandes" + where +
               " GROUP BY produit ORDER BY SUM(montant) DESC");
        while (q.next()) data[q.value(0).toString()] = q.value(1).toDouble();
        refreshHBar(m_produitView, data);
    }

    // --- CA par région (ville du client via mail_client) ---
    {
        QMap<QString,double> data;
        QString joinWhere = where.isEmpty()
            ? " WHERE c.ville IS NOT NULL"
            : where.replace("WHERE", "WHERE c.ville IS NOT NULL AND");
        q.exec("SELECT c.ville, SUM(co.montant) "
               "FROM Commandes co "
               "JOIN Clients c ON c.email = co.mail_client" +
               (where.isEmpty()
                    ? QString(" WHERE c.ville IS NOT NULL")
                    : QString(" WHERE c.ville IS NOT NULL AND TO_CHAR(co.date_creation,'YYYY-MM') = '%1'").arg(filtre)) +
               " GROUP BY c.ville ORDER BY SUM(co.montant) DESC");
        while (q.next()) data[q.value(0).toString()] = q.value(1).toDouble();
        refreshHBar(m_regionView, data);
    }
}

void BilanDialog::exportCSV()
{
    QString path = QFileDialog::getSaveFileName(this, "Exporter CSV", "bilan.csv",
                                                "CSV (*.csv)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible d'ouvrir le fichier.");
        return;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);

    // En-têtes
    out << "Date;Référence;Produit;Montant;Priorité;Statut;ID Employé;État\n";

    // Données directement depuis la BDD
    QSqlQuery q;
    q.exec("SELECT date_creation, reference, produit, montant, priorite, statut, id_employe, etat "
           "FROM Commandes ORDER BY date_creation DESC");
    while (q.next()) {
        QStringList row;
        row << q.value(0).toDate().toString("dd/MM/yyyy");
        for (int c = 1; c < 8; ++c) {
            QString v = (c == 3) ? fmt(q.value(c).toDouble()) : q.value(c).toString().trimmed();
            row << (v.isEmpty() ? "—" : v);
        }
        out << row.join(";") << "\n";
    }
    file.close();
    QMessageBox::information(this, "Export", "Fichier exporté avec succès.");
}
