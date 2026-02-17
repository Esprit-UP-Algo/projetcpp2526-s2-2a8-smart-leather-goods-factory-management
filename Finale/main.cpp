#include "mainwindow.h"
#include <QApplication>
#include <QMessageBox>
#include "connection.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    // Vérifier les drivers disponibles
    QStringList drivers = QSqlDatabase::drivers();
    qDebug() << "Drivers SQL disponibles:" << drivers;
    
    // Obtenir l'instance unique de Connection et établir la connexion
    bool connected = Connection::getInstance().createConnection();
    
    if (connected) {
        QMessageBox::information(nullptr, "Connexion", "Connexion à la base de données réussie !");
    } else {
        QString errorMsg = "Échec de connexion à la base de données.\n\n";
        errorMsg += "Drivers disponibles: " + drivers.join(", ") + "\n\n";
        errorMsg += "Vérifiez que:\n";
        errorMsg += "1. Le driver QODBC est installé\n";
        errorMsg += "2. La source de données ODBC 'Source_Projet2A' existe\n";
        errorMsg += "3. Oracle Client est installé\n\n";
        errorMsg += "L'application va continuer sans connexion.";
        
        QMessageBox::warning(nullptr, "Connexion", errorMsg);
    }
    
    MainWindow w;
    w.show();
    
    return a.exec();
}
