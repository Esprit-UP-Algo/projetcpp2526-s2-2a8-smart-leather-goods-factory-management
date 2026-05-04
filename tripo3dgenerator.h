#ifndef TRIPO3DGENERATOR_H
#define TRIPO3DGENERATOR_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QString>

// ══════════════════════════════════════════════════════════════════════════════
// Générateur d'images produit IA pour articles en cuir
// 
// Flow: 1. Groq génère un prompt pro (optionnel)
//       2. Hugging Face Stable Diffusion génère l'image
//       3. Fallback: Pollinations AI (sans clé)
// ══════════════════════════════════════════════════════════════════════════════
class Tripo3DGenerator : public QObject
{
    Q_OBJECT
public:
    explicit Tripo3DGenerator(QObject *parent = nullptr);

    // Génération simple avec prompt direct
    void generate(const QString &prompt);
    
    // Génération intelligente avec infos article (utilise Groq pour le prompt)
    void generateForArticle(const QString &nom, const QString &type, 
                            const QString &categorie, const QString &couleurNom,
                            double largeur, double hauteur, double prix,
                            const QString &reference);
    
    void cancel();
    
    // Accesseur pour le chemin de la dernière image
    QString lastImagePath() const { return m_lastImagePath; }

signals:
    void progress(int percent, const QString &status);
    void finished(const QString &imagePath, const QString &reference);
    void error(const QString &message);

private slots:
    void onGroqReplyFinished(QNetworkReply *reply);
    void onHFReplyFinished(QNetworkReply *reply);
    void onPollinationsReplyFinished(QNetworkReply *reply);

private:
    void generatePromptViaGroq();
    void generateViaHuggingFace(const QString &prompt);
    void generateViaPollinations(const QString &prompt);
    void saveImageAndEmit(const QByteArray &imageData);
    QString buildDefaultPrompt();

    QNetworkAccessManager *m_network;
    QString m_hfToken;
    QString m_groqKey;
    QString m_currentPrompt;
    QString m_currentReference;
    bool m_cancelled;
    QString m_lastImagePath;
    
    // Infos article pour prompt intelligent
    QString m_articleNom;
    QString m_articleType;
    QString m_articleCategorie;
    QString m_articleCouleur;
    double m_articleLargeur;
    double m_articleHauteur;
    double m_articlePrix;
};

#endif // TRIPO3DGENERATOR_H
