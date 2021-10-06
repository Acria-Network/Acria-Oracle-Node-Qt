QT       += core gui
QT       += network
QT       += webkit webenginewidgets
QT += webchannel

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    abouttransactionwindow.cpp \
    accountmanager.cpp \
    acriaconfig.cpp \
    availableaccounts.cpp \
    availabledatacontracts.cpp \
    balances.cpp \
    bignum256.cpp \
    compinfo.cpp \
    config.cpp \
    configitem.cpp \
    data.cpp \
    deploywindow.cpp \
    ecdsa.cpp \
    endian.cpp \
    enterpassworddialog.cpp \
    entersinglepassworddialog.cpp \
    ethbasedchain.cpp \
    gasprice.cpp \
    hash.cpp \
    hmac_drbg.cpp \
    infogeth.cpp \
    keccak256.cpp \
    logwindow.cpp \
    main.cpp \
    mainwindow.cpp \
    node.cpp \
    noncemanager.cpp \
    processingwindow.cpp \
    qprogressindicator.cpp \
    reportactive.cpp \
    resource.cpp \
    rlp.cpp \
    scripteditor.cpp \
    sha256.cpp \
    sha3.cpp \
    signconfigwindow.cpp \
    signtransaction.cpp \
    tasks.cpp \
    uint256.cpp \
    verifyethaddress.cpp \
    withdraw.cpp

HEADERS += \
    abouttransactionwindow.h \
    accountmanager.h \
    acriaconfig.h \
    availableaccounts.h \
    availabledatacontracts.h \
    balances.h \
    bignum256.h \
    common.h \
    compinfo.h \
    config.h \
    configitem.h \
    data.h \
    deploywindow.h \
    ecdsa.h \
    endian.h \
    enterpassworddialog.h \
    entersinglepassworddialog.h \
    ethbasedchain.h \
    gasprice.h \
    hash.h \
    hmac_drbg.h \
    infogeth.h \
    keccak256.h \
    logwindow.h \
    mainwindow.h \
    node.h \
    noncemanager.h \
    processingwindow.h \
    qprogressindicator.h \
    reportactive.h \
    resource.h \
    rlp.h \
    scripteditor.h \
    sha256.h \
    sha3.h \
    signconfigwindow.h \
    signtransaction.h \
    span.h \
    tasks.h \
    uint256.h \
    util.h \
    verifyethaddress.h \
    withdraw.h

FORMS += \
    abouttransactionwindow.ui \
    accountmanager.ui \
    acriaconfig.ui \
    availableaccounts.ui \
    availabledatacontracts.ui \
    configitem.ui \
    deploywindow.ui \
    enterpassworddialog.ui \
    entersinglepassworddialog.ui \
    infogeth.ui \
    logwindow.ui \
    mainwindow.ui \
    processingwindow.ui \
    scripteditor.ui \
    signconfigwindow.ui

TRANSLATIONS += \
    Acria-Oracle-Node-Qt_de_DE.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
