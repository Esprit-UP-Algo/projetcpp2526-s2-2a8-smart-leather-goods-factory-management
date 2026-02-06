#ifndef CLIENT_H
#define CLIENT_H

#include <QString>
#include <QDate>

class Client {
public:
    QString nom, prenom, sexe, cin, pays, ville, adresse, email;
    QDate dateInscrit;

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

#endif // CLIENT_H
