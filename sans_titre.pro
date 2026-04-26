QT       += core gui charts printsupport sql network multimedia serialport
QT += charts
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++17

# Windows Speech API (SAPI) — utilisé par mainwindow.cpp pour les commandes vocales
win32: LIBS += -lsapi -lole32 -loleaut32

TARGET = CUIREA_Management
TEMPLATE = app

SOURCES += \
    arduino.cpp \
    arduinoconnection.cpp \
    aichatwidget.cpp \
    rfidoutput.cpp \
    articleviewer3d.cpp \
    bilandialog.cpp \
    client.cpp \
    connection.cpp \
    email.cpp \
    employe.cpp \
    fichepaie.cpp \
    statsrh.cpp \
    usersession.cpp \
    main.cpp \
    mainwindow.cpp \
    employeedialog.cpp \
    clientmanagerdialog.cpp \
    map.cpp \
    matiere.cpp \
    matieredialog.cpp \
    matieredetection.cpp \
    statscharts.cpp \
    voicematieres.cpp \
    fournisseur.cpp \
    fournisseurdialog.cpp \
    smsfournisseurdialog.cpp \
    qrfournisseurdialog.cpp \
    production.cpp \
    productionview.cpp \
    article.cpp \
    articledialog.cpp \
    tripo3dgenerator.cpp \
    logindialog.cpp \
    notification.cpp \
    pointage.cpp \
    pointagedialog.cpp

HEADERS += \
    arduino.h \
    arduinoconnection.h \
    aichatwidget.h \
    rfidoutput.h \
    articleviewer3d.h \
    bilandialog.h \
    email.h \
    envloader.h \
    connection.h \
    employe.h \
    usersession.h \
    fichepaie.h \
    statsrh.h \
    mainwindow.h \
    employeedialog.h \
    client.h \
    clientmanagerdialog.h \
    map.h \
    matiere.h \
    matieredialog.h \
    matieredetection.h \
    statscharts.h \
    voicematieres.h \
    fournisseur.h \
    fournisseurdialog.h \
    smsfournisseurdialog.h \
    qrfournisseurdialog.h \
    production.h \
    productionview.h \
    article.h \
    articledialog.h \
    tripo3dgenerator.h \
    logindialog.h \
    notification.h \
    pointage.h \
    pointagedialog.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

# Default rules for deployment
qnx: target.path = /tmp/${TARGET}/bin
else: unix:!android: target.path = /opt/${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
