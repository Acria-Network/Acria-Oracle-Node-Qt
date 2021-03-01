QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    acriaconfig.cpp \
    compinfo.cpp \
    config.cpp \
    configitem.cpp \
    data.cpp \
    infogeth.cpp \
    main.cpp \
    mainwindow.cpp \
    node.cpp \
    qprogressindicator.cpp \
    resource.cpp \
    tasks.cpp \
    uint256.cpp

HEADERS += \
    acriaconfig.h \
    compinfo.h \
    config.h \
    configitem.h \
    data.h \
    infogeth.h \
    mainwindow.h \
    node.h \
    qprogressindicator.h \
    resource.h \
    span.h \
    tasks.h \
    uint256.h \
    util.h

FORMS += \
    acriaconfig.ui \
    configitem.ui \
    infogeth.ui \
    mainwindow.ui

TRANSLATIONS += \
    Acria-Oracle-Node-Qt_de_DE.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
