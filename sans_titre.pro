QT       += core gui charts printsupport sql network multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# Windows Speech API (SAPI) — utilisé par mainwindow.cpp pour les commandes vocales
win32: LIBS += -lsapi -lole32 -loleaut32

TARGET = CUIREA_Management
TEMPLATE = app

SOURCES += \
    aichatwidget.cpp \
    bilandialog.cpp \
    client.cpp \
    connection.cpp \
    email.cpp \
    employe.cpp \
    main.cpp \
    mainwindow.cpp \
    employeedialog.cpp \
    clientmanagerdialog.cpp \
    matiere.cpp \
    matieredialog.cpp \
    fournisseur.cpp \
    fournisseurdialog.cpp \
    smsfournisseurdialog.cpp \
    qrfournisseurdialog.cpp \
    production.cpp \
    productionview.cpp \
    article.cpp \
    articledialog.cpp \
    logindialog.cpp \
    notificationwidget.cpp

HEADERS += \
    aichatwidget.h \
    bilandialog.h \
    email.h \
    envloader.h \
    connection.h \
    employe.h \
    mainwindow.h \
    employeedialog.h \
    client.h \
    clientmanagerdialog.h \
    matiere.h \
    matieredialog.h \
    fournisseur.h \
    fournisseurdialog.h \
    smsfournisseurdialog.h \
    qrfournisseurdialog.h \
    production.h \
    productionview.h \
    article.h \
    articledialog.h \
    logindialog.h \
    notificationwidget.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources.qrc

# Default rules for deployment
qnx: target.path = /tmp/${TARGET}/bin
else: unix:!android: target.path = /opt/${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
