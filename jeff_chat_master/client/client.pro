#-------------------------------------------------
#
# Project created by QtCreator 2017-10-31T08:17:40
#
#-------------------------------------------------

QT       += core gui
QT       += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    login.cpp \
    logindialog.cpp \
    filesender.cpp \
    filereceiver.cpp \
    forgetpswd.cpp

HEADERS  += mainwindow.h \
    login.h \
    logindialog.h \
    filesender.h \
    filereceiver.h \
    forgetpswd.h

FORMS    += mainwindow.ui \
    login.ui \
    logindialog.ui \
    filesender.ui \
    filereceiver.ui \
    forgetpswd.ui
