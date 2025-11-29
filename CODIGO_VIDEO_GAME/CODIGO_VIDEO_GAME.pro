QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


QT += widgets multimedia
CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    Level1Widget.cpp \
    Level2Widget.cpp \
    Level3Widget.cpp \
    Jugador_R_L.cpp \
    Jugador_U_D_Diagonals.cpp \
    Enemigos.cpp \
    Interacciones.cpp \
    Media.cpp \
    Objeto_Interactivo.cpp

HEADERS += \
    mainwindow.h \
    Level1Widget.h \
    Level2Widget.h \
    Level3Widget.h \
    Jugador_R_L.h \
    Jugador_U_D_Diagonals.h \
    Enemigos.h \
    Interacciones.h \
    Media.h \
    Objeto_Interactivo.h

FORMS += mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
