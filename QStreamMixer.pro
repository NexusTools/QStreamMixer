#-------------------------------------------------
#
# Project created by QtCreator 2014-05-16T15:45:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
TARGET = QStreamMixer
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    avconv.cpp

HEADERS  += mainwindow.h \
    avconv.h

FORMS    += mainwindow.ui
