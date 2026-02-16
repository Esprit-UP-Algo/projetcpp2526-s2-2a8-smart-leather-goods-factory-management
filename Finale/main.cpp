#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    Connection c;
    
    // Connexion à la base de données des matières premières
    bool test = c.createconnect();
    
    // Connexion à la base de données de production
    bool testProduction = c.createconnectProduction();
    
    if (test && testProduction)
    {
        w.show();
        QMessageBox::information(nullptr, QObject::tr("Connexions établies"),
                                QObject::tr("Connexion aux bases de données réussie.\n"
                                          "- Matières Premières: OK\n"
                                          "- Production: OK\n"
                                          "Click OK to continue."), QMessageBox::Ok);
    }
    else
    {
        QString errorMsg = "Échec de connexion:\n";
        if (!test) errorMsg += "- Matières Premières: ÉCHEC\n";
        if (!testProduction) errorMsg += "- Production: ÉCHEC\n";
        
        QMessageBox::critical(nullptr, QObject::tr("Erreur de connexion"),
                            QObject::tr(errorMsg.toStdString().c_str()), QMessageBox::Cancel);
    }
    
    return a.exec();
}
