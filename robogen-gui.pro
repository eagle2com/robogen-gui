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
    dirwatcher.cpp \
    evolutionconfigform.cpp \
    simulationconfigform.cpp \
    robotconfigform.cpp \
    configoverviewform.cpp \
    settingswindow.cpp \
    configurations.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    dirwatcher.h \
    evolutionconfigform.h \
    simulationconfigform.h \
    robotconfigform.h \
    configoverviewform.h \
    settingswindow.h \
    configurations.h

FORMS    += mainwindow.ui \
    testwindow.ui \
    evolutionconfigform.ui \
    simulationconfigform.ui \
    robotconfigform.ui \
    configoverviewform.ui \
    settingswindow.ui

RESOURCES += \
    res.qrc

DISTFILES += \
    TODO
