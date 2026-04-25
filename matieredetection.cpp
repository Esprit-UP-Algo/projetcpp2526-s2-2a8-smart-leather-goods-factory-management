#include "matieredetection.h"
#include "mainwindow.h"
#include <QMainWindow>
#include <QStatusBar>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressBar>
#include <QTimer>
#include <QPointer>
#include <QFile>
#include <QFileInfo>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonArray>
#include <QNetworkRequest>
#include <QFrame>

MatiereDetection::MatiereDetection(MainWindow *mainWin, QTableWidget *table, 
                                   QNetworkAccessManager *netMgr, const QString &api)
    : QObject(mainWin), mainWindow(mainWin), matiereTable(table), 
      networkManager(netMgr), apiUrl(api)
{
}

MatiereDetection::~MatiereDetection()
{
}

void MatiereDetection::startDetection()
{
    showSelectionDialog();
}

void MatiereDetection::showSelectionDialog()
{
    // Créer une boîte de dialogue améliorée pour la détection
    QDialog *detectionDialog = new QDialog(mainWindow);
    detectionDialog->setWindowTitle("Détection de Défauts - Cuir");
    detectionDialog->setFixedSize(650, 550);
    detectionDialog->setStyleSheet(
        "QDialog { background: #FAF5F0; }"
        "QLabel { color: #291C0E; }"
        "QPushButton { border-radius: 8px; padding: 10px 20px; font-weight: bold; }"
        "QComboBox { background: white; border: 2px solid #BCAAA4; border-radius: 6px; padding: 8px; }"
        "QGroupBox { font-weight: bold; border: 2px solid #BCAAA4; border-radius: 8px; margin-top: 10px; padding-top: 15px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 15px; padding: 0 5px; }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(detectionDialog);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    
    // Titre
    QLabel *titleLabel = new QLabel("🔬 Analyse de Qualité du Cuir", detectionDialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #8D6E63; margin-bottom: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // Description
    QLabel *descLabel = new QLabel("Sélectionnez une image de matière première pour détecter les défauts potentiels.", detectionDialog);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    descLabel->setStyleSheet("color: #5D4037; font-size: 12px; margin-bottom: 15px;");
    mainLayout->addWidget(descLabel);
    
    // Zone de prévisualisation de l'image
    QLabel *previewLabel = new QLabel(detectionDialog);
    previewLabel->setFixedSize(200, 200);
    previewLabel->setAlignment(Qt::AlignCenter);
    previewLabel->setStyleSheet(
        "background: #E8E0D8; border: 3px dashed #BCAAA4; border-radius: 12px;"
    );
    previewLabel->setText("📷\nAucune image\nsélectionnée");
    
    QHBoxLayout *previewLayout = new QHBoxLayout();
    previewLayout->addStretch();
    previewLayout->addWidget(previewLabel);
    previewLayout->addStretch();
    mainLayout->addLayout(previewLayout);
    
    // Variable pour stocker le chemin de l'image
    QString *selectedImagePath = new QString();
    
    // GroupBox: Sélection depuis le tableau
    QGroupBox *tableGroup = new QGroupBox("📋 Sélectionner depuis le tableau des matières", detectionDialog);
    QVBoxLayout *tableLayout = new QVBoxLayout(tableGroup);
    
    QComboBox *matiereCombo = new QComboBox(detectionDialog);
    matiereCombo->addItem("-- Choisir une matière première --", "");
    
    // Remplir le combo avec les matières ayant une photo
    for (int row = 0; row < matiereTable->rowCount(); ++row) {
        QString photoPath = matiereTable->item(row, 0)->data(Qt::UserRole + 1).toString();
        if (!photoPath.isEmpty() && QFile::exists(photoPath)) {
            QString matiereName = matiereTable->item(row, 0)->text();
            QString matiereRef = matiereTable->item(row, 1)->text();
            matiereCombo->addItem(QString("📦 %1 (%2)").arg(matiereName, matiereRef), photoPath);
        }
    }
    
    if (matiereCombo->count() == 1) {
        matiereCombo->setItemText(0, "-- Aucune matière avec photo disponible --");
        matiereCombo->setEnabled(false);
    }
    
    tableLayout->addWidget(matiereCombo);
    mainLayout->addWidget(tableGroup);
    
    // GroupBox: Sélection depuis fichier
    QGroupBox *fileGroup = new QGroupBox("📁 Ou importer depuis un fichier", detectionDialog);
    QHBoxLayout *fileLayout = new QHBoxLayout(fileGroup);
    
    QPushButton *browseBtn = new QPushButton("🖼️ Parcourir...", detectionDialog);
    browseBtn->setStyleSheet(
        "QPushButton { background: #6D4C41; color: white; }"
        "QPushButton:hover { background: #8D6E63; }"
    );
    fileLayout->addWidget(browseBtn);
    
    QLabel *filePathLabel = new QLabel("Aucun fichier sélectionné", detectionDialog);
    filePathLabel->setStyleSheet("color: #757575; font-style: italic;");
    fileLayout->addWidget(filePathLabel, 1);
    mainLayout->addWidget(fileGroup);
    
    // Boutons d'action
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    QPushButton *analyzeBtn = new QPushButton("🔍 Analyser", detectionDialog);
    analyzeBtn->setStyleSheet(
        "QPushButton { background: #4CAF50; color: white; font-size: 14px; padding: 12px 30px; }"
        "QPushButton:hover { background: #66BB6A; }"
        "QPushButton:disabled { background: #BDBDBD; }"
    );
    analyzeBtn->setEnabled(false);
    
    QPushButton *cancelBtn = new QPushButton("Annuler", detectionDialog);
    cancelBtn->setStyleSheet(
        "QPushButton { background: #BCAAA4; color: white; }"
        "QPushButton:hover { background: #A1887F; }"
    );
    
    buttonLayout->addWidget(analyzeBtn);
    buttonLayout->addWidget(cancelBtn);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    
    // Connexions
    auto updatePreview = [previewLabel, analyzeBtn, selectedImagePath](const QString &path) {
        *selectedImagePath = path;
        if (!path.isEmpty() && QFile::exists(path)) {
            QPixmap pixmap(path);
            previewLabel->setPixmap(pixmap.scaled(190, 190, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            analyzeBtn->setEnabled(true);
        } else {
            previewLabel->setText("📷\nAucune image\nsélectionnée");
            analyzeBtn->setEnabled(false);
        }
    };
    
    connect(matiereCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
            [matiereCombo, filePathLabel, updatePreview](int index) {
        QString path = matiereCombo->itemData(index).toString();
        if (!path.isEmpty()) {
            filePathLabel->setText("Image du tableau");
            updatePreview(path);
        }
    });
    
    connect(browseBtn, &QPushButton::clicked, [detectionDialog, filePathLabel, matiereCombo, updatePreview]() {
        QString imagePath = QFileDialog::getOpenFileName(
            detectionDialog,
            "Sélectionner une image de cuir",
            "",
            "Images (*.png *.jpg *.jpeg *.bmp *.tiff)"
        );
        if (!imagePath.isEmpty()) {
            QFileInfo fi(imagePath);
            filePathLabel->setText(fi.fileName());
            matiereCombo->setCurrentIndex(0);
            updatePreview(imagePath);
        }
    });
    
    connect(cancelBtn, &QPushButton::clicked, detectionDialog, &QDialog::reject);
    
    connect(analyzeBtn, &QPushButton::clicked, [this, detectionDialog, selectedImagePath]() {
        if (selectedImagePath->isEmpty()) {
            QMessageBox::warning(detectionDialog, "Attention", "Veuillez sélectionner une image.");
            return;
        }
        detectionDialog->accept();
        startDetectionAnalysis(*selectedImagePath);
    });
    
    detectionDialog->exec();
    delete selectedImagePath;
    detectionDialog->deleteLater();
}

void MatiereDetection::startDetectionAnalysis(const QString &imagePath)
{
    // Créer une boîte de dialogue de connexion
    QDialog *waitDialog = new QDialog(mainWindow);
    waitDialog->setWindowTitle("Connexion au serveur");
    waitDialog->setModal(true);
    waitDialog->setFixedSize(400, 180);
    waitDialog->setStyleSheet("QDialog { background: #FAF5F0; }");
    
    QVBoxLayout *waitLayout = new QVBoxLayout(waitDialog);
    waitLayout->setSpacing(15);
    waitLayout->setContentsMargins(25, 25, 25, 25);
    
    QLabel *iconLabel = new QLabel("🔄", waitDialog);
    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setStyleSheet("font-size: 40px;");
    waitLayout->addWidget(iconLabel);
    
    QLabel *waitLabel = new QLabel("Connexion au serveur de détection...", waitDialog);
    waitLabel->setAlignment(Qt::AlignCenter);
    waitLabel->setStyleSheet("font-size: 14px; color: #5D4037;");
    waitLayout->addWidget(waitLabel);
    
    QProgressBar *waitProgress = new QProgressBar(waitDialog);
    waitProgress->setRange(0, 0);
    waitProgress->setStyleSheet(
        "QProgressBar { border: 2px solid #BCAAA4; border-radius: 5px; background: white; }"
        "QProgressBar::chunk { background: #8D6E63; }"
    );
    waitLayout->addWidget(waitProgress);
    
    QPushButton *cancelBtn = new QPushButton("Annuler", waitDialog);
    cancelBtn->setStyleSheet(
        "QPushButton { background: #BCAAA4; color: white; border-radius: 6px; padding: 8px 20px; }"
        "QPushButton:hover { background: #A1887F; }"
    );
    waitLayout->addWidget(cancelBtn, 0, Qt::AlignCenter);
    
    // Timer pour les tentatives de connexion
    QTimer *retryTimer = new QTimer(waitDialog);
    int *attempt = new int(0);
    int maxRetries = 10;  // 10 secondes d'attente
    
    connect(cancelBtn, &QPushButton::clicked, [waitDialog, retryTimer]() {
        retryTimer->stop();
        waitDialog->close();
        waitDialog->deleteLater();
    });
    
    connect(retryTimer, &QTimer::timeout, [this, imagePath, waitDialog, waitLabel, retryTimer, attempt, maxRetries]() {
        QNetworkRequest healthRequest(QUrl(apiUrl + "/health"));
        healthRequest.setTransferTimeout(2000);
        QNetworkReply *healthReply = networkManager->get(healthRequest);
        
        connect(healthReply, &QNetworkReply::finished, [this, imagePath, healthReply, waitDialog, waitLabel, retryTimer, attempt, maxRetries]() {
            healthReply->deleteLater();
            
            if (healthReply->error() == QNetworkReply::NoError) {
                retryTimer->stop();
                waitDialog->close();
                waitDialog->deleteLater();
                delete attempt;
                detectDefectsInImage(imagePath);
                return;
            }
            
            (*attempt)++;
            waitLabel->setText(QString("Attente du serveur... (%1/%2)").arg(*attempt).arg(maxRetries));
            
            if (*attempt >= maxRetries) {
                retryTimer->stop();
                delete attempt;
                
                // Fermer le dialog d'attente silencieusement (pas de message d'erreur)
                waitDialog->close();
                QTimer::singleShot(100, [waitDialog]() {
                    waitDialog->deleteLater();
                });
            }
        });
    });
    
    waitDialog->show();
    retryTimer->start(1000);
}

void MatiereDetection::detectDefectsInImage(const QString &imagePath)
{
    // Créer une boîte de dialogue de progression
    QDialog *progressDialog = new QDialog(mainWindow);
    progressDialog->setWindowTitle("Détection en cours");
    progressDialog->setModal(true);
    progressDialog->setFixedSize(400, 150);
    
    QVBoxLayout *layout = new QVBoxLayout(progressDialog);
    
    QLabel *statusLabel = new QLabel("Analyse de l'image en cours...", progressDialog);
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);
    
    QProgressBar *progressBar = new QProgressBar(progressDialog);
    progressBar->setRange(0, 0); // Mode indéterminé
    layout->addWidget(progressBar);
    
    progressDialog->show();
    
    // Lire et encoder l'image en base64
    QFile imageFile(imagePath);
    if (!imageFile.open(QIODevice::ReadOnly)) {
        progressDialog->close();
        progressDialog->deleteLater();
        QMessageBox::critical(mainWindow, "Erreur", "Impossible de lire l'image");
        return;
    }
    
    QByteArray imageData = imageFile.readAll();
    imageFile.close();
    
    // Vérifier que l'image n'est pas trop grande (max 10MB)
    if (imageData.size() > 10 * 1024 * 1024) {
        progressDialog->close();
        progressDialog->deleteLater();
        QMessageBox::warning(mainWindow, "Attention", 
            "L'image est trop volumineuse (> 10MB).\n"
            "Veuillez utiliser une image plus petite.");
        return;
    }
    
    QString base64Image = imageData.toBase64();
    
    // Préparer la requête JSON
    QJsonObject requestData;
    requestData["image"] = "data:image/jpeg;base64," + base64Image;
    
    QJsonDocument doc(requestData);
    QByteArray jsonData = doc.toJson();
    
    // Envoyer la requête à l'API
    QNetworkRequest request(QUrl(apiUrl + "/predict"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setTransferTimeout(60000);  // Timeout de 60 secondes pour l'analyse
    
    QNetworkReply *reply = networkManager->post(request, jsonData);
    
    // Utiliser QPointer pour éviter les dangling pointers
    QPointer<QDialog> dialogPtr(progressDialog);
    
    connect(reply, &QNetworkReply::finished, this, [this, reply, dialogPtr]() {
        // Vérifier que le dialog existe encore
        if (dialogPtr) {
            dialogPtr->close();
            dialogPtr->deleteLater();
        }
        
        if (reply->error() != QNetworkReply::NoError) {
            QString errorDetails = QString("Code: %1\nMessage: %2\nURL: %3")
                .arg(reply->error())
                .arg(reply->errorString())
                .arg(reply->url().toString());
            
            // Lire la réponse même en cas d'erreur pour voir le message du serveur
            QByteArray responseData = reply->readAll();
            if (!responseData.isEmpty()) {
                errorDetails += QString("\n\nRéponse du serveur:\n%1")
                    .arg(QString::fromUtf8(responseData));
            }
            
            QMessageBox::critical(mainWindow, "Erreur de détection", errorDetails);
            reply->deleteLater();
            return;
        }
        
        // Traiter la réponse
        QByteArray responseData = reply->readAll();
        
        // Vérifier que la réponse n'est pas vide
        if (responseData.isEmpty()) {
            QMessageBox::critical(mainWindow, "Erreur", "Réponse vide du serveur");
            reply->deleteLater();
            return;
        }
        
        // Parser le JSON
        QJsonParseError parseError;
        QJsonDocument responseDoc = QJsonDocument::fromJson(responseData, &parseError);
        
        if (parseError.error != QJsonParseError::NoError) {
            QMessageBox::critical(mainWindow, "Erreur JSON", 
                QString("Erreur de parsing JSON:\n%1\n\nRéponse brute:\n%2")
                    .arg(parseError.errorString())
                    .arg(QString::fromUtf8(responseData)));
            reply->deleteLater();
            return;
        }
        
        QJsonObject response = responseDoc.object();
        
        // Vérifier le champ "success"
        if (!response.contains("success")) {
            QMessageBox::critical(mainWindow, "Erreur", 
                "La réponse ne contient pas le champ 'success'");
            reply->deleteLater();
            return;
        }
        
        if (response["success"].toBool()) {
            showDetectionResults(response);
        } else {
            QString errorMsg = response["error"].toString();
            if (errorMsg.isEmpty()) {
                errorMsg = "Erreur inconnue";
            }
            QMessageBox::critical(mainWindow, "Erreur", 
                "Échec de la détection:\n" + errorMsg);
        }
        
        reply->deleteLater();
    });
}

QString MatiereDetection::translateDefectClass(const QString &englishName)
{
    // Traduction des classes de défauts en français
    static QMap<QString, QString> translations = {
        {"Folding marks", "Marques de pliage"},
        {"Grain off", "Grain détaché"},
        {"Growth marks", "Marques de croissance"},
        {"loose grains", "Grains lâches"},
        {"non defective", "Non défectueux"},
        {"pinhole", "Trou d'épingle"}
    };
    
    return translations.value(englishName, englishName);
}

bool MatiereDetection::isCuirMaterial(const QJsonObject &results)
{
    QJsonArray allPredictions = results["all_predictions"].toArray();
    QJsonObject prediction = results["prediction"].toObject();
    QString className = prediction["class_name"].toString().toLower();
    double confidence = prediction["confidence_percent"].toDouble();
    
    qDebug() << "=== ANALYSE CUIR ===";
    qDebug() << "Classe principale:" << className;
    qDebug() << "Confiance:" << confidence << "%";
    
    // Si c'est "non defective" avec une confiance raisonnable, c'est du cuir
    if (className == "non defective" && confidence > 30.0) {
        qDebug() << "✓ Cuir non défectueux détecté";
        return true; // C'EST DU CUIR
    }
    
    // Liste des défauts connus du cuir
    QStringList defectClasses = {"folding marks", "grain off", "growth marks", 
                                "loose grains", "pinhole"};
    
    // Si c'est un défaut de cuir avec une confiance élevée
    if (defectClasses.contains(className) && confidence > 50.0) {
        
        // CAS SPÉCIAL: "Folding marks" à très haute confiance (>95%) avec toutes autres à 0%
        // Peut être un document/tableau
        if (className == "folding marks" && confidence >= 95.0) {
            int nonZeroPredictions = 0;
            for (int i = 0; i < allPredictions.size(); ++i) {
                QJsonObject predObj = allPredictions[i].toObject();
                double conf = predObj["confidence_percent"].toDouble();
                if (conf > 0.5) nonZeroPredictions++;
            }
            
            if (nonZeroPredictions == 1) {
                qDebug() << "⚠ Pattern suspect: Folding marks isolé à" << confidence << "% - probablement document";
                return false; // NON CUIR
            }
        }
        
        qDebug() << "✓ Défaut de cuir détecté:" << className;
        return true; // C'EST DU CUIR
    }
    
    // Analyser la distribution générale
    int nonZeroPredictions = 0;
    double totalConfidence = 0.0;
    double maxOtherConfidence = 0.0;
    
    for (int i = 0; i < allPredictions.size(); ++i) {
        QJsonObject predObj = allPredictions[i].toObject();
        QString predClass = predObj["class_name"].toString().toLower();
        double conf = predObj["confidence_percent"].toDouble();
        totalConfidence += conf;
        
        if (conf > 0.1) {
            nonZeroPredictions++;
        }
        
        if (predClass != className && conf > maxOtherConfidence) {
            maxOtherConfidence = conf;
        }
    }
    
    qDebug() << "Prédictions non-nulles:" << nonZeroPredictions;
    qDebug() << "Confiance totale:" << totalConfidence;
    qDebug() << "Max autre confiance:" << maxOtherConfidence;
    
    // Cas très suspects (probablement pas du cuir)
    if (confidence < 5.0 && totalConfidence < 15.0) {
        qDebug() << "✗ Toutes les confiances sont très faibles";
        return false; // NON CUIR
    }
    
    if (confidence < 10.0 && nonZeroPredictions > 4 && maxOtherConfidence > confidence * 0.8) {
        qDebug() << "✗ Confiances dispersées et incertaines";
        return false; // NON CUIR
    }
    
    // Par défaut, si on arrive ici avec une confiance raisonnable, c'est du cuir
    if (confidence >= 15.0) {
        qDebug() << "✓ Considéré comme cuir (confiance suffisante)";
        return true; // C'EST DU CUIR
    }
    
    qDebug() << "✗ Confiance trop faible:" << confidence << "%";
    return false; // NON CUIR
}

void MatiereDetection::showNonCuirResult(const QString &materialType)
{
    Q_UNUSED(materialType);
    // Créer le dialogue pour "NON CUIR"
    QDialog *resultDialog = new QDialog(mainWindow);
    resultDialog->setWindowTitle("Résultats de Détection");
    resultDialog->setModal(true);
    resultDialog->setFixedSize(520, 450);
    resultDialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FAFAFA, stop:1 #F0EBE6); }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(resultDialog);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // === HEADER avec gradient BLEU (pour NON CUIR) ===
    QFrame *headerFrame = new QFrame(resultDialog);
    headerFrame->setFixedHeight(180);
    headerFrame->setStyleSheet(
        "QFrame { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #2196F3, stop:1 #42A5F5); "
        "border-top-left-radius: 0px; border-top-right-radius: 0px; }"
    );
    
    QVBoxLayout *headerLayout = new QVBoxLayout(headerFrame);
    headerLayout->setAlignment(Qt::AlignCenter);
    headerLayout->setSpacing(8);
    
    // Icône circulaire
    QLabel *iconCircle = new QLabel("ℹ", headerFrame);
    iconCircle->setFixedSize(70, 70);
    iconCircle->setAlignment(Qt::AlignCenter);
    iconCircle->setStyleSheet(
        "QLabel { background: rgba(255,255,255,0.25); color: white; font-size: 36px; "
        "font-weight: bold; border-radius: 35px; }"
    );
    headerLayout->addWidget(iconCircle, 0, Qt::AlignCenter);
    
    // Texte de statut
    QLabel *statusLabel = new QLabel("MATÉRIAU NON CUIR", headerFrame);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(
        "QLabel { color: white; font-size: 20px; font-weight: bold; letter-spacing: 2px; }"
    );
    headerLayout->addWidget(statusLabel);
    
    // Type de matériau détecté
    QLabel *materialLabel = new QLabel("Matériau non identifié", headerFrame);
    materialLabel->setAlignment(Qt::AlignCenter);
    materialLabel->setStyleSheet(
        "QLabel { color: rgba(255,255,255,0.9); font-size: 14px; font-weight: 500; }"
    );
    headerLayout->addWidget(materialLabel);
    
    mainLayout->addWidget(headerFrame);
    
    // === CONTENU ===
    QWidget *contentWidget = new QWidget(resultDialog);
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(20);
    contentLayout->setContentsMargins(25, 25, 25, 20);
    
    // Message d'information
    QFrame *infoCard = new QFrame(contentWidget);
    infoCard->setStyleSheet(
        "QFrame { background: white; border-radius: 12px; border: 2px solid #E3F2FD; }"
    );
    QVBoxLayout *infoLayout = new QVBoxLayout(infoCard);
    infoLayout->setContentsMargins(20, 15, 20, 15);
    
    QLabel *infoTitle = new QLabel("🔍 Analyse du Matériau", infoCard);
    infoTitle->setStyleSheet("QLabel { color: #1976D2; font-size: 14px; font-weight: bold; }");
    infoLayout->addWidget(infoTitle);
    
    QLabel *infoText = new QLabel(
        "L'image analysée ne correspond pas à du cuir.\n\n"
        "• Vérifiez que l'image contient bien du cuir\n"
        "• Assurez-vous que l'éclairage est suffisant\n"
        "• Évitez les reflets ou ombres importantes\n"
        "• Utilisez une image de bonne qualité", infoCard);
    infoText->setStyleSheet("QLabel { color: #424242; font-size: 12px; line-height: 1.4; }");
    infoText->setWordWrap(true);
    infoLayout->addWidget(infoText);
    
    contentLayout->addWidget(infoCard);
    
    // Recommandations
    QFrame *recommendCard = new QFrame(contentWidget);
    recommendCard->setStyleSheet(
        "QFrame { background: #FFF3E0; border-radius: 12px; border: 2px solid #FFB74D; }"
    );
    QVBoxLayout *recommendLayout = new QVBoxLayout(recommendCard);
    recommendLayout->setContentsMargins(20, 15, 20, 15);
    
    QLabel *recommendTitle = new QLabel("💡 Recommandations", recommendCard);
    recommendTitle->setStyleSheet("QLabel { color: #F57C00; font-size: 14px; font-weight: bold; }");
    recommendLayout->addWidget(recommendTitle);
    
    QLabel *recommendText = new QLabel(
        "Pour une détection optimale :\n"
        "• Utilisez des échantillons de cuir uniquement\n"
        "• Photographiez sur fond neutre\n"
        "• Maintenez une distance appropriée", recommendCard);
    recommendText->setStyleSheet("QLabel { color: #E65100; font-size: 12px; }");
    recommendText->setWordWrap(true);
    recommendLayout->addWidget(recommendText);
    
    contentLayout->addWidget(recommendCard);
    contentLayout->addStretch();
    
    // Boutons
    QHBoxLayout *btnLayout = new QHBoxLayout();
    
    QPushButton *retryBtn = new QPushButton("🔄 Réessayer", contentWidget);
    retryBtn->setFixedSize(140, 44);
    retryBtn->setCursor(Qt::PointingHandCursor);
    retryBtn->setStyleSheet(
        "QPushButton { background: #2196F3; color: white; border: none; border-radius: 22px; "
        "font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background: #42A5F5; }"
        "QPushButton:pressed { background: #1976D2; }"
    );
    connect(retryBtn, &QPushButton::clicked, [this, resultDialog]() {
        resultDialog->accept();
        startDetection(); // Relancer la détection
    });
    
    QPushButton *closeBtn = new QPushButton("Fermer", contentWidget);
    closeBtn->setFixedSize(140, 44);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { background: #BDBDBD; color: white; border: none; border-radius: 22px; "
        "font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background: #9E9E9E; }"
        "QPushButton:pressed { background: #757575; }"
    );
    connect(closeBtn, &QPushButton::clicked, resultDialog, &QDialog::accept);
    
    btnLayout->addStretch();
    btnLayout->addWidget(retryBtn);
    btnLayout->addWidget(closeBtn);
    btnLayout->addStretch();
    contentLayout->addLayout(btnLayout);
    
    mainLayout->addWidget(contentWidget);
    
    resultDialog->exec();
    resultDialog->deleteLater();
}

void MatiereDetection::showDetectionResults(const QJsonObject &results)
{
    // Vérifier que les champs nécessaires existent
    if (!results.contains("prediction") || !results.contains("all_predictions")) {
        QMessageBox::critical(mainWindow, "Erreur", 
            "Format de réponse invalide: champs manquants");
        return;
    }
    
    // NOUVELLE LOGIQUE: Vérifier d'abord si c'est du cuir
    if (!isCuirMaterial(results)) {
        showNonCuirResult("Matériau non identifié comme cuir");
        return;
    }
    
    QJsonObject prediction = results["prediction"].toObject();
    
    if (!prediction.contains("class_name") || !prediction.contains("confidence_percent")) {
        QMessageBox::critical(mainWindow, "Erreur", 
            "Format de prédiction invalide");
        return;
    }
    
    QString className = prediction["class_name"].toString();
    QString classNameFr = translateDefectClass(className);
    double confidence = prediction["confidence_percent"].toDouble();
    
    // Déterminer le style selon le résultat
    bool isDefective = (className.toLower() != "non defective");
    QString statusIcon, statusText, gradientStart, gradientEnd, accentColor;
    
    if (!isDefective) {
        statusIcon = "✓";
        statusText = "QUALITÉ VALIDÉE";
        gradientStart = "#43A047";
        gradientEnd = "#66BB6A";
        accentColor = "#2E7D32";
    } else if (confidence > 70) {
        statusIcon = "✗";
        statusText = "DÉFAUT DÉTECTÉ";
        gradientStart = "#E53935";
        gradientEnd = "#EF5350";
        accentColor = "#C62828";
    } else {
        statusIcon = "!";
        statusText = "À VÉRIFIER";
        gradientStart = "#FB8C00";
        gradientEnd = "#FFA726";
        accentColor = "#EF6C00";
    }
    
    // Créer le dialogue principal
    QDialog *resultDialog = new QDialog(mainWindow);
    resultDialog->setWindowTitle("Résultats de Détection");
    resultDialog->setModal(true);
    resultDialog->setFixedSize(520, 580);
    resultDialog->setStyleSheet(
        "QDialog { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #FAFAFA, stop:1 #F0EBE6); }"
    );
    
    QVBoxLayout *mainLayout = new QVBoxLayout(resultDialog);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    
    // === HEADER avec gradient ===
    QFrame *headerFrame = new QFrame(resultDialog);
    headerFrame->setFixedHeight(180);
    headerFrame->setStyleSheet(QString(
        "QFrame { background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 %1, stop:1 %2); "
        "border-top-left-radius: 0px; border-top-right-radius: 0px; }"
    ).arg(gradientStart, gradientEnd));
    
    QVBoxLayout *headerLayout = new QVBoxLayout(headerFrame);
    headerLayout->setAlignment(Qt::AlignCenter);
    headerLayout->setSpacing(8);
    
    // Icône circulaire
    QLabel *iconCircle = new QLabel(statusIcon, headerFrame);
    iconCircle->setFixedSize(70, 70);
    iconCircle->setAlignment(Qt::AlignCenter);
    iconCircle->setStyleSheet(
        "QLabel { background: rgba(255,255,255,0.25); color: white; font-size: 36px; "
        "font-weight: bold; border-radius: 35px; }"
    );
    headerLayout->addWidget(iconCircle, 0, Qt::AlignCenter);
    
    // Texte de statut
    QLabel *statusLabel = new QLabel(statusText, headerFrame);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(
        "QLabel { color: white; font-size: 20px; font-weight: bold; letter-spacing: 2px; }"
    );
    headerLayout->addWidget(statusLabel);
    
    // Nom du défaut
    QLabel *defectLabel = new QLabel(classNameFr, headerFrame);
    defectLabel->setAlignment(Qt::AlignCenter);
    defectLabel->setStyleSheet(
        "QLabel { color: rgba(255,255,255,0.9); font-size: 14px; font-weight: 500; }"
    );
    headerLayout->addWidget(defectLabel);
    
    mainLayout->addWidget(headerFrame);
    
    // === CONTENU ===
    QWidget *contentWidget = new QWidget(resultDialog);
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(20);
    contentLayout->setContentsMargins(25, 25, 25, 20);
    
    // Carte de confiance
    QFrame *confidenceCard = new QFrame(contentWidget);
    confidenceCard->setStyleSheet(
        "QFrame { background: white; border-radius: 12px; }"
    );
    QHBoxLayout *confLayout = new QHBoxLayout(confidenceCard);
    confLayout->setContentsMargins(20, 15, 20, 15);
    
    QLabel *confTitle = new QLabel("Niveau de Confiance", confidenceCard);
    confTitle->setStyleSheet("QLabel { color: #5D4037; font-size: 13px; font-weight: 600; }");
    confLayout->addWidget(confTitle);
    
    confLayout->addStretch();
    
    QLabel *confValue = new QLabel(QString("%1%").arg(confidence, 0, 'f', 1), confidenceCard);
    confValue->setStyleSheet(QString(
        "QLabel { color: %1; font-size: 24px; font-weight: bold; }"
    ).arg(accentColor));
    confLayout->addWidget(confValue);
    
    contentLayout->addWidget(confidenceCard);
    
    // Section Analyse Détaillée
    QLabel *detailsTitle = new QLabel("Analyse Complète", contentWidget);
    detailsTitle->setStyleSheet(
        "QLabel { color: #5D4037; font-size: 14px; font-weight: bold; margin-top: 5px; }"
    );
    contentLayout->addWidget(detailsTitle);
    
    // Carte des détails
    QFrame *detailsCard = new QFrame(contentWidget);
    detailsCard->setStyleSheet(
        "QFrame { background: white; border-radius: 12px; }"
    );
    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsCard);
    detailsLayout->setSpacing(12);
    detailsLayout->setContentsMargins(18, 18, 18, 18);
    
    QJsonArray allPredictions = results["all_predictions"].toArray();
    
    for (int i = 0; i < allPredictions.size(); ++i) {
        QJsonObject predObj = allPredictions[i].toObject();
        QString name = predObj["class_name"].toString();
        QString nameFr = translateDefectClass(name);
        double conf = predObj["confidence_percent"].toDouble();
        
        QWidget *rowWidget = new QWidget(detailsCard);
        QHBoxLayout *rowLayout = new QHBoxLayout(rowWidget);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(12);
        
        // Indicateur coloré
        QLabel *dot = new QLabel(rowWidget);
        dot->setFixedSize(8, 8);
        QString dotColor = (name.toLower() == "non defective") ? "#4CAF50" : 
                          (conf > 50) ? "#F44336" : "#BDBDBD";
        dot->setStyleSheet(QString(
            "QLabel { background: %1; border-radius: 4px; }"
        ).arg(dotColor));
        rowLayout->addWidget(dot);
        
        // Nom
        QLabel *nameLabel = new QLabel(nameFr, rowWidget);
        nameLabel->setStyleSheet("QLabel { color: #424242; font-size: 12px; }");
        nameLabel->setMinimumWidth(140);
        rowLayout->addWidget(nameLabel);
        
        // Barre de progression stylée
        QFrame *barBg = new QFrame(rowWidget);
        barBg->setFixedHeight(8);
        barBg->setStyleSheet("QFrame { background: #EEEEEE; border-radius: 4px; }");
        
        QFrame *barFill = new QFrame(barBg);
        int fillWidth = static_cast<int>(conf * 1.2); // Max 120px
        barFill->setFixedSize(fillWidth, 8);
        barFill->move(0, 0);
        QString fillColor = (name.toLower() == "non defective") ? "#4CAF50" : "#8D6E63";
        barFill->setStyleSheet(QString(
            "QFrame { background: %1; border-radius: 4px; }"
        ).arg(fillColor));
        
        rowLayout->addWidget(barBg, 1);
        
        // Pourcentage
        QLabel *percLabel = new QLabel(QString("%1%").arg(conf, 0, 'f', 1), rowWidget);
        percLabel->setFixedWidth(50);
        percLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        QString percColor = (conf > 50) ? "#424242" : "#9E9E9E";
        percLabel->setStyleSheet(QString(
            "QLabel { color: %1; font-size: 12px; font-weight: 600; }"
        ).arg(percColor));
        rowLayout->addWidget(percLabel);
        
        detailsLayout->addWidget(rowWidget);
    }
    
    contentLayout->addWidget(detailsCard);
    contentLayout->addStretch();
    
    // Bouton Fermer
    QPushButton *closeBtn = new QPushButton("Fermer", contentWidget);
    closeBtn->setFixedSize(160, 44);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(QString(
        "QPushButton { background: %1; color: white; border: none; border-radius: 22px; "
        "font-size: 14px; font-weight: bold; }"
        "QPushButton:hover { background: %2; }"
        "QPushButton:pressed { background: %1; }"
    ).arg(gradientStart, gradientEnd));
    connect(closeBtn, &QPushButton::clicked, resultDialog, &QDialog::accept);
    
    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(closeBtn);
    btnLayout->addStretch();
    contentLayout->addLayout(btnLayout);
    
    mainLayout->addWidget(contentWidget);
    
    resultDialog->exec();
    resultDialog->deleteLater();
}
