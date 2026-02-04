// mainwindow.h (relevant parts)
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QDate>

class Client {
public:
    QString nom, prenom, sexe, cin, pays, ville, adresse, email;
    QDate dateInscrit ;

    QString getNom() const { return nom; }
    QString getPrenom() const { return prenom; }
    QString getSexe() const { return sexe; }
    QString getCin() const { return cin; }
    QString getPays() const { return pays; }
    QString getVille() const { return ville; }
    QString getAdresse() const { return adresse; }
    QString getEmail() const { return email; }
    QDate getDateInscrit() const { return dateInscrit; }


    void setNom(const QString &v) { nom = v; }
    void setPrenom(const QString &v) { prenom = v; }
    void setSexe(const QString &v) { sexe = v; }
    void setCin(const QString &v) { cin = v; }
    void setPays(const QString &v) { pays = v; }
    void setVille(const QString &v) { ville = v; }
    void setAdresse(const QString &v) { adresse = v; }
    void setEmail(const QString &v) { email = v; }
    void setDateInscrit(const QDate &v) { dateInscrit = v; }
};


class QTableWidget;
class QLineEdit;
class QComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    QVector<Client> clients;
    int currentClientIndex = -1;

    // UI elements we need to access
    QTableWidget *tableClients = nullptr;
    QLineEdit *editNom = nullptr;
    QLineEdit *editPrenom = nullptr;
    QComboBox *comboSexe = nullptr;
    QLineEdit *editCin = nullptr;
    QLineEdit *editPays = nullptr;
    QLineEdit *editVille = nullptr;
    QLineEdit *editAdresse = nullptr;
    QLineEdit *editEmail = nullptr;
    QLineEdit *editDateInscrit = nullptr;

    void refreshTable();
    void clearForm();
    void fillFormFromCurrent();
    void saveCurrentToVector();
};

#endif // MAINWINDOW_H
