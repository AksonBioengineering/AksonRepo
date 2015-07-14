#-------------------------------------------------
#
# Project created by QtCreator 2015-05-12T18:46:48
#
#-------------------------------------------------

QT       += core gui serialport
QT       += concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

RC_FILE = app.rc
TARGET = ImpedanceManager
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++0x

SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    csettingsdialog.cpp \
    cgenericproject.cpp \
    ceisproject.cpp \
    cnewprojectdialog.cpp \
    csettingsmanager.cpp \
    cserialthread.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    csettingsdialog.h \
    cgenericproject.h \
    ceisproject.h \
    cnewprojectdialog.h \
    MeasureUtility.h \
    csettingsmanager.h \
    cserialthread.h

FORMS    += mainwindow.ui \
    csettingsdialog.ui \
    cgenericproject.ui \
    cnewprojectdialog.ui

RESOURCES += \
    Resources.qrc
