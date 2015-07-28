#-------------------------------------------------
#
# Project created by QtCreator 2015-07-28T18:06:44
#
#-------------------------------------------------

QT       += core gui serialport
QT       += concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SerialMonitor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
