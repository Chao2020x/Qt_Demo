#-------------------------------------------------
#
# Project created by QtCreator 2018-11-11T20:35:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BPNNDemo
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/bpnetwork.cpp \
    src/classifiertype.cpp \
    src/classifiercolor.cpp \
    src/classifierselector.cpp \
    src/classifierselectorlist.cpp \
    src/classifiercontroller.cpp \
    src/surface.cpp \
    src/surfacenode.cpp \
    src/bpnetworkqt.cpp \
    src/trainthread.cpp \
    src/hlncntselector.cpp \
    src/paintthread.cpp

HEADERS += \
    inc/mainwindow.h \
    inc/bpnetwork.h \
    inc/classifierselector.h \
    inc/classifiertype.h \
    inc/classifiercolor.h \
    inc/classifierselectorlist.h \
    inc/classifiercontroller.h \
    inc/hlncntselector.h \
    inc/surface.h \
    inc/surfacenode.h \
    inc/bpnetworkqt.h \
    inc/trainthread.h \
    inc/paintthread.h

FORMS += \
        mainwindow.ui \
    classifierselector.ui \
    classifiercontroller.ui \
    hlncntselector.ui \
    surface.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RC_ICONS = icon.ico

