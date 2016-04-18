#-------------------------------------------------
#
# Project created by QtCreator 2016-04-06T11:43:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = robogen-gui
TEMPLATE = app

!win32 {
    QMAKE_CXXFLAGS += -std=c++11
}

SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    dirwatcher.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    dirwatcher.h

FORMS    += mainwindow.ui

RESOURCES += \
    res.qrc

DISTFILES += \
    TODO
