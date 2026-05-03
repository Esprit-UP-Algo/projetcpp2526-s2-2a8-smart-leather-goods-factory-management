#include "metierdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QMessageBox>

static const QString DIALOG_STYLE =
    "QDialog { background-color: #FAF5F0; }"
    "QLabel { color: #291C0E; font-size: 12px; font-weight: bold; }"
    "QLineEdit, QComboBox, QDoubleSpinBox, QTextEdit {"
    "  background-color: white; border: 2px solid #BCAAA4;"
    "  border-radius: 6px; padding: 6px; font-size: 12px; color: #291C0E; }"
    "QLineEdit:focus, QComboBox:focus, QDoubleSpinBox:focus, QTextEdit:focus {"
    "  border-color: #8D6E63; }"
    "QPushButton { background-color: #8D6E63; color: white; border: none;"
    "  border-radius: 6px; padding: 8px 20px; font-size: 12px; font-weight: bold; min-width: 80px; }"
    "QPushButton:hover { background-color: #A0826D; }"
    "QPushButton[flat=true] { background-color: #BCAAA4; }"
    "QGroupBox { border: 2px solid #BCAAA4; border-radius: 8px; margin-top: 10px;"
    "  font-weight: bold; color: #291C0E; padding: 10px; }"
    "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 5px; }";

MetierDialog::MetierDialog(QWidget *parent, DialogMode mode)
    : QDialog(parent)
{
    buildUI(mode);
    setStyleSheet(DIALOG_STYLE);
    setMinimumWidth(480);
}

void MetierDialog::buildUI(DialogMode mode)
{
    setWindowTitle(mode == AddMode ? "Ajouter un Métier" : "Modifier le Métier");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(16);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    // ── Title ──────────────────────────────────────────────────────────────
    auto *titleLabel = new QLabel(mode == AddMode ? "➕ Nouveau Métier" : "✏️ Modifier Métier");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #8D6E63;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // ── Form ───────────────────────────────────────────────────────────────
    auto *group = new QGroupBox("Informations du Métier");
    auto *form  = new QFormLayout(group);
    form->setSpacing(12);
    form->setLabelAlignment(Qt::AlignRight);

    m_code = new QLineEdit();
    m_code->setPlaceholderText("ex: CORD-001");
    m_code->setMaxLength(20);
    form->addRow("Code *", m_code);

    m_libelle = new QLineEdit();
    m_libelle->setPlaceholderText("ex: Cordonnier");
    m_libelle->setMaxLength(100);
    form->addRow("Libellé *", m_libelle);

    m_domaine = new QComboBox();
    m_domaine->addItems({
        "Maroquinerie", "Cordonnerie", "Sellerie", "Tannerie",
        "Couture", "Découpe", "Finition", "Contrôle Qualité",
        "Maintenance", "Logistique", "Administration", "Autre"
    });
    form->addRow("Domaine *", m_domaine);

    m_niveau = new QComboBox();
    m_niveau->addItems({
        "CAP", "BEP", "Bac Pro", "BTS", "Licence", "Master", "Expert"
    });
    form->addRow("Niveau de Qualification *", m_niveau);

    m_salaireBase = new QDoubleSpinBox();
    m_salaireBase->setRange(0.0, 99999.99);
    m_salaireBase->setDecimals(2);
    m_salaireBase->setSuffix(" TND");
    m_salaireBase->setValue(800.0);
    form->addRow("Salaire de Base", m_salaireBase);

    m_description = new QTextEdit();
    m_description->setPlaceholderText("Description du métier, compétences requises...");
    m_description->setMaximumHeight(80);
    form->addRow("Description", m_description);

    m_actif = new QCheckBox("Métier actif");
    m_actif->setChecked(true);
    m_actif->setStyleSheet("QCheckBox { font-weight: normal; }");
    form->addRow("", m_actif);

    mainLayout->addWidget(group);

    // ── Error label ────────────────────────────────────────────────────────
    m_errorLabel = new QLabel();
    m_errorLabel->setStyleSheet("color: #C62828; font-size: 11px; font-weight: normal;");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->setVisible(false);
    mainLayout->addWidget(m_errorLabel);

    // ── Buttons ────────────────────────────────────────────────────────────
    auto *btnBox = new QDialogButtonBox();
    auto *btnOk  = btnBox->addButton(
        mode == AddMode ? "Ajouter" : "Enregistrer",
        QDialogButtonBox::AcceptRole
    );
    auto *btnCancel = btnBox->addButton("Annuler", QDialogButtonBox::RejectRole);
    btnCancel->setProperty("flat", true);

    connect(btnOk,     &QPushButton::clicked, this, &MetierDialog::onValidate);
    connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);

    mainLayout->addWidget(btnBox);
}

void MetierDialog::onValidate()
{
    if (!validateInputs()) return;
    accept();
}

bool MetierDialog::validateInputs()
{
    if (m_code->text().trimmed().isEmpty()) {
        m_errorLabel->setText("⚠️ Le code est obligatoire.");
        m_errorLabel->setVisible(true);
        m_code->setFocus();
        return false;
    }
    if (m_libelle->text().trimmed().isEmpty()) {
        m_errorLabel->setText("⚠️ Le libellé est obligatoire.");
        m_errorLabel->setVisible(true);
        m_libelle->setFocus();
        return false;
    }
    m_errorLabel->setVisible(false);
    return true;
}

void MetierDialog::setMetierData(int id, const QString &code, const QString &libelle,
                                  const QString &description, const QString &domaine,
                                  const QString &niveau, double salaireBase, bool actif)
{
    m_id = id;
    m_code->setText(code);
    m_libelle->setText(libelle);
    m_description->setPlainText(description);

    int domIdx = m_domaine->findText(domaine);
    if (domIdx >= 0) m_domaine->setCurrentIndex(domIdx);
    else { m_domaine->addItem(domaine); m_domaine->setCurrentText(domaine); }

    int nivIdx = m_niveau->findText(niveau);
    if (nivIdx >= 0) m_niveau->setCurrentIndex(nivIdx);
    else { m_niveau->addItem(niveau); m_niveau->setCurrentText(niveau); }

    m_salaireBase->setValue(salaireBase);
    m_actif->setChecked(actif);
}

int     MetierDialog::getId()                const { return m_id; }
QString MetierDialog::getCode()              const { return m_code->text().trimmed().toUpper(); }
QString MetierDialog::getLibelle()           const { return m_libelle->text().trimmed(); }
QString MetierDialog::getDescription()       const { return m_description->toPlainText().trimmed(); }
QString MetierDialog::getDomaine()           const { return m_domaine->currentText(); }
QString MetierDialog::getNiveauQualification() const { return m_niveau->currentText(); }
double  MetierDialog::getSalaireBase()       const { return m_salaireBase->value(); }
bool    MetierDialog::isActif()              const { return m_actif->isChecked(); }
