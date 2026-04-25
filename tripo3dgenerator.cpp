#include "tripo3dgenerator.h"
#include "envloader.h"
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <QFile>
#include <QDateTime>
#include <QUrlQuery>

// ══════════════════════════════════════════════════════════════════════════════
// APIs utilisées
// ══════════════════════════════════════════════════════════════════════════════
static const QString HF_API = "https://api-inference.huggingface.co/models/stabilityai/stable-diffusion-2-1";
static const QString GROQ_API = "https://api.groq.com/openai/v1/chat/completions";
static const QString POLLINATIONS_API = "https://image.pollinations.ai/prompt/";

Tripo3DGenerator::Tripo3DGenerator(QObject *parent)
    : QObject(parent), m_cancelled(false),
      m_articleLargeur(0), m_articleHauteur(0), m_articlePrix(0)
{
    m_network = new QNetworkAccessManager(this);
    
    // Charger les clés API
    m_hfToken = EnvLoader::get("HF_TOKEN");
    if (m_hfToken.isEmpty())
        m_hfToken = EnvLoader::get("HUGGINGFACE_TOKEN");
    
    m_groqKey = EnvLoader::get("GROQ_API_KEY");
    
    qDebug() << "[ImageGen] HF_TOKEN:" << (m_hfToken.isEmpty() ? "ABSENT" : "OK");
    qDebug() << "[ImageGen] GROQ_API_KEY:" << (m_groqKey.isEmpty() ? "ABSENT" : "OK");
}

// ══════════════════════════════════════════════════════════════════════════════
// Génération simple avec prompt direct
// ══════════════════════════════════════════════════════════════════════════════
void Tripo3DGenerator::generate(const QString &prompt)
{
    m_cancelled = false;
    m_currentPrompt = prompt;
    m_currentReference = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");

    if (!m_hfToken.isEmpty()) {
        qDebug() << "[ImageGen] Mode: Hugging Face Stable Diffusion";
        generateViaHuggingFace(prompt);
    } else {
        qDebug() << "[ImageGen] Mode: Pollinations AI (aucune cle)";
        generateViaPollinations(prompt);
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// Génération intelligente avec infos article
// ══════════════════════════════════════════════════════════════════════════════
void Tripo3DGenerator::generateForArticle(const QString &nom, const QString &type,
                                           const QString &categorie, const QString &couleurNom,
                                           double largeur, double hauteur, double prix,
                                           const QString &reference)
{
    m_cancelled = false;
    m_articleNom = nom;
    m_articleType = type;
    m_articleCategorie = categorie;
    m_articleCouleur = couleurNom;
    m_articleLargeur = largeur;
    m_articleHauteur = hauteur;
    m_articlePrix = prix;
    m_currentReference = reference.isEmpty() 
        ? QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") 
        : reference;

    // Si Groq disponible, générer un prompt intelligent
    if (!m_groqKey.isEmpty()) {
        qDebug() << "[ImageGen] Utilisation de Groq pour prompt intelligent";
        emit progress(5, "Generation du prompt IA via Groq...");
        generatePromptViaGroq();
    } else {
        // Sinon, utiliser le prompt par défaut
        m_currentPrompt = buildDefaultPrompt();
        qDebug() << "[ImageGen] Prompt par defaut:" << m_currentPrompt;
        
        if (!m_hfToken.isEmpty()) {
            generateViaHuggingFace(m_currentPrompt);
        } else {
            generateViaPollinations(m_currentPrompt);
        }
    }
}

void Tripo3DGenerator::cancel()
{
    m_cancelled = true;
}

// ══════════════════════════════════════════════════════════════════════════════
// Construction du prompt par défaut (sans Groq)
// ══════════════════════════════════════════════════════════════════════════════
QString Tripo3DGenerator::buildDefaultPrompt()
{
    // Traduire le type en anglais pour de meilleurs résultats
    QString typeEN = m_articleType.toLower();
    if (typeEN.contains("sac") && typeEN.contains("main")) typeEN = "handbag";
    else if (typeEN.contains("bandoul")) typeEN = "crossbody bag";
    else if (typeEN.contains("tote")) typeEN = "tote bag";
    else if (typeEN.contains("dos")) typeEN = "backpack";
    else if (typeEN.contains("portefeuille")) typeEN = "wallet";
    else if (typeEN.contains("porte-cartes") || typeEN.contains("carte")) typeEN = "card holder";
    else if (typeEN.contains("ceinture")) typeEN = "leather belt";
    else if (typeEN.contains("porte-monnaie") || typeEN.contains("monnaie")) typeEN = "coin purse";
    else if (typeEN.contains("pochette")) typeEN = "clutch bag";
    else if (typeEN.contains("cle")) typeEN = "key holder";
    else typeEN = "leather accessory";

    // Traduire la couleur
    QString couleurEN = m_articleCouleur.toLower();
    if (couleurEN.contains("noir")) couleurEN = "black";
    else if (couleurEN.contains("marron")) couleurEN = "brown";
    else if (couleurEN.contains("camel")) couleurEN = "camel tan";
    else if (couleurEN.contains("cognac")) couleurEN = "cognac";
    else if (couleurEN.contains("beige")) couleurEN = "beige";
    else if (couleurEN.contains("blanc")) couleurEN = "white";
    else if (couleurEN.contains("rouge")) couleurEN = "burgundy red";
    else if (couleurEN.contains("bleu")) couleurEN = "navy blue";
    else if (couleurEN.contains("gris")) couleurEN = "grey";
    else if (couleurEN.contains("vert")) couleurEN = "forest green";
    else couleurEN = "natural leather";

    return QString("professional product photography of a luxury %1 %2, "
                   "premium full grain leather, %3 color, "
                   "elegant stitching details, gold hardware accents, "
                   "studio lighting, pure white background, "
                   "high-end fashion catalog style, 4k, photorealistic")
           .arg(couleurEN, typeEN, couleurEN);
}

// ══════════════════════════════════════════════════════════════════════════════
// Génération du prompt via Groq (LLM)
// ══════════════════════════════════════════════════════════════════════════════
void Tripo3DGenerator::generatePromptViaGroq()
{
    QString systemPrompt = 
        "Tu es un expert en photographie produit de maroquinerie de luxe. "
        "Tu génères des prompts en anglais pour Stable Diffusion. "
        "Tes prompts produisent des images de qualité catalogue professionnel. "
        "Réponds UNIQUEMENT avec le prompt, sans explication, max 50 mots.";

    QString userPrompt = QString(
        "Génère un prompt Stable Diffusion pour photographier ce produit en cuir:\n"
        "- Nom: %1\n"
        "- Type: %2\n"
        "- Catégorie: %3\n"
        "- Couleur: %4\n"
        "- Dimensions: %5 x %6 cm\n"
        "- Gamme de prix: %7 DT\n\n"
        "Le prompt doit inclure: type de cuir, couleur, style, éclairage studio, fond blanc.")
        .arg(m_articleNom, m_articleType, m_articleCategorie, m_articleCouleur)
        .arg(m_articleLargeur, 0, 'f', 0)
        .arg(m_articleHauteur, 0, 'f', 0)
        .arg(m_articlePrix, 0, 'f', 0);

    QJsonArray messages;
    QJsonObject sys; sys["role"] = "system"; sys["content"] = systemPrompt;
    QJsonObject usr; usr["role"] = "user"; usr["content"] = userPrompt;
    messages.append(sys);
    messages.append(usr);

    QJsonObject body;
    body["model"] = "llama-3.3-70b-versatile";
    body["messages"] = messages;
    body["temperature"] = 0.7;
    body["max_tokens"] = 150;

    QUrl url(GROQ_API);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", ("Bearer " + m_groqKey).toUtf8());
    req.setTransferTimeout(15000);

    QNetworkReply *reply = m_network->post(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onGroqReplyFinished(reply);
    });
}

void Tripo3DGenerator::onGroqReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    if (m_cancelled) return;

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "[ImageGen] Groq erreur, utilisation prompt par defaut";
        m_currentPrompt = buildDefaultPrompt();
    } else {
        QByteArray data = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray choices = doc.object()["choices"].toArray();
        
        if (!choices.isEmpty()) {
            QString content = choices[0].toObject()["message"].toObject()["content"].toString().trimmed();
            if (!content.isEmpty() && content.length() > 20) {
                m_currentPrompt = content;
                qDebug() << "[ImageGen] Prompt Groq:" << m_currentPrompt;
            } else {
                m_currentPrompt = buildDefaultPrompt();
            }
        } else {
            m_currentPrompt = buildDefaultPrompt();
        }
    }

    emit progress(15, "Prompt genere, envoi a l'IA image...");

    // Maintenant générer l'image
    if (!m_hfToken.isEmpty()) {
        generateViaHuggingFace(m_currentPrompt);
    } else {
        generateViaPollinations(m_currentPrompt);
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// Hugging Face Inference API
// ══════════════════════════════════════════════════════════════════════════════
void Tripo3DGenerator::generateViaHuggingFace(const QString &prompt)
{
    emit progress(20, "Envoi a Hugging Face Stable Diffusion...");

    QJsonObject body;
    body["inputs"] = prompt;

    QUrl url(HF_API);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Authorization", ("Bearer " + m_hfToken).toUtf8());
    req.setTransferTimeout(90000); // 90s timeout

    QNetworkReply *reply = m_network->post(req, QJsonDocument(body).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onHFReplyFinished(reply);
    });

    // Progression simulée
    QTimer *progressTimer = new QTimer(this);
    int *step = new int(20);
    connect(progressTimer, &QTimer::timeout, this, [this, step, progressTimer]() {
        if (m_cancelled) { progressTimer->stop(); progressTimer->deleteLater(); delete step; return; }
        *step = qMin(*step + 5, 85);
        emit progress(*step, QString("Generation IA en cours... %1%").arg(*step));
    });
    progressTimer->start(2000);

    connect(reply, &QNetworkReply::finished, progressTimer, [progressTimer, step]() {
        progressTimer->stop(); progressTimer->deleteLater(); delete step;
    });
}

void Tripo3DGenerator::onHFReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    if (m_cancelled) return;

    int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "[ImageGen] HF erreur HTTP" << httpCode << reply->errorString();

        // Fallback vers Pollinations si erreur HF
        if (httpCode == 503 || httpCode == 429 || httpCode == 500) {
            emit progress(25, "HF indisponible, fallback Pollinations...");
            generateViaPollinations(m_currentPrompt);
            return;
        }
        emit error(QString("Erreur Hugging Face (HTTP %1)").arg(httpCode));
        return;
    }

    QByteArray data = reply->readAll();
    QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();

    // HF retourne directement les bytes de l'image
    if (contentType.contains("image") || data.size() > 5000) {
        emit progress(90, "Image recue, sauvegarde...");
        saveImageAndEmit(data);
    } else {
        // Réponse JSON = erreur ou modèle en chargement
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QString errMsg = doc.object()["error"].toString();
        qDebug() << "[ImageGen] HF reponse JSON:" << errMsg;
        
        if (errMsg.contains("loading", Qt::CaseInsensitive) || 
            errMsg.contains("currently loading", Qt::CaseInsensitive)) {
            emit progress(25, "Modele HF en chargement, fallback Pollinations...");
            generateViaPollinations(m_currentPrompt);
        } else {
            emit error("Erreur HF: " + errMsg);
        }
    }
}

// ══════════════════════════════════════════════════════════════════════════════
// Pollinations AI (fallback gratuit sans clé)
// ══════════════════════════════════════════════════════════════════════════════
void Tripo3DGenerator::generateViaPollinations(const QString &prompt)
{
    emit progress(30, "Generation via Pollinations AI...");

    // Nettoyer le prompt pour l'URL
    QString cleanPrompt = prompt;
    cleanPrompt.replace("\"", "");
    cleanPrompt.replace("'", "");
    QString encodedPrompt = QUrl::toPercentEncoding(cleanPrompt);
    
    QString urlStr = POLLINATIONS_API + encodedPrompt + "?width=512&height=512&nologo=true&seed=" + 
                     QString::number(QDateTime::currentMSecsSinceEpoch() % 100000);

    QUrl url(urlStr);
    QNetworkRequest req(url);
    req.setTransferTimeout(60000);

    QNetworkReply *reply = m_network->get(req);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onPollinationsReplyFinished(reply);
    });

    // Progression simulée
    QTimer *progressTimer = new QTimer(this);
    int *step = new int(30);
    connect(progressTimer, &QTimer::timeout, this, [this, step, progressTimer]() {
        if (m_cancelled) { progressTimer->stop(); progressTimer->deleteLater(); delete step; return; }
        *step = qMin(*step + 4, 85);
        emit progress(*step, QString("Pollinations AI... %1%").arg(*step));
    });
    progressTimer->start(2500);

    connect(reply, &QNetworkReply::finished, progressTimer, [progressTimer, step]() {
        progressTimer->stop(); progressTimer->deleteLater(); delete step;
    });
}

void Tripo3DGenerator::onPollinationsReplyFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    if (m_cancelled) return;

    if (reply->error() != QNetworkReply::NoError) {
        emit error("Erreur Pollinations: " + reply->errorString());
        return;
    }

    QByteArray data = reply->readAll();
    if (data.size() < 1000) {
        emit error("Image Pollinations invalide (trop petite)");
        return;
    }

    emit progress(90, "Image recue, sauvegarde...");
    saveImageAndEmit(data);
}

// ══════════════════════════════════════════════════════════════════════════════
// Sauvegarde de l'image
// ══════════════════════════════════════════════════════════════════════════════
void Tripo3DGenerator::saveImageAndEmit(const QByteArray &imageData)
{
    // Créer le dossier photos_articles
    QString dir = QCoreApplication::applicationDirPath() + "/photos_articles";
    QDir().mkpath(dir);

    // Nom de fichier basé sur la référence article
    QString safeRef = m_currentReference;
    safeRef.replace("/", "_").replace("\\", "_").replace(" ", "_");
    QString filename = dir + "/" + safeRef + ".png";

    // Si le fichier existe déjà, ajouter un timestamp
    if (QFile::exists(filename)) {
        QString timestamp = QDateTime::currentDateTime().toString("_HHmmss");
        filename = dir + "/" + safeRef + timestamp + ".png";
    }

    QFile f(filename);
    if (f.open(QIODevice::WriteOnly)) {
        f.write(imageData);
        f.close();
        m_lastImagePath = filename;
        qDebug() << "[ImageGen] Image sauvegardee:" << filename << "(" << imageData.size() << "bytes)";
        emit progress(100, "Generation terminee !");
        emit finished(filename, m_currentReference);
    } else {
        emit error("Impossible de sauvegarder: " + filename);
    }
}
