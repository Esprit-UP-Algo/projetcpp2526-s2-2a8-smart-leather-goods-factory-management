#include "voicematieres.h"
#include "mainwindow.h"
#include "matieredialog.h"
#include "matiere.h"
#include <QMainWindow>
#include <QStatusBar>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QTimer>
#include <QRegularExpression>
#include <QDate>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

VoiceMatieres::VoiceMatieres(MainWindow *mainWin, QTableWidget *table)
    : QObject(mainWin), mainWindow(mainWin), matiereTable(table)
{
}

VoiceMatieres::~VoiceMatieres()
{
}

QString VoiceMatieres::cellText(QTableWidget *table, int row, int col)
{
    QTableWidgetItem *item = table->item(row, col);
    return item ? item->text() : "";
}

void VoiceMatieres::openVoiceDialog()
{
    // Dialog avec champ texte pour saisie vocale via Win+H
    QDialog dlg(mainWindow);
    dlg.setWindowTitle("Commande Vocale");
    dlg.setMinimumWidth(420);
    dlg.setStyleSheet(
        "QDialog { background-color: #FAF5F0; }"
        "QLabel { color: #291C0E; font-size: 13px; }"
        "QLineEdit { background: white; border: 2px solid #8D6E63; border-radius: 8px; "
        "padding: 10px; font-size: 14px; color: #291C0E; }"
        "QPushButton { background-color: #8D6E63; color: white; border: none; "
        "border-radius: 6px; padding: 8px 20px; font-size: 12px; font-weight: bold; }"
        "QPushButton:hover { background-color: #A0826D; }");

    QVBoxLayout lay(&dlg);
    lay.setSpacing(12);
    lay.setContentsMargins(20, 20, 20, 20);
    
    auto *info = new QLabel("🎤 Appuyez sur  Win + H  puis dites votre commande :", &dlg);
    info->setWordWrap(true);
    
    auto *hint = new QLabel(
        "<small style='color:#8D6E63;'>"
        "Commandes : <b>ajouter</b> · <b>modifier</b> · <b>supprimer</b> · "
        "<b>statistiques</b> · <b>exporter</b> · <b>détection</b> · "
        "<b>trier</b> · <b>rechercher [mot]</b>"
        "</small>", &dlg);
    hint->setWordWrap(true);
    hint->setTextFormat(Qt::RichText);
    
    auto *input = new QLineEdit(&dlg);
    input->setPlaceholderText("La commande apparaîtra ici...");
    
    QHBoxLayout btnLay;
    auto *btnOk     = new QPushButton("✔ Exécuter", &dlg);
    auto *btnCancel = new QPushButton("Annuler",    &dlg);
    btnCancel->setStyleSheet(
        "QPushButton { background-color: #E7DDD1; color: #291C0E; border: 1px solid #BCAAA4; "
        "border-radius: 6px; padding: 8px 20px; }"
        "QPushButton:hover { background-color: #F0E6DA; }");
    
    btnLay.addStretch();
    btnLay.addWidget(btnOk);
    btnLay.addWidget(btnCancel);
    
    lay.addWidget(info);
    lay.addWidget(hint);
    lay.addWidget(input);
    lay.addLayout(&btnLay);
    
    connect(btnOk,     &QPushButton::clicked, &dlg, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);
    connect(input, &QLineEdit::returnPressed, &dlg, &QDialog::accept);
    
    // Focus sur le champ et déclencher Win+H automatiquement
    input->setFocus();
    QTimer::singleShot(300, [&]() {
#ifdef Q_OS_WIN
        // Simuler Win+H pour ouvrir la saisie vocale Windows sur le champ
        INPUT inputs[4] = {};
        inputs[0].type = INPUT_KEYBOARD;
        inputs[0].ki.wVk = VK_LWIN;
        inputs[1].type = INPUT_KEYBOARD;
        inputs[1].ki.wVk = 'H';
        inputs[2].type = INPUT_KEYBOARD;
        inputs[2].ki.wVk = 'H';
        inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
        inputs[3].type = INPUT_KEYBOARD;
        inputs[3].ki.wVk = VK_LWIN;
        inputs[3].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(4, inputs, sizeof(INPUT));
#endif
    });
    
    if (dlg.exec() == QDialog::Accepted) {
        QString cmd = input->text().trimmed().toLower();
        if (!cmd.isEmpty())
            processVoiceCommand(cmd);
    }
}

void VoiceMatieres::processVoiceCommand(const QString &rawCmd)
{
    // Normaliser : minuscules, supprimer ponctuation finale
    QString cmd = rawCmd.toLower().trimmed();
    cmd.remove(QRegularExpression("[.!?,;]$"));
    
    QString feedback = "🎤 \"" + cmd + "\"";
    
    // ── Ajouter ──────────────────────────────────────────────
    if (cmd.contains("ajouter") || cmd.contains("ajoutez") || cmd.contains("ajoute") ||
        cmd.contains("ajout")   || cmd.contains("créer")   || cmd.contains("créez")  ||
        cmd.contains("creer")   || cmd.contains("cree")    || cmd.contains("créé")   ||
        cmd.contains("nouvelle") || cmd.contains("nouveau") || cmd.contains("insérer") ||
        cmd.contains("inserer") || cmd.contains("add")) {

        // Extraire quantité : "quantité 2.9" / "quantite 5"
        QRegularExpression reQty("(quantit[eé]|quantite)\\s+(\\d+[.,]?\\d*)");
        auto mQty = reQty.match(cmd);
        QString qty = mQty.hasMatch() ? mQty.captured(2).replace(",", ".") : "";

        // Extraire nom/module : "module ahmed" / "avec module ahmed" / "nom aaa"
        QRegularExpression reModule("(?:avec\\s+)?module\\s+(\\w+)");
        auto mModule = reModule.match(cmd);
        QString nom = "";
        if (mModule.hasMatch()) {
            nom = mModule.captured(1);
        } else {
            // Fallback: chercher "nom xxx"
            QRegularExpression reNom("(nom|nomm[eé]e?|appel[eé]e?)\\s+(\\w+)");
            auto mNom = reNom.match(cmd);
            nom = mNom.hasMatch() ? mNom.captured(2) : "";
        }

        // Extraire référence : "référence dd" / "ref dd" / "avec référence dd"
        QRegularExpression reRef("(?:avec\\s+)?(r[eé]f[eé]rence|ref|r[eé]f)\\s+(\\w+)");
        auto mRef = reRef.match(cmd);
        QString ref = mRef.hasMatch() ? mRef.captured(2) : "";

        // Extraire type : "type cuir" / "de type peau"
        QRegularExpression reType("(?:avec\\s+)?(type|de type)\\s+(\\w+(?:\\s+\\w+)?)");
        auto mType = reType.match(cmd);
        QString type = mType.hasMatch() ? mType.captured(2) : "";

        // Extraire seuil : "seuil 100"
        QRegularExpression reSeuil("(?:avec\\s+)?(seuil)\\s+(\\d+)");
        auto mSeuil = reSeuil.match(cmd);
        QString seuil = mSeuil.hasMatch() ? mSeuil.captured(2) : "";

        MatiereDialog dlg(mainWindow, MatiereDialog::AddMode);
        if (!nom.isEmpty() || !qty.isEmpty() || !ref.isEmpty() || !seuil.isEmpty())
            dlg.setMatiereData(nom, ref, type, qty, seuil, "");
        
        if (dlg.exec() == QDialog::Accepted) {
            Matiere m;
            m.setNom(dlg.getModule());
            m.setReference(dlg.getReference());
            m.setType(dlg.getType());
            m.setQuantite(dlg.getQuantite().toDouble());
            m.setSeuil(dlg.getSeuil().toInt());
            m.setDateExpiration(QDate::fromString(dlg.getDateExpiration(), "yyyy-MM-dd"));
            m.setPhotoUrl(dlg.getPhotoUrl());
            if (m.ajouter()) {
                // Appeler les méthodes de MainWindow via pointeur
                mainWindow->setupMatiereTable();
                mainWindow->updateMatiereStatistics();
            }
        }

    // ── Modifier ─────────────────────────────────────────────
    } else if (cmd.contains("modifier") || cmd.contains("modifiez") || cmd.contains("modifie") ||
               cmd.contains("changer")  || cmd.contains("changez")  || cmd.contains("éditer") ||
               cmd.contains("mettre à jour") || cmd.contains("première matière")) {
        
        // Extraire nom pour sélectionner la ligne
        QRegularExpression reNomCible("(mati[eè]re|matiere)\\s+(\\w+)");
        auto mNomCible = reNomCible.match(cmd);
        if (mNomCible.hasMatch()) {
            QString nomCible = mNomCible.captured(2);
            for (int r = 0; r < matiereTable->rowCount(); ++r) {
                if (cellText(matiereTable, r, 0).toLower().contains(nomCible)) {
                    matiereTable->selectRow(r);
                    break;
                }
            }
        }
        
        int row = matiereTable->currentRow();
        if (row < 0) {
            QMessageBox::warning(mainWindow, "Modifier", "Veuillez sélectionner une matière à modifier.");
            return;
        }
        
        // Extraire les modifications demandées
        QRegularExpression reQty("(quantit[eé]|quantite)\\s+([àa]\\s+|pour\\s+)?(\\d+[.,]?\\d*)");
        auto mQty = reQty.match(cmd);
        QString qty = mQty.hasMatch() ? mQty.captured(3).replace(",", ".") : "";
        
        QRegularExpression reSeuil("(seuil)\\s+([àa]\\s+|pour\\s+)?(\\d+)");
        auto mSeuil = reSeuil.match(cmd);
        QString seuil = mSeuil.hasMatch() ? mSeuil.captured(3) : "";
        
        QRegularExpression reType("(type)\\s+([àa]\\s+|pour\\s+)?(\\w+(?:\\s+\\w+)?)");
        auto mType = reType.match(cmd);
        QString type = mType.hasMatch() ? mType.captured(3) : "";
        
        QRegularExpression reRef("(r[eé]f[eé]rence|ref)\\s+([àa]\\s+|pour\\s+)?(\\w+)");
        auto mRef = reRef.match(cmd);
        QString ref = mRef.hasMatch() ? mRef.captured(3) : "";
        
        QRegularExpression reNom("(nom|module)\\s+([àa]\\s+|pour\\s+)?(\\w+(?:\\s+\\w+)?)");
        auto mNom = reNom.match(cmd);
        QString nom = mNom.hasMatch() ? mNom.captured(3) : "";
        
        QRegularExpression reDate("(date|expiration)\\s+([àa]\\s+|pour\\s+)?(\\d{4})[\\s\\-\\.](\\d{2})[\\s\\-\\.](\\d{2})");
        auto mDate = reDate.match(cmd);
        QString dateStr = mDate.hasMatch() ? mDate.captured(3) + "-" + mDate.captured(4) + "-" + mDate.captured(5) : "";
        
        MatiereDialog dlg(mainWindow, MatiereDialog::EditMode);
        dlg.setMatiereData(
            cellText(matiereTable, row, 0),
            cellText(matiereTable, row, 1),
            cellText(matiereTable, row, 2),
            cellText(matiereTable, row, 3).remove(" m²"),
            cellText(matiereTable, row, 4),
            cellText(matiereTable, row, 5),
            ""
        );
        
        // Appliquer les modifications vocales
        if (!qty.isEmpty())   dlg.setQuantite(qty);
        if (!seuil.isEmpty()) dlg.setSeuil(seuil);
        if (!type.isEmpty())  dlg.setTypeMatiere(type);
        if (!ref.isEmpty())   dlg.setReference(ref);
        if (!nom.isEmpty())   dlg.setModule(nom);
        if (!dateStr.isEmpty()) dlg.setDateExpiration(dateStr);
        
        if (dlg.exec() == QDialog::Accepted) {
            Matiere m;
            m.setId(cellText(matiereTable, row, 0).toInt());
            m.setNom(dlg.getModule());
            m.setReference(dlg.getReference());
            m.setType(dlg.getType());
            m.setQuantite(dlg.getQuantite().toDouble());
            m.setSeuil(dlg.getSeuil().toInt());
            m.setDateExpiration(QDate::fromString(dlg.getDateExpiration(), "yyyy-MM-dd"));
            m.setPhotoUrl(dlg.getPhotoUrl());
            if (m.modifier()) {
                mainWindow->setupMatiereTable();
                mainWindow->updateMatiereStatistics();
            }
        }
        
    // ── Supprimer ─────────────────────────────────────────────
    } else if (cmd.contains("supprimer") || cmd.contains("supprimez") || cmd.contains("supprime") ||
               cmd.contains("effacer")   || cmd.contains("effacez")   || cmd.contains("enlever") ||
               cmd.contains("retirer")   || cmd.contains("enlève")) {
        
        QRegularExpression reNom("(mati[eè]re|matiere)\\s+(\\w+)");
        auto mNom = reNom.match(cmd);
        if (mNom.hasMatch()) {
            QString nomCible = mNom.captured(2);
            for (int r = 0; r < matiereTable->rowCount(); ++r) {
                if (cellText(matiereTable, r, 0).toLower().contains(nomCible)) {
                    matiereTable->selectRow(r);
                    break;
                }
            }
        }
        mainWindow->onDeleteMatiere();
        
    // ── Rechercher ────────────────────────────────────────────
    } else if (cmd.contains("rechercher") || cmd.contains("recherche") ||
               cmd.contains("chercher")   || cmd.contains("trouver")   ||
               cmd.contains("filtrer")    || cmd.contains("afficher")) {
        
        // Extraire le terme : "rechercher cuir" / "chercher stock critique"
        QString term = cmd;
        for (const QString &kw : QStringList{"rechercher", "recherche", "chercher", "trouver",
                                   "filtrer", "afficher", "la matière", "matière", "les matières"})
            term.remove(kw);
        term = term.trimmed();
        
        // Détection de filtres spéciaux
        if (term.contains("critique") || term.contains("stock critique")) {
            // Filtrer stock critique
            for (int r = 0; r < matiereTable->rowCount(); ++r) {
                int qty = cellText(matiereTable, r, 3).remove(" m²").toInt();
                int seuil = cellText(matiereTable, r, 4).toInt();
                matiereTable->setRowHidden(r, qty >= seuil);
            }
            mainWindow->statusBar()->showMessage("🔍 Affichage : stock critique", 3000);
        } else if (term.contains("expiré") || term.contains("expire") || term.contains("périmé")) {
            // Filtrer matières expirées
            for (int r = 0; r < matiereTable->rowCount(); ++r) {
                QString dateStr = cellText(matiereTable, r, 5);
                QDate expDate = QDate::fromString(dateStr, "yyyy-MM-dd");
                int daysLeft = QDate::currentDate().daysTo(expDate);
                matiereTable->setRowHidden(r, daysLeft >= 0);
            }
            mainWindow->statusBar()->showMessage("🔍 Affichage : matières expirées", 3000);
        } else if (!term.isEmpty()) {
            // Recherche textuelle simple
            QString lower = term.toLower();
            for (int r = 0; r < matiereTable->rowCount(); ++r) {
                bool match = false;
                for (int c = 0; c < 3; ++c) { // Nom, Ref, Type
                    if (cellText(matiereTable, r, c).toLower().contains(lower)) {
                        match = true;
                        break;
                    }
                }
                matiereTable->setRowHidden(r, !match);
            }
            mainWindow->statusBar()->showMessage("🔍 Recherche : " + term, 3000);
        } else {
            // Ouvrir le dialog de recherche avancée
            mainWindow->onRechercheTriMatiere();
        }
        
    // ── Statistiques ──────────────────────────────────────────
    } else if (cmd.contains("statistique") || cmd.contains("stats") ||
               cmd.contains("statistiques")) {
        mainWindow->onStatistiquesMatiere();
        
    // ── Exporter ──────────────────────────────────────────────
    } else if (cmd.contains("export") || cmd.contains("exporter") ||
               cmd.contains("exportez") || cmd.contains("pdf") ||
               cmd.contains("rapport")) {
        mainWindow->onExportMatiere();
        
    // ── Détection ─────────────────────────────────────────────
    } else if (cmd.contains("détection") || cmd.contains("detection") ||
               cmd.contains("détecter")  || cmd.contains("analyser") ||
               cmd.contains("défaut")    || cmd.contains("defaut")) {
        mainWindow->onDetectionDefauts();
        
    // ── Trier ─────────────────────────────────────────────────
    } else if (cmd.contains("trier") || cmd.contains("tri") ||
               cmd.contains("trie")  || cmd.contains("classer") || cmd.contains("ordonner")) {
        mainWindow->onTriMatiere();
        
    } else {
        QMessageBox::information(mainWindow, "Vocal",
            "Commande non reconnue : \"" + cmd + "\"\n\n"
            "Exemples :\n"
            "• \"ajouter matière\"\n"
            "• \"modifier la matière dd\"\n"
            "• \"supprimer la matière cuir\"\n"
            "• \"rechercher peau de veau\"\n"
            "• \"statistiques\"\n"
            "• \"exporter\"\n"
            "• \"détection\"\n"
            "• \"trier\"");
        return;
    }
    
    mainWindow->statusBar()->showMessage(feedback, 3000);
}
