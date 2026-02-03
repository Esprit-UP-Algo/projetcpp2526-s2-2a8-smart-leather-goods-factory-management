#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDebug>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile file("C:/Users/USER/Desktop/cpp/test2/style1.qss");
    if(file.open(QFile::ReadOnly)) {
        QString style = file.readAll();
        a.setStyleSheet(style);
        file.close();
    } else {
        qWarning() << "Cannot open QSS file!";
    }

    MainWindow w;
    w.show();

    return a.exec();
}
