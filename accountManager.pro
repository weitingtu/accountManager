#-------------------------------------------------
#
# Project created by QtCreator 2014-08-19T22:09:02
#
#-------------------------------------------------

QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = untitled
TEMPLATE = app

QMAKE_CXXFLAGS += -Wall

SOURCES += main.cpp\
        wttmainwindow.cpp \
    wttview.cpp

HEADERS  += wttmainwindow.h \
    wttview.h

CONFIG += mobility

MOBILITY =

