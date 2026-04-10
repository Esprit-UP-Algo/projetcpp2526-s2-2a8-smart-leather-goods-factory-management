#include "statsrh.h"
#include "connection.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QPainter>
#include <QTableWidget>
#include <QHeaderView>
#include <QProgressBar>
#include <QPushButton>
#include <QtMath>
#include <QGraphicsDropShadowEffect>
#include <QScrollArea>
#include <QFrame>

StatsRHDialog::StatsRHDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    loadStatistics();
    applyStyles();
    animateKPIs();
    
    setWindowTitle("Statistiques RH - CUIREA");
    setMinimumSize(1200, 800);
}

void StatsRHDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);
    
    // Titre
    QLabel *title = new QLabel("Statistiques des Ressources Humaines");
    title->setObjectName("mainTitle");
    title->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(title);
    
    // Bande KPI
    QWidget *kpiWidget = new QWidget();
    kpiWidget->setObjectName("kpiWidget");
    QHBoxLayout *kpiLayout = new QHBoxLayout(kpiWidget);
    kpiLayout->setSpacing(15);
    
    auto createKPI = [this](const QString &label, const QString &value, const QString &color, const QString &icon) {
        QWidget *kpi = new QWidget();
        kpi->setObjectName("kpiCard");
        
        // Effet d'ombre
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
        shadow->setBlurRadius(15);
        shadow->setColor(QColor(0, 0, 0, 60));
        shadow->setOffset(0, 3);
        kpi->setGraphicsEffect(shadow);
        
        // Style simple avec couleur unie
        QString style = QString(
            "QWidget#kpiCard { "
            "background-color: %1; "
            "border-radius: 12px; padding: 20px; "
            "border: 2px solid rgba(255,255,255,0.3); }"
        ).arg(color);
        
        kpi->setStyleSheet(style);
        
        QVBoxLayout *layout = new QVBoxLayout(kpi);
        layout->setSpacing(8);
        
        // Icône
        QLabel *iconLabel = new QLabel(icon);
        iconLabel->setObjectName("kpiIcon");
        iconLabel->setAlignment(Qt::AlignCenter);
        
        QLabel *valueLabel = new QLabel(value);
        valueLabel->setObjectName("kpiValue");
        valueLabel->setAlignment(Qt::AlignCenter);
        
        QLabel *labelLabel = new QLabel(label);
        labelLabel->setObjectName("kpiLabel");
        labelLabel->setAlignment(Qt::AlignCenter);
        
        layout->addWidget(iconLabel);
        layout->addWidget(valueLabel);
        layout->addWidget(labelLabel);
        
        return kpi;
    };
    
    totalLabel = new QLabel("0");
    departementsLabel = new QLabel("0");
    postesLabel = new QLabel("0");
    hommesLabel = new QLabel("0");
    femmesLabel = new QLabel("0");
    pariteLabel = new QLabel("0%");
    
    QWidget *kpi1 = createKPI("Total Employés", "0", "#8D6E63", "👥");
    QWidget *kpi2 = createKPI("Départements", "0", "#5E35B1", "🏢");
    QWidget *kpi3 = createKPI("Postes", "0", "#00897B", "💼");
    QWidget *kpi4 = createKPI("Hommes", "0", "#1976D2", "👨");
    QWidget *kpi5 = createKPI("Femmes", "0", "#C2185B", "👩");
    QWidget *kpi6 = createKPI("% Parité", "0%", "#43A047", "⚖️");
    
    kpiLayout->addWidget(kpi1);
    kpiLayout->addWidget(kpi2);
    kpiLayout->addWidget(kpi3);
    kpiLayout->addWidget(kpi4);
    kpiLayout->addWidget(kpi5);
    kpiLayout->addWidget(kpi6);
    
    // Stocker les références aux labels de valeur
    totalLabel = kpi1->findChild<QLabel*>("kpiValue");
    departementsLabel = kpi2->findChild<QLabel*>("kpiValue");
    postesLabel = kpi3->findChild<QLabel*>("kpiValue");
    hommesLabel = kpi4->findChild<QLabel*>("kpiValue");
    femmesLabel = kpi5->findChild<QLabel*>("kpiValue");
    pariteLabel = kpi6->findChild<QLabel*>("kpiValue");
    
    mainLayout->addWidget(kpiWidget);
    
    // Onglets
    tabWidget = new QTabWidget();
    tabWidget->setObjectName("statsTabWidget");
    
    tabWidget->addTab(createDepartementTab(), "📊 Par Département");
    tabWidget->addTab(createPosteTab(), "📈 Par Poste");
    tabWidget->addTab(createTableauTab(), "📋 Tableau Récapitulatif");
    tabWidget->addTab(createPariteTab(), "⚖️ Parité H/F");
    
    mainLayout->addWidget(tabWidget);
    
    // Bouton fermer
    QPushButton *closeBtn = new QPushButton("Fermer");
    closeBtn->setObjectName("closeButton");
    closeBtn->setMaximumWidth(150);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(closeBtn);
    btnLayout->addStretch();
    
    mainLayout->addLayout(btnLayout);
}

QWidget* StatsRHDialog::createDepartementTab()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // Scroll area
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(15);
    
    QLabel *titre = new QLabel("Répartition de l'effectif par département");
    titre->setObjectName("tabTitle");
    titre->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(titre);
    
    // Graphique donut sera créé après chargement des données
    QWidget *graphContainer = new QWidget();
    graphContainer->setObjectName("graphContainer");
    graphContainer->setMinimumHeight(600);
    new QVBoxLayout(graphContainer); // Layout sera utilisé pour ajouter le graphique
    
    contentLayout->addWidget(graphContainer);
    contentLayout->addStretch();
    
    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);
    
    return widget;
}

QWidget* StatsRHDialog::createPosteTab()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // Scroll area
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(15);
    
    QLabel *titre = new QLabel("Répartition de l'effectif par poste");
    titre->setObjectName("tabTitle");
    titre->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(titre);
    
    QWidget *graphContainer = new QWidget();
    graphContainer->setObjectName("graphContainer");
    graphContainer->setMinimumHeight(600);
    new QVBoxLayout(graphContainer); // Layout sera utilisé pour ajouter le graphique
    
    contentLayout->addWidget(graphContainer);
    contentLayout->addStretch();
    
    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);
    
    return widget;
}

QWidget* StatsRHDialog::createTableauTab()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // Scroll area
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(15);
    
    QLabel *titre = new QLabel("Tableau récapitulatif des effectifs");
    titre->setObjectName("tabTitle");
    titre->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(titre);
    
    // Tableau sera rempli après chargement des données
    QTableWidget *table = new QTableWidget();
    table->setObjectName("statsTable");
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"Département", "Effectif", "Pourcentage"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->verticalHeader()->setVisible(false);
    table->setAlternatingRowColors(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    
    contentLayout->addWidget(table);
    contentLayout->addStretch();
    
    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);
    
    return widget;
}

QWidget* StatsRHDialog::createPariteTab()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    
    // Scroll area
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(20, 20, 20, 20);
    contentLayout->setSpacing(15);
    
    QLabel *titre = new QLabel("Parité Hommes / Femmes");
    titre->setObjectName("tabTitle");
    titre->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(titre);
    
    QWidget *graphContainer = new QWidget();
    graphContainer->setObjectName("graphContainer");
    graphContainer->setMinimumHeight(600);
    new QVBoxLayout(graphContainer); // Layout sera utilisé pour ajouter le graphique
    
    contentLayout->addWidget(graphContainer);
    contentLayout->addStretch();
    
    scrollArea->setWidget(scrollContent);
    layout->addWidget(scrollArea);
    
    return widget;
}

void StatsRHDialog::loadStatistics()
{
    // Charger les données directement via SQL
    QSqlQuery q;

    effectifDepartement.clear();
    q.exec("SELECT DEPARTEMENT, COUNT(*) FROM EMPLOYES GROUP BY DEPARTEMENT");
    while (q.next())
        effectifDepartement[q.value(0).toString()] = q.value(1).toInt();

    effectifPoste.clear();
    q.exec("SELECT POSTE, COUNT(*) FROM EMPLOYES GROUP BY POSTE");
    while (q.next())
        effectifPoste[q.value(0).toString()] = q.value(1).toInt();

    effectifSexe.clear();
    q.exec("SELECT SEXE, COUNT(*) FROM EMPLOYES GROUP BY SEXE");
    while (q.next()) {
        QString sexe = q.value(0).toString().toLower();
        QString key = (sexe == "m" || sexe == "homme" || sexe == "masculin") ? "Hommes" : "Femmes";
        effectifSexe[key] += q.value(1).toInt();
    }

    q.exec("SELECT COUNT(*) FROM EMPLOYES");
    totalEmployes = q.next() ? q.value(0).toInt() : 0;

    nombreDepartements = effectifDepartement.size();
    nombrePostes       = effectifPoste.size();
    
    // Mettre à jour les KPI
    if (totalLabel) totalLabel->setText(QString::number(totalEmployes));
    if (departementsLabel) departementsLabel->setText(QString::number(nombreDepartements));
    if (postesLabel) postesLabel->setText(QString::number(nombrePostes));
    
    int hommes = effectifSexe.value("Hommes", 0);
    int femmes = effectifSexe.value("Femmes", 0);
    
    if (hommesLabel) hommesLabel->setText(QString::number(hommes));
    if (femmesLabel) femmesLabel->setText(QString::number(femmes));
    
    double parite = totalEmployes > 0 ? (femmes * 100.0 / totalEmployes) : 0;
    if (pariteLabel) pariteLabel->setText(QString::number(parite, 'f', 1) + "%");
    
    // Ajouter les graphiques
    // Onglet 1: Donut département
    QWidget *tab1 = tabWidget->widget(0);
    QScrollArea *scroll1 = tab1->findChild<QScrollArea*>();
    if (scroll1) {
        QWidget *container1 = scroll1->widget()->findChild<QWidget*>("graphContainer");
        if (container1) {
            GraphiqueWidget *graph = new GraphiqueWidget(GraphiqueWidget::Donut,
                                                         effectifDepartement,
                                                         "Effectif par Département");
            container1->layout()->addWidget(graph);
        }
    }
    
    // Onglet 2: Barres poste
    QWidget *tab2 = tabWidget->widget(1);
    QScrollArea *scroll2 = tab2->findChild<QScrollArea*>();
    if (scroll2) {
        QWidget *container2 = scroll2->widget()->findChild<QWidget*>("graphContainer");
        if (container2) {
            GraphiqueWidget *graph = new GraphiqueWidget(GraphiqueWidget::Barres,
                                                         effectifPoste,
                                                         "Effectif par Poste");
            container2->layout()->addWidget(graph);
        }
    }
    
    // Onglet 3: Tableau
    QWidget *tab3 = tabWidget->widget(2);
    QScrollArea *scroll3 = tab3->findChild<QScrollArea*>();
    QTableWidget *table = nullptr;
    if (scroll3) {
        table = scroll3->widget()->findChild<QTableWidget*>("statsTable");
    }
    if (table) {
        table->setRowCount(effectifDepartement.size() + 1);
        int row = 0;
        
        for (auto it = effectifDepartement.begin(); it != effectifDepartement.end(); ++it) {
            table->setItem(row, 0, new QTableWidgetItem(it.key()));
            table->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
            
            double pct = totalEmployes > 0 ? (it.value() * 100.0 / totalEmployes) : 0;
            QTableWidgetItem *pctItem = new QTableWidgetItem(QString::number(pct, 'f', 1) + "%");
            table->setItem(row, 2, pctItem);
            
            // Barre de progression visuelle
            QProgressBar *bar = new QProgressBar();
            bar->setRange(0, 100);
            bar->setValue(static_cast<int>(pct));
            bar->setTextVisible(false);
            bar->setMaximumHeight(20);
            bar->setStyleSheet("QProgressBar { border: 1px solid #BCAAA4; border-radius: 3px; "
                             "background-color: #F5F5F5; } "
                             "QProgressBar::chunk { background-color: #8D6E63; }");
            
            row++;
        }
        
        // Ligne totale
        QFont boldFont;
        boldFont.setBold(true);
        
        QTableWidgetItem *totalItem = new QTableWidgetItem("TOTAL");
        totalItem->setFont(boldFont);
        totalItem->setBackground(QBrush(QColor(232, 245, 233)));
        table->setItem(row, 0, totalItem);
        
        QTableWidgetItem *totalValItem = new QTableWidgetItem(QString::number(totalEmployes));
        totalValItem->setFont(boldFont);
        totalValItem->setBackground(QBrush(QColor(232, 245, 233)));
        table->setItem(row, 1, totalValItem);
        
        QTableWidgetItem *total100Item = new QTableWidgetItem("100.0%");
        total100Item->setFont(boldFont);
        total100Item->setBackground(QBrush(QColor(232, 245, 233)));
        table->setItem(row, 2, total100Item);
        
        table->resizeColumnsToContents();
    }
    
    // Onglet 4: Camembert parité
    QWidget *tab4 = tabWidget->widget(3);
    QScrollArea *scroll4 = tab4->findChild<QScrollArea*>();
    if (scroll4) {
        QWidget *container4 = scroll4->widget()->findChild<QWidget*>("graphContainer");
        if (container4) {
            GraphiqueWidget *graph = new GraphiqueWidget(GraphiqueWidget::Camembert,
                                                         effectifSexe,
                                                         "Répartition Hommes / Femmes");
            container4->layout()->addWidget(graph);
        }
    }
}

void StatsRHDialog::applyStyles()
{
    setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel#mainTitle { color: #291C0E; font-family: 'Segoe UI', Arial, sans-serif; "
        "font-size: 26px; font-weight: bold; padding: 20px; }"
        "QLabel#tabTitle { color: #8D6E63; font-family: 'Segoe UI', Arial, sans-serif; "
        "font-size: 20px; font-weight: bold; padding: 15px; }"
        "QLabel#kpiValue { color: white; font-family: 'Segoe UI', Arial, sans-serif; "
        "font-size: 32px; font-weight: bold; }"
        "QLabel#kpiLabel { color: rgba(255,255,255,0.95); font-family: 'Segoe UI', Arial, sans-serif; "
        "font-size: 12px; font-weight: 600; }"
        "QLabel#kpiIcon { color: white; font-size: 36px; }"
        "QWidget#kpiWidget { background-color: transparent; }"
        "QWidget#graphContainer { background-color: white; "
        "border: 2px solid #E0E0E0; border-radius: 12px; padding: 20px; }"
        "QScrollArea { background-color: white; border: none; }"
        "QScrollArea > QWidget > QWidget { background-color: white; }"
        "QScrollBar:vertical { background-color: #F5F5F5; width: 12px; border-radius: 6px; }"
        "QScrollBar::handle:vertical { background-color: #BCAAA4; border-radius: 6px; min-height: 30px; }"
        "QScrollBar::handle:vertical:hover { background-color: #8D6E63; }"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0px; }"
        "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: none; }"
        "QTabWidget::pane { border: 2px solid #E0E0E0; border-radius: 10px; "
        "background-color: white; margin-top: -1px; }"
        "QTabBar::tab { background-color: #F5F5F5; "
        "color: #424242; padding: 12px 24px; margin-right: 4px; "
        "border-top-left-radius: 10px; border-top-right-radius: 10px; "
        "font-family: 'Segoe UI', Arial, sans-serif; font-size: 13px; font-weight: 600; "
        "border: 2px solid #E0E0E0; border-bottom: none; }"
        "QTabBar::tab:selected { background-color: #8D6E63; "
        "color: white; border-color: #8D6E63; }"
        "QTabBar::tab:hover:!selected { background-color: #E8E8E8; }"
        "QTableWidget { background-color: white; border: 1px solid #E0E0E0; "
        "gridline-color: #F0F0F0; font-family: 'Segoe UI', Arial, sans-serif; font-size: 12px; "
        "border-radius: 8px; }"
        "QTableWidget::item { padding: 12px; }"
        "QTableWidget::item:alternate { background-color: #FAFAFA; }"
        "QTableWidget::item:selected { background-color: #E3F2FD; color: #1976D2; }"
        "QHeaderView::section { background-color: #8D6E63; "
        "color: white; padding: 12px; font-weight: bold; border: none; "
        "font-size: 13px; }"
        "QPushButton#closeButton { background-color: #8D6E63; "
        "color: white; border: none; border-radius: 10px; padding: 12px 40px; "
        "font-family: 'Segoe UI', Arial, sans-serif; font-size: 13px; font-weight: bold; }"
        "QPushButton#closeButton:hover { background-color: #A0826D; }"
        "QPushButton#closeButton:pressed { background-color: #7D5E53; }"
    );
}

QString StatsRHDialog::adjustColorBrightness(const QString &color, int amount)
{
    QColor c(color);
    int h, s, v;
    c.getHsv(&h, &s, &v);
    v = qBound(0, v + amount, 255);
    c.setHsv(h, s, v);
    return c.name();
}

void StatsRHDialog::animateKPIs()
{
    QList<QWidget*> kpiCards = findChildren<QWidget*>("kpiCard");
    
    for (QWidget *card : kpiCards) {
        card->setVisible(true);
    }
}

// GraphiqueWidget implementation
GraphiqueWidget::GraphiqueWidget(TypeGraphique type, const QMap<QString, int> &data,
                                const QString &titre, QWidget *parent)
    : QWidget(parent)
    , m_type(type)
    , m_data(data)
    , m_titre(titre)
{
    setMinimumSize(600, 400);
    
    // Couleurs CUIREA professionnelles
    m_colors = {
        QColor(141, 110, 99),   // #8D6E63
        QColor(160, 130, 109),  // #A0826D
        QColor(188, 170, 164),  // #BCAAA4
        QColor(33, 150, 243),   // #2196F3
        QColor(76, 175, 80),    // #4CAF50
        QColor(255, 152, 0),    // #FF9800
        QColor(233, 30, 99),    // #E91E63
        QColor(156, 39, 176),   // #9C27B0
        QColor(0, 188, 212),    // #00BCD4
        QColor(205, 220, 57)    // #CDDC39
    };
}

void GraphiqueWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    switch (m_type) {
    case Donut:
        drawDonut(painter);
        break;
    case Barres:
        drawBarres(painter);
        break;
    case Camembert:
        drawCamembert(painter);
        break;
    }
}

void GraphiqueWidget::drawDonut(QPainter &painter)
{
    if (m_data.isEmpty()) return;
    
    int total = 0;
    for (int val : m_data) total += val;
    if (total == 0) return;
    
    // Zone de dessin - ajusté pour la légende en bas
    int centerX = width() / 2;
    int legendHeight = m_data.size() * 40 + 30;
    int availableHeight = height() - legendHeight;
    int centerY = availableHeight / 2 + 30;
    int radius = qMin(width(), availableHeight) / 3;
    int innerRadius = radius * 0.55;
    
    QRect rect(centerX - radius, centerY - radius, radius * 2, radius * 2);
    
    // Effet d'ombre pour le donut
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 30));
    painter.drawEllipse(centerX - radius + 5, centerY - radius + 5, radius * 2, radius * 2);
    
    // Dessiner les segments avec gradient
    int startAngle = 90 * 16;
    int colorIndex = 0;
    
    for (auto it = m_data.begin(); it != m_data.end(); ++it) {
        int spanAngle = -(it.value() * 360 * 16) / total;
        
        // Gradient radial pour effet 3D
        QColor baseColor = m_colors[colorIndex % m_colors.size()];
        QRadialGradient gradient(centerX, centerY, radius);
        gradient.setColorAt(0, baseColor.lighter(120));
        gradient.setColorAt(0.7, baseColor);
        gradient.setColorAt(1, baseColor.darker(110));
        
        painter.setBrush(gradient);
        painter.setPen(QPen(Qt::white, 3));
        painter.drawPie(rect, startAngle, spanAngle);
        
        startAngle += spanAngle;
        colorIndex++;
    }
    
    // Trou central avec gradient
    QRadialGradient centerGradient(centerX, centerY, innerRadius);
    centerGradient.setColorAt(0, Qt::white);
    centerGradient.setColorAt(1, QColor(245, 245, 245));
    painter.setBrush(centerGradient);
    painter.setPen(QPen(QColor(200, 200, 200), 2));
    painter.drawEllipse(centerX - innerRadius, centerY - innerRadius,
                       innerRadius * 2, innerRadius * 2);
    
    // Texte central avec ombre
    painter.setPen(QColor(0, 0, 0, 50));
    QFont font("Segoe UI", 26, QFont::Bold);
    painter.setFont(font);
    painter.drawText(rect.adjusted(2, 2, 2, 2), Qt::AlignCenter, QString::number(total));
    
    painter.setPen(QColor(141, 110, 99));
    painter.drawText(rect, Qt::AlignCenter, QString::number(total));
    
    QFont subFont("Segoe UI", 10);
    painter.setFont(subFont);
    painter.setPen(QColor(100, 100, 100));
    painter.drawText(rect.adjusted(0, 35, 0, 0), Qt::AlignCenter, "Employés");
    
    // Légende améliorée avec cartes - positionnée en bas
    int legendX = 15;
    int legendY = height() - legendHeight + 10;
    int maxLegendWidth = width() - 30;  // Largeur maximale avec marges réduites
    int cardWidth = qMin(380, maxLegendWidth);  // Adapter la largeur
    colorIndex = 0;
    
    for (auto it = m_data.begin(); it != m_data.end(); ++it) {
        // Carte de légende avec ombre
        QRect cardRect(legendX, legendY, cardWidth, 35);
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 20));
        painter.drawRoundedRect(cardRect.adjusted(2, 2, 2, 2), 6, 6);
        
        painter.setBrush(QColor(255, 255, 255, 250));
        painter.drawRoundedRect(cardRect, 6, 6);
        
        // Indicateur de couleur
        QColor color = m_colors[colorIndex % m_colors.size()];
        painter.setBrush(color);
        painter.drawRoundedRect(legendX + 6, legendY + 8, 18, 18, 4, 4);
        
        // Texte
        painter.setPen(QColor(41, 28, 14));
        QFont legendFont("Segoe UI", 9, QFont::DemiBold);
        painter.setFont(legendFont);
        double pct = (it.value() * 100.0) / total;
        QString text = QString("%1").arg(it.key());
        
        // Tronquer le texte si trop long
        QFontMetrics fm(legendFont);
        int maxTextWidth = cardWidth * 0.3;
        text = fm.elidedText(text, Qt::ElideRight, maxTextWidth);
        painter.drawText(legendX + 30, legendY + 20, text);
        
        // Valeur
        QFont valueFont("Segoe UI", 9, QFont::Bold);
        painter.setFont(valueFont);
        painter.setPen(color);
        QString valueText = QString("%1 (%2%)").arg(it.value()).arg(pct, 0, 'f', 1);
        int valueX = legendX + cardWidth * 0.35;
        painter.drawText(valueX, legendY + 20, valueText);
        
        // Barre de progression moderne
        int barWidth = cardWidth * 0.45;
        int barX = legendX + cardWidth * 0.52;
        int barY = legendY + 10;
        
        // S'assurer que la barre ne dépasse pas
        if (barX + barWidth > legendX + cardWidth - 8) {
            barWidth = legendX + cardWidth - 8 - barX;
        }
        
        // Fond de la barre
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(240, 240, 240));
        painter.drawRoundedRect(barX, barY, barWidth, 14, 7, 7);
        
        // Barre de progression avec gradient
        QLinearGradient barGradient(barX, barY, barX, barY + 14);
        barGradient.setColorAt(0, color.lighter(110));
        barGradient.setColorAt(1, color);
        painter.setBrush(barGradient);
        int fillWidth = barWidth * pct / 100;
        painter.drawRoundedRect(barX, barY, fillWidth, 14, 7, 7);
        
        legendY += 40;
        colorIndex++;
    }
}

void GraphiqueWidget::drawBarres(QPainter &painter)
{
    if (m_data.isEmpty()) return;
    
    int maxVal = 0;
    for (int val : m_data) {
        if (val > maxVal) maxVal = val;
    }
    if (maxVal == 0) return;
    
    // Marges
    int marginLeft = 70;
    int marginRight = 50;
    int marginTop = 70;
    int marginBottom = 120;
    
    int graphWidth = width() - marginLeft - marginRight;
    int graphHeight = height() - marginTop - marginBottom;
    
    // Fond du graphique
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(250, 250, 250));
    painter.drawRoundedRect(marginLeft, marginTop, graphWidth, graphHeight, 8, 8);
    
    // Grille horizontale
    painter.setPen(QPen(QColor(230, 230, 230), 1));
    for (int i = 0; i <= 5; i++) {
        int y = height() - marginBottom - (graphHeight * i) / 5;
        painter.drawLine(marginLeft, y, width() - marginRight, y);
    }
    
    // Axes
    painter.setPen(QPen(QColor(141, 110, 99), 3));
    painter.drawLine(marginLeft, marginTop, marginLeft, height() - marginBottom);
    painter.drawLine(marginLeft, height() - marginBottom,
                    width() - marginRight, height() - marginBottom);
    
    // Barres avec effet 3D
    int barCount = m_data.size();
    int spacing = 25;
    int barWidth = (graphWidth - (barCount + 1) * spacing) / barCount;
    int x = marginLeft + spacing;
    int colorIndex = 0;
    
    for (auto it = m_data.begin(); it != m_data.end(); ++it) {
        int barHeight = (it.value() * graphHeight) / maxVal;
        int y = height() - marginBottom - barHeight;
        
        QColor color = m_colors[colorIndex % m_colors.size()];
        
        // Ombre de la barre
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 40));
        painter.drawRoundedRect(x + 3, y + 3, barWidth, barHeight, 6, 6);
        
        // Barre avec gradient vertical
        QLinearGradient gradient(x, y, x, y + barHeight);
        gradient.setColorAt(0, color.lighter(115));
        gradient.setColorAt(0.5, color);
        gradient.setColorAt(1, color.darker(105));
        
        painter.setBrush(gradient);
        painter.setPen(QPen(color.darker(120), 2));
        painter.drawRoundedRect(x, y, barWidth, barHeight, 6, 6);
        
        // Reflet sur la barre
        QLinearGradient shine(x, y, x + barWidth, y);
        shine.setColorAt(0, QColor(255, 255, 255, 80));
        shine.setColorAt(0.5, QColor(255, 255, 255, 0));
        painter.setBrush(shine);
        painter.setPen(Qt::NoPen);
        painter.drawRoundedRect(x, y, barWidth / 3, barHeight, 6, 6);
        
        // Valeur au-dessus avec badge
        QFont valueFont("Segoe UI", 11, QFont::Bold);
        painter.setFont(valueFont);
        
        QString valueText = QString::number(it.value());
        QFontMetrics fm(valueFont);
        int textWidth = fm.horizontalAdvance(valueText);
        int badgeX = x + (barWidth - textWidth) / 2 - 8;
        int badgeY = y - 35;
        
        // Badge
        painter.setBrush(color);
        painter.drawRoundedRect(badgeX, badgeY, textWidth + 16, 26, 13, 13);
        
        painter.setPen(Qt::white);
        painter.drawText(badgeX + 8, badgeY + 18, valueText);
        
        // Label en bas avec rotation
        painter.save();
        painter.translate(x + barWidth / 2, height() - marginBottom + 15);
        painter.rotate(45);
        QFont labelFont("Segoe UI", 9, QFont::DemiBold);
        painter.setFont(labelFont);
        painter.setPen(QColor(60, 60, 60));
        painter.drawText(0, 0, it.key());
        painter.restore();
        
        x += barWidth + spacing;
        colorIndex++;
    }
    
    // Échelle Y avec style
    painter.setPen(QColor(100, 100, 100));
    QFont axisFont("Segoe UI", 9);
    painter.setFont(axisFont);
    
    for (int i = 0; i <= 5; i++) {
        int val = (maxVal * i) / 5;
        int y = height() - marginBottom - (graphHeight * i) / 5;
        painter.drawText(5, y - 8, marginLeft - 15, 20, Qt::AlignRight,
                        QString::number(val));
    }
}

void GraphiqueWidget::drawCamembert(QPainter &painter)
{
    if (m_data.isEmpty()) return;
    
    int total = 0;
    for (int val : m_data) total += val;
    if (total == 0) return;
    
    // Zone de dessin - ajusté pour laisser de la place à la légende
    int centerX = width() / 2;
    int centerY = (height() - 120) / 2 + 40;  // Remonter le graphique
    int radius = qMin(width(), height() - 180) / 3;  // Réduire le rayon
    
    QRect rect(centerX - radius, centerY - radius, radius * 2, radius * 2);
    
    // Ombre du camembert
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 40));
    painter.drawEllipse(centerX - radius + 6, centerY - radius + 6, radius * 2, radius * 2);
    
    // Dessiner les segments avec effet 3D
    int startAngle = 90 * 16;
    
    for (auto it = m_data.begin(); it != m_data.end(); ++it) {
        int spanAngle = -(it.value() * 360 * 16) / total;
        
        QColor color = (it.key() == "Hommes") ? QColor(25, 118, 210) : QColor(194, 24, 91);
        
        // Gradient radial pour effet 3D
        QRadialGradient gradient(centerX, centerY, radius);
        gradient.setColorAt(0, color.lighter(130));
        gradient.setColorAt(0.6, color);
        gradient.setColorAt(1, color.darker(110));
        
        painter.setBrush(gradient);
        painter.setPen(QPen(Qt::white, 4));
        painter.drawPie(rect, startAngle, spanAngle);
        
        // Pourcentage dans le segment avec badge
        double pct = (it.value() * 100.0) / total;
        double angle = (startAngle + spanAngle / 2) / 16.0;
        double rad = qDegreesToRadians(angle);
        int textX = centerX + (radius * 0.65) * qCos(rad);
        int textY = centerY - (radius * 0.65) * qSin(rad);
        
        // Badge pour le pourcentage
        QFont pctFont("Segoe UI", 16, QFont::Bold);
        painter.setFont(pctFont);
        QString pctText = QString::number(pct, 'f', 1) + "%";
        QFontMetrics fm(pctFont);
        int textWidth = fm.horizontalAdvance(pctText);
        int textHeight = fm.height();
        
        QRect badgeRect(textX - textWidth/2 - 12, textY - textHeight/2 - 6,
                       textWidth + 24, textHeight + 12);
        
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 255, 255, 240));
        painter.drawRoundedRect(badgeRect, 8, 8);
        
        painter.setPen(color.darker(120));
        painter.drawText(badgeRect, Qt::AlignCenter, pctText);
        
        startAngle += spanAngle;
    }
    
    // Légende moderne en bas - plus compacte
    int legendY = height() - 80;
    int legendSpacing = 240;
    int startX = (width() - (m_data.size() * legendSpacing)) / 2;
    
    for (auto it = m_data.begin(); it != m_data.end(); ++it) {
        QColor color = (it.key() == "Hommes") ? QColor(25, 118, 210) : QColor(194, 24, 91);
        
        // Carte de légende plus petite
        QRect cardRect(startX, legendY, 220, 60);
        
        // Ombre
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(0, 0, 0, 30));
        painter.drawRoundedRect(cardRect.adjusted(3, 3, 3, 3), 10, 10);
        
        // Fond avec couleur
        painter.setBrush(color);
        painter.drawRoundedRect(cardRect, 10, 10);
        
        // Icône
        QString icon = (it.key() == "Hommes") ? "👨" : "👩";
        QFont iconFont("Segoe UI", 24);
        painter.setFont(iconFont);
        painter.setPen(Qt::white);
        painter.drawText(startX + 12, legendY + 40, icon);
        
        // Texte
        QFont labelFont("Segoe UI", 12, QFont::Bold);
        painter.setFont(labelFont);
        painter.drawText(startX + 55, legendY + 25, it.key());
        
        QFont valueFont("Segoe UI", 16, QFont::Bold);
        painter.setFont(valueFont);
        painter.drawText(startX + 55, legendY + 48, QString::number(it.value()));
        
        startX += legendSpacing;
    }
}
