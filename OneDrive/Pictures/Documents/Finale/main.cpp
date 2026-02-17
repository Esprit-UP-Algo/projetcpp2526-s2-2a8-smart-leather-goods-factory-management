#include "mainwindow.h"
#include "logindialog.h"
#include "connection.h"
#include <QApplication>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Tester la connexion à Oracle
    Connection* conn = Connection::getInstance();
    
    if (!conn->createconnect()) {
        QMessageBox::critical(nullptr, "Erreur",
            "Impossible de se connecter à Oracle.\n"
            "Vérifiez vos paramètres dans connection.h\n\n"
            "Assurez-vous que:\n"
            "1. Oracle est démarré\n"
            "2. La source ODBC est configurée\n"
            "3. Les identifiants sont corrects");
        return -1;
    }
    
    QMessageBox::information(nullptr, "Succès",
        "✅ Connexion à Oracle établie !");
    
    // Afficher la page de login
    LoginDialog loginDialog;
    if (loginDialog.exec() == QDialog::Accepted) {
        // Si la connexion réussit, afficher la fenêtre principale
        MainWindow w;
        w.setWindowTitle(QString("CUIREA - Connecté en tant que: %1").arg(loginDialog.getUsername()));
        w.show();
        return a.exec();
    }
    
    // Si l'utilisateur annule, quitter l'application
    return 0;
}
