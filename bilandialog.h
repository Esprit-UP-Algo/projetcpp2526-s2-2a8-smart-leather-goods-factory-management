#ifndef BILANDIALOG_H
#define BILANDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QPushButton>

/**
 * @class BilanDialog
 * @brief Fenêtre de bilan financier global basé sur les commandes en base de données.
 *
 * Affiche :
 *  - Le chiffre d'affaires total et la moyenne par commande (KPI)
 *  - La répartition du CA par priorité (Basse / Normale / Urgente) avec barres visuelles
 *  - Le nombre total de commandes
 *
 * Permet aussi d'exporter le bilan au format CSV.
 */
class BilanDialog : public QDialog
{
    Q_OBJECT
public:
    /** @brief Constructeur — construit l'UI et charge les données immédiatement. */
    explicit BilanDialog(QWidget *parent = nullptr);

private slots:
    /** @brief Exporte le bilan affiché dans un fichier CSV choisi par l'utilisateur. */
    void exportCSV();

private:
    /** @brief Construit tous les widgets de la fenêtre (en-tête, KPI, barres priorité, boutons). */
    void setupUI();

    /** @brief Interroge la base de données et met à jour tous les labels/barres. */
    void loadData();

    // ── Helpers de construction UI ──────────────────────────────────────────

    /**
     * @brief Crée une carte KPI (icône + titre + valeur + séparateur coloré).
     * @param icon     Emoji affiché à gauche du titre.
     * @param title    Libellé de l'indicateur.
     * @param valLabel Label dont le texte sera mis à jour par loadData().
     * @param color    Couleur du séparateur et de la valeur.
     * @param subtitle Texte secondaire optionnel sous la valeur.
     */
    QFrame *makeKpiCard(const QString &icon, const QString &title,
                        QLabel *valLabel, const QString &color,
                        const QString &subtitle = "");

    /**
     * @brief Crée une ligne de répartition priorité (icône + nom + barre + montant).
     * @param label   Nom de la priorité.
     * @param montant Montant total pour cette priorité.
     * @param total   Montant global (pour calculer le pourcentage de la barre).
     * @param color   Couleur de la barre et du texte.
     */
    QFrame *makePrioriteRow(const QString &label, double montant,
                            double total, const QString &color);

    // ── Labels mis à jour dynamiquement par loadData() ──────────────────────

    QLabel *m_lblCA;      ///< Chiffre d'affaires total
    QLabel *m_lblMoyen;   ///< Montant moyen par commande

    // Montants par priorité
    QLabel *m_lblBasse;
    QLabel *m_lblNormale;
    QLabel *m_lblUrgente;

    // Barres visuelles de répartition (largeur proportionnelle au CA)
    QFrame *m_barBasse;
    QFrame *m_barNormale;
    QFrame *m_barUrgente;

    QLabel *m_lblNbTotal; ///< Nombre total de commandes
    QLabel *m_lblDate;    ///< Date de génération du bilan
};

#endif // BILANDIALOG_H
