#include "mainwindow.h"
#include "logindialog.h"
#include "connection.h"
#include "envloader.h"
#include "usersession.h"
#include "notification.h"
#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QCoreApplication>
#include <QStyle>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Icône tray système (notifications Windows même app minimisée)
    NotificationWidget::initTray(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));

    // Charger les variables d'environnement depuis .env
    // Cherche d'abord à côté de l'exécutable, puis dans le répertoire source
    QString envPath = QCoreApplication::applicationDirPath() + "/.env";
    if (!QFile::exists(envPath))
        envPath = QCoreApplication::applicationDirPath() + "/../../.env";
    if (!QFile::exists(envPath))
        envPath = QCoreApplication::applicationDirPath() + "/../../../.env";
    if (!QFile::exists(envPath))
        envPath = QCoreApplication::applicationDirPath() + "/../../../../.env";
    if (!QFile::exists(envPath))
        envPath = ".env";
    qDebug() << "Loading .env from:" << envPath << "exists:" << QFile::exists(envPath);
    EnvLoader::load(envPath);    
    // Établir la connexion à la base de données
    Connection* conn = Connection::instance();
    if (!conn->createConnect()) {
        QMessageBox::critical(nullptr, "Erreur de Connexion", 
            "Impossible de se connecter à la base de données Projet2A.\n\n"
            "Vérifiez que:\n"
            "1. Oracle Client est installé\n"
            "2. La source ODBC 'Projet2A' est configurée\n"
            "3. Les identifiants sont corrects (cuirea/0000)");
        conn->closeConnection();
        return -1;
    }
    
    int result = 0;
    
    // Afficher la page de login
    LoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted) {
        // Si la connexion réussit, afficher la fenêtre principale
        MainWindow w;
        
        // Appliquer les permissions basées sur le rôle de l'utilisateur
        w.applyUserPermissions();
        
        UserSession &session = UserSession::instance();
        w.setWindowTitle(QString("CUIREA - %1 (%2)").arg(session.getNomComplet()).arg(session.getRoleString()));
        w.showMaximized();  // ← Plein écran au démarrage
        result = a.exec();
    }
    
    // Fermer la connexion à la sortie (toujours exécuté)
    conn->closeConnection();
    
    return result;
}
