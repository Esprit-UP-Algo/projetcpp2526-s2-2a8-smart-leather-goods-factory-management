#include "mainwindow.h"
#include "logindialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
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
