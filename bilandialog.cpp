#include "bilandialog.h"
#include "connection.h"
#include "notification.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDate>
#include <QLocale>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFont>
#include <QPainter>
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

// ── Constantes de thème ───────────────────────────────────────────────────────
static constexpr const char* BG_PAGE = "#FBF5F0";
static constexpr const char* ACCENT  = "#C4923A";
static constexpr const char* PRIMARY = "#6B2737";
static constexpr const char* BORDER  = "#C4923A";

static QString fmt(double v)
{
    return QLocale(QLocale::French).toString(v, 'f', 2) + " DT";
}

static QString fmtCur(double v, const QString &sym)
{
    return QString("≈ %1 %2").arg(QLocale(QLocale::French).toString(v, 'f', 0), sym);
}

// ── Helpers charts ────────────────────────────────────────────────────────────

// Crée un QChartView stylisé avec les réglages communs
static QChartView *makeChartView(QChart *chart)
{
    chart->setTitleFont(QFont("Arial", 9, QFont::Bold));
    chart->setTitleBrush(QColor(PRIMARY));
    chart->setBackgroundBrush(QColor(BG_PAGE));
    chart->setBackgroundRoundness(8);
    chart->setMargins(QMargins(8, 8, 8, 8));

    auto *view = new QChartView(chart);
    view->setRenderHint(QPainter::Antialiasing);
    view->setStyleSheet(QString("border:1px solid %1;border-radius:8px").arg(BORDER));
    return view;
}

// Vide un chart de ses axes et séries
static void clearChart(QChart *chart)
{
    for (auto *ax : chart->axes()) chart->removeAxis(ax);
    chart->removeAllSeries();
}

// Applique la police Arial 8 sur tous les axes d'un chart
static void styleAxes(QChart *chart)
{
    for (auto *ax : chart->axes())
        ax->setLabelsFont(QFont("Arial", 8));
}

// ── BilanDialog ───────────────────────────────────────────────────────────────

BilanDialog::BilanDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Bilan Financier — CUIREA");
    setMinimumSize(1100, 720);
    setStyleSheet(QString(
        "QDialog{background:%1}"
        "QGroupBox{background:%1;border:none;border-left:3px solid %2;"
         "border-radius:8px;padding:8px}"
        "QGroupBox::title{color:%3;font-size:10px;font-weight:bold;"
         "subcontrol-origin:margin;left:10px;padding:0 4px}"
        "QComboBox{background:%2;color:white;border:none;border-radius:6px;"
         "padding:6px 12px;font-size:12px;font-weight:bold}"
        "QComboBox::drop-down{border:none}"
        "QComboBox QAbstractItemView{background:white;color:%3;"
         "selection-background-color:%2}"
        "QPushButton{border-radius:7px;padding:8px 22px;font-size:12px;font-weight:bold}"
        "QScrollBar:vertical{width:8px;background:#E8D8C4;border-radius:4px}"
        "QScrollBar::handle:vertical{background:%2;border-radius:4px}"
    ).arg(BG_PAGE, ACCENT, PRIMARY));

    setupUI();
    loadData();

    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &BilanDialog::onTauxReceived);
    fetchTauxChange();
    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &BilanDialog::fetchTauxChange);
    timer->start(60000);
}

void BilanDialog::setupUI()
{
    auto *rootLay = new QVBoxLayout(this);
    rootLay->setContentsMargins(0, 0, 0, 0);
    rootLay->setSpacing(0);

    // Top bar
    auto *topBar = new QFrame();
    topBar->setStyleSheet(QString("QFrame{background:%1}").arg(PRIMARY));
    auto *topLay = new QHBoxLayout(topBar);
    topLay->setContentsMargins(20, 12, 20, 12);
    topLay->setSpacing(16);

    auto *titleLbl = new QLabel("BILAN FINANCIER");
    titleLbl->setStyleSheet("font-size:18px;font-weight:bold;color:white;letter-spacing:2px");
    m_lblDate = new QLabel(QDate::currentDate().toString("dd/MM/yyyy"));
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

    // Scroll area
    auto *scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setStyleSheet(QString("QScrollArea{background:%1;border:none}").arg(BG_PAGE));

    auto *content = new QWidget();
    content->setStyleSheet(QString("background:%1").arg(BG_PAGE));
    auto *mainLay = new QVBoxLayout(content);
    mainLay->setContentsMargins(20, 20, 20, 20);
    mainLay->setSpacing(16);

    // KPI column
    auto *kpiCol = new QVBoxLayout();
    kpiCol->setSpacing(12);

    auto addConvLabels = [](QGroupBox *box, QLabel *&eur, QLabel *&usd) {
        auto *lay = qobject_cast<QVBoxLayout*>(box->layout());
        eur = new QLabel("≈ … €");
        usd = new QLabel("≈ … $");
        for (auto *lbl : {eur, usd}) {
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setStyleSheet("font-size:12px;border:none");
            lay->addWidget(lbl);
        }
        eur->setStyleSheet("font-size:12px;color:#27ae60;border:none");
        usd->setStyleSheet("font-size:12px;color:#2980b9;border:none");
    };

    auto *caBox = makeKpiBox("Chiffre d'affaires total", m_lblCA);
    addConvLabels(caBox, m_lblCA_EUR, m_lblCA_USD);
    kpiCol->addWidget(caBox);
    kpiCol->addWidget(makeKpiBox("Meilleur produit du mois", m_lblBestProduct, true));

    auto *benBox = makeKpiBox("Bénéfice net", m_lblBenefice);
    addConvLabels(benBox, m_lblBenefice_EUR, m_lblBenefice_USD);
    kpiCol->addWidget(benBox);
    kpiCol->addWidget(makeKpiBox("Marge brute",       m_lblMarge));
    kpiCol->addWidget(makeKpiBox("Commandes totales", m_lblCommandes));
    kpiCol->addStretch();

    auto *kpiWidget = new QWidget();
    kpiWidget->setLayout(kpiCol);
    kpiWidget->setFixedWidth(240);

    // Charts
    m_pieView    = makePieChart();
    m_barView    = makeBarChart();
    m_splineView = makeSplineChart();
    m_produitView = makeHBarChart("CA par produit");
    m_regionView  = makeHBarChart("CA par région");
    m_pieView->setMinimumSize(260, 220);
    m_barView->setMinimumSize(260, 220);
    m_splineView->setMinimumHeight(200);
    m_produitView->setMinimumHeight(220);
    m_regionView->setMinimumHeight(220);

    auto *topCharts = new QHBoxLayout(); topCharts->setSpacing(12);
    topCharts->addWidget(m_pieView, 1);
    topCharts->addWidget(m_barView, 1);

    auto *botCharts = new QHBoxLayout(); botCharts->setSpacing(12);
    botCharts->addWidget(m_produitView, 1);
    botCharts->addWidget(m_regionView, 1);

    auto *chartsCol = new QVBoxLayout(); chartsCol->setSpacing(12);
    chartsCol->addLayout(topCharts);
    chartsCol->addWidget(m_splineView);
    chartsCol->addLayout(botCharts);

    auto *midRow = new QHBoxLayout(); midRow->setSpacing(16);
    midRow->addWidget(kpiWidget);
    midRow->addLayout(chartsCol, 1);
    mainLay->addLayout(midRow);

    scroll->setWidget(content);
    rootLay->addWidget(scroll, 1);

    // Button bar
    auto *btnBar = new QFrame();
    btnBar->setStyleSheet(
        QString("QFrame{background:%1;border-top:2px solid %2}").arg(BG_PAGE, ACCENT));
    auto *btnLay = new QHBoxLayout(btnBar);
    btnLay->setContentsMargins(20, 10, 20, 10);
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
    if (highlight)
        box->setStyleSheet(
            QString("QGroupBox{background:%1;border:none;border-radius:8px;padding:8px}"
                    "QGroupBox::title{color:%2;font-size:10px;font-weight:bold;"
                    "subcontrol-origin:margin;left:10px;padding:0 4px}").arg(PRIMARY, ACCENT));

    auto *lay = new QVBoxLayout(box);
    lay->setContentsMargins(10, 18, 10, 10);
    valLabel = new QLabel("—");
    valLabel->setAlignment(Qt::AlignCenter);
    valLabel->setWordWrap(true);
    valLabel->setStyleSheet(
        QString("font-size:22px;font-weight:bold;color:%1;border:none")
        .arg(highlight ? ACCENT : PRIMARY));
    lay->addWidget(valLabel);
    return box;
}

QWidget *BilanDialog::makePieChart()
{
    auto *series = new QPieSeries();
    series->append("Basse",   1)->setColor(QColor(ACCENT));
    series->append("Normale", 1)->setColor(QColor("#A0485A"));
    series->append("Urgente", 1)->setColor(QColor(PRIMARY));

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Répartition par priorité");
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setFont(QFont("Arial", 8));
    return makeChartView(chart);
}

QWidget *BilanDialog::makeBarChart()
{
    auto *set = new QBarSet("CA");
    set->setColor(QColor(ACCENT));
    *set << 0 << 0 << 0;
    auto *series = new QBarSeries();
    series->append(set);

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("CA par priorité");
    chart->legend()->hide();

    auto *axisX = new QBarCategoryAxis();
    axisX->append({"Basse", "Normale", "Urgente"});
    axisX->setLabelsFont(QFont("Arial", 8));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setLabelsFont(QFont("Arial", 8));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    return makeChartView(chart);
}

QWidget *BilanDialog::makeSplineChart()
{
    auto *series = new QSplineSeries();
    series->setName("CA quotidien");
    series->setColor(QColor(PRIMARY));
    series->setPen(QPen(QColor(PRIMARY), 2));

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Évolution quotidienne du CA (30 derniers jours)");
    chart->legend()->hide();
    return makeChartView(chart);
}

QWidget *BilanDialog::makeHBarChart(const QString &title)
{
    auto *set = new QBarSet("");
    set->setColor(QColor(ACCENT));
    auto *series = new QHorizontalBarSeries();
    series->append(set);

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(title);
    chart->legend()->hide();
    return makeChartView(chart);
}

void BilanDialog::refreshHBar(QWidget *view, const QMap<QString,double> &data)
{
    auto *cv = qobject_cast<QChartView*>(view);
    if (!cv) return;
    QChart *chart = cv->chart();
    clearChart(chart);

    if (data.isEmpty()) return;

    // Trier par valeur croissante (barres horizontales : bas = plus grand)
    QList<QPair<QString,double>> sorted(data.keyValueBegin(), data.keyValueEnd());
    std::sort(sorted.begin(), sorted.end(),
              [](const auto &a, const auto &b) { return a.second < b.second; });

    auto *set = new QBarSet("");
    set->setColor(QColor(ACCENT));
    QStringList labels;
    for (const auto &[label, val] : sorted) {
        *set << val;
        // Tronquer les labels trop longs
        labels << (label.length() > 14 ? label.left(12) + "…" : label);
    }

    auto *series = new QHorizontalBarSeries();
    series->append(set);
    chart->addSeries(series);

    auto *axisY = new QBarCategoryAxis();
    axisY->append(labels);
    axisY->setLabelsFont(QFont("Arial", 9));
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    // Axe X : entiers sans décimales, suffixe DT
    const double maxVal = sorted.last().second;
    auto *axisX = new QValueAxis();
    axisX->setRange(0, maxVal * 1.1);
    axisX->setLabelFormat("%.0f DT");
    axisX->setTickCount(5);
    axisX->setLabelsFont(QFont("Arial", 8));
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
}

void BilanDialog::loadData()
{
    m_comboPeriod->blockSignals(true);
    m_comboPeriod->clear();
    m_comboPeriod->addItem("Tous");

    QSqlQuery q(Connection::instance()->getDatabase());
    q.exec("SELECT DISTINCT TO_CHAR(date_creation,'YYYY-MM') AS mois "
           "FROM Commandes ORDER BY mois DESC");
    while (q.next())
        m_comboPeriod->addItem(q.value(0).toString());

    m_comboPeriod->blockSignals(false);
    onPeriodChanged();
}

void BilanDialog::onPeriodChanged()
{
    const QString filtre = m_comboPeriod->currentText();
    const bool    allPeriods = (filtre == "Tous");
    const QString whereMonth = allPeriods ? QString()
        : QString(" WHERE TO_CHAR(date_creation,'YYYY-MM') = '%1'").arg(filtre);
    const QString andMonth = allPeriods ? QString()
        : QString(" AND TO_CHAR(date_creation,'YYYY-MM') = '%1'").arg(filtre);

    QSqlQuery q(Connection::instance()->getDatabase());

    // ── KPIs (une seule requête pour total + en attente) ─────────────────────
    q.exec("SELECT SUM(montant), COUNT(*), "
           "SUM(CASE WHEN LOWER(statut)='en attente' THEN 1 ELSE 0 END) "
           "FROM Commandes" + whereMonth);
    double ca = 0.0; int total = 0, enAttente = 0;
    if (q.next()) {
        ca        = q.value(0).isNull() ? 0.0 : q.value(0).toDouble();
        total     = q.value(1).toInt();
        enAttente = q.value(2).toInt();
    }
    m_totalCA = ca;
    m_lblCA->setText(fmt(ca));
    m_lblCommandes->setText(QString("%1\n(%2 en attente)").arg(total).arg(enAttente));

    // Bénéfice net depuis Articles
    q.exec(QString(
        "SELECT SUM(a.prix_unitaire - a.cout_fabrication), SUM(a.prix_unitaire) "
        "FROM Articles a JOIN Commandes co ON co.id_commande = a.id_commande"
    ) + (allPeriods ? "" : QString(" WHERE TO_CHAR(co.date_creation,'YYYY-MM')='%1'").arg(filtre)));

    double benefice = 0.0, totalPrix = 0.0;
    if (q.next() && !q.value(0).isNull()) {
        benefice  = q.value(0).toDouble();
        totalPrix = q.value(1).toDouble();
    }
    if (totalPrix <= 0 && ca > 0) { benefice = ca * 0.30; totalPrix = ca; }
    m_totalBenefice = benefice;
    m_lblBenefice->setText(fmt(benefice));
    m_lblMarge->setText(QString::number(totalPrix > 0 ? benefice / totalPrix * 100.0 : 0.0, 'f', 1) + " %");

    q.exec("SELECT produit FROM Commandes" + whereMonth +
           (allPeriods ? " WHERE " : " AND ") +
           "produit IS NOT NULL GROUP BY produit ORDER BY COUNT(*) DESC");
    // Fix: use proper WHERE/AND
    q.exec("SELECT produit, COUNT(*) FROM Commandes" + whereMonth +
           " GROUP BY produit ORDER BY COUNT(*) DESC");
    m_lblBestProduct->setText(q.next() ? q.value(0).toString() : "—");

    // Notifications
    if (ca == 0.0 && total > 0)
        NotificationWidget::show("Bilan — Aucun CA",
            "Aucun chiffre d'affaires pour la période sélectionnée.",
            NotificationWidget::Warning);
    else if (total > 0 && enAttente > 0 && (enAttente * 100 / total) >= 30)
        NotificationWidget::show("Commandes en attente",
            QString("%1 commandes en attente sur %2 — action requise.").arg(enAttente).arg(total),
            NotificationWidget::Warning);

    // ── Pie chart ─────────────────────────────────────────────────────────────
    if (auto *cv = qobject_cast<QChartView*>(m_pieView)) {
        cv->chart()->removeAllSeries();
        QMap<QString,double> prio;
        q.exec("SELECT priorite, COUNT(*) FROM Commandes" + whereMonth + " GROUP BY priorite");
        while (q.next()) prio[q.value(0).toString()] = q.value(1).toDouble();

        static const QMap<QString,QColor> colors = {
            {"Basse", QColor(ACCENT)}, {"Normale", QColor("#A0485A")}, {"Urgente", QColor(PRIMARY)}
        };
        auto *series = new QPieSeries();
        for (auto it = prio.cbegin(); it != prio.cend(); ++it) {
            auto *slice = series->append(it.key(), it.value());
            slice->setColor(colors.value(it.key(), QColor(ACCENT)));
            slice->setLabelVisible(true);
            slice->setLabel(QString("%1\n%2").arg(it.key()).arg((int)it.value()));
        }
        cv->chart()->addSeries(series);
    }

    // ── Bar chart ─────────────────────────────────────────────────────────────
    if (auto *cv = qobject_cast<QChartView*>(m_barView)) {
        QChart *chart = cv->chart();
        clearChart(chart);

        QMap<QString,double> prio;
        q.exec("SELECT priorite, SUM(montant) FROM Commandes" + whereMonth + " GROUP BY priorite");
        while (q.next()) prio[q.value(0).toString()] = q.value(1).toDouble();

        auto *set = new QBarSet("CA"); set->setColor(QColor(ACCENT));
        for (const QString p : {"Basse", "Normale", "Urgente"}) *set << prio.value(p, 0.0);

        auto *series = new QBarSeries(); series->append(set);
        chart->addSeries(series);

        auto *axisX = new QBarCategoryAxis();
        axisX->append({"Basse", "Normale", "Urgente"});
        chart->addAxis(axisX, Qt::AlignBottom); series->attachAxis(axisX);

        double maxVal = prio.isEmpty() ? 100.0 : *std::max_element(prio.cbegin(), prio.cend());
        auto *axisY = new QValueAxis();
        axisY->setRange(0, maxVal > 0 ? maxVal * 1.15 : 100);
        chart->addAxis(axisY, Qt::AlignLeft); series->attachAxis(axisY);
        styleAxes(chart);
    }

    // ── Spline chart ──────────────────────────────────────────────────────────
    if (auto *cv = qobject_cast<QChartView*>(m_splineView)) {
        QChart *chart = cv->chart();
        clearChart(chart);

        auto *series = new QSplineSeries();
        series->setColor(QColor(PRIMARY));
        series->setPen(QPen(QColor(PRIMARY), 2));

        if (allPeriods) {
            chart->setTitle("Évolution mensuelle du CA");
            q.exec("SELECT TO_CHAR(date_creation,'YYYY-MM'), SUM(montant) FROM Commandes "
                   "GROUP BY TO_CHAR(date_creation,'YYYY-MM') "
                   "ORDER BY TO_CHAR(date_creation,'YYYY-MM')");
            for (int idx = 0; q.next(); ++idx) series->append(idx, q.value(1).toDouble());
        } else {
            chart->setTitle("Évolution quotidienne du CA — " + filtre);
            q.exec(QString(
                "SELECT EXTRACT(DAY FROM date_creation), SUM(montant) FROM Commandes "
                "WHERE TO_CHAR(date_creation,'YYYY-MM')='%1' "
                "GROUP BY EXTRACT(DAY FROM date_creation) "
                "ORDER BY EXTRACT(DAY FROM date_creation)").arg(filtre));
            while (q.next()) series->append(q.value(0).toDouble(), q.value(1).toDouble());
        }
        chart->addSeries(series);
        chart->createDefaultAxes();
        styleAxes(chart);
    }

    // ── CA par produit (via Articles.nom pour avoir les vrais noms) ──────────
    {
        QMap<QString,double> data;
        // Priorité : nom de l'article lié ; fallback sur Commandes.produit
        QString sqlProduit =
            "SELECT COALESCE(a.nom, co.produit) AS nom_produit, SUM(co.montant) "
            "FROM Commandes co "
            "LEFT JOIN Articles a ON a.id_commande = co.id_commande ";
        if (!allPeriods)
            sqlProduit += QString("WHERE TO_CHAR(co.date_creation,'YYYY-MM')='%1' ").arg(filtre);
        sqlProduit += "GROUP BY COALESCE(a.nom, co.produit) ORDER BY SUM(co.montant) DESC";

        q.exec(sqlProduit);
        while (q.next()) {
            const QString k = q.value(0).toString().trimmed();
            const double  v = q.value(1).toDouble();
            if (!k.isEmpty() && v > 0) data[k] = v;
        }

        // Fallback si aucun article lié : grouper par Commandes.produit
        if (data.isEmpty()) {
            q.exec("SELECT produit, SUM(montant) FROM Commandes" + whereMonth +
                   " GROUP BY produit ORDER BY SUM(montant) DESC");
            while (q.next()) {
                const QString k = q.value(0).toString().trimmed();
                const double  v = q.value(1).toDouble();
                if (!k.isEmpty() && v > 0) data[k] = v;
            }
        }
        refreshHBar(m_produitView, data);
    }

    // ── CA par région ─────────────────────────────────────────────────────────
    {
        QMap<QString,double> data;
        QString sql =
            "SELECT COALESCE(c.ville, co.mail_client) AS region, SUM(co.montant) "
            "FROM Commandes co "
            "LEFT JOIN Clients c ON UPPER(TRIM(c.email)) = UPPER(TRIM(co.mail_client)) ";
        if (!allPeriods)
            sql += QString("WHERE TO_CHAR(co.date_creation,'YYYY-MM')='%1' ").arg(filtre);
        sql += "GROUP BY COALESCE(c.ville, co.mail_client) ORDER BY SUM(co.montant) DESC";

        q.exec(sql);
        while (q.next()) {
            const QString k = q.value(0).toString().trimmed();
            const double  v = q.value(1).toDouble();
            if (!k.isEmpty() && v > 0) data[k] = v;
        }

        if (data.isEmpty()) {
            sql = "SELECT co.mail_client, SUM(co.montant) FROM Commandes co ";
            if (!allPeriods)
                sql += QString("WHERE TO_CHAR(co.date_creation,'YYYY-MM')='%1' ").arg(filtre);
            sql += "GROUP BY co.mail_client ORDER BY SUM(co.montant) DESC";
            q.exec(sql);
            while (q.next()) {
                const QString k = q.value(0).toString().trimmed();
                const double  v = q.value(1).toDouble();
                if (!k.isEmpty() && v > 0) data[k] = v;
            }
        }
        refreshHBar(m_regionView, data);
    }
}

void BilanDialog::exportCSV()
{
    const QString path = QFileDialog::getSaveFileName(this, "Exporter CSV", "bilan.csv", "CSV (*.csv)");
    if (path.isEmpty()) return;

    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Erreur", "Impossible d'ouvrir le fichier.");
        return;
    }
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << "Date;Référence;Produit;Montant;Priorité;Statut;ID Employé;État\n";

    QSqlQuery q(Connection::instance()->getDatabase());
    q.exec("SELECT date_creation, reference, produit, montant, priorite, statut, id_employe, etat "
           "FROM Commandes ORDER BY date_creation DESC");
    while (q.next()) {
        QStringList row;
        row << q.value(0).toDate().toString("dd/MM/yyyy");
        for (int c = 1; c < 8; ++c) {
            const QString v = (c == 3) ? fmt(q.value(c).toDouble()) : q.value(c).toString().trimmed();
            row << (v.isEmpty() ? "—" : v);
        }
        out << row.join(";") << "\n";
    }
    file.close();
    NotificationWidget::show("Export réussi", "Bilan exporté : " + QFileInfo(path).fileName(),
                             NotificationWidget::Success);
}

void BilanDialog::fetchTauxChange()
{
    static const QString apiKey = "65a39a8f559bcde817876cc401ec5ded";
    m_networkManager->get(QNetworkRequest(
        QUrl("https://data.fixer.io/api/latest?access_key=" + apiKey + "&base=EUR&symbols=TND,USD")));
}

void BilanDialog::onTauxReceived(QNetworkReply *reply)
{
    const QByteArray raw = reply->readAll();
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        m_lblCA_EUR->setText("≈ hors ligne");
        m_lblCA_USD->setText("");
        return;
    }

    const QJsonObject rates = QJsonDocument::fromJson(raw)["rates"].toObject();
    const double eurToTND = rates["TND"].toDouble();
    const double eurToUSD = rates["USD"].toDouble();
    if (eurToTND <= 0) return;

    auto updateConv = [&](double tnd, QLabel *eur, QLabel *usd) {
        const double inEUR = tnd / eurToTND;
        eur->setText(fmtCur(inEUR, "€"));
        usd->setText(fmtCur(inEUR * eurToUSD, "$"));
    };

    if (m_totalCA      > 0) updateConv(m_totalCA,      m_lblCA_EUR,      m_lblCA_USD);
    if (m_totalBenefice > 0) updateConv(m_totalBenefice, m_lblBenefice_EUR, m_lblBenefice_USD);
}
