#ifndef EXPORTDIALOG_H
#define EXPORTDIALOG_H

#include <QDialog>
#include <QRadioButton>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>

class ExportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportDialog(QWidget *parent = nullptr);
    ~ExportDialog();

private:
    void setupUI();
    void applyStyles();

    QRadioButton *csvRadio;
    QRadioButton *htmlRadio;
    QRadioButton *txtRadio;
    QButtonGroup *formatGroup;
    QLineEdit *fileNameEdit;
    QLineEdit *destinationEdit;
    QPushButton *browseButton;
    QPushButton *exportButton;
    QPushButton *cancelButton;
};

#endif
