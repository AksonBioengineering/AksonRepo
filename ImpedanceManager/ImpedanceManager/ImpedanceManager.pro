#-------------------------------------------------
#
# Project created by QtCreator 2015-05-12T18:46:48
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

RC_FILE = app.rc
TARGET = ImpedanceManager
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    csettingsdialog.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    csettingsdialog.h

FORMS    += mainwindow.ui \
    csettingsdialog.ui

RESOURCES += \
    Resources.qrc
