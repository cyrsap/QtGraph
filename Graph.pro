#-------------------------------------------------
#
# Project created by QtCreator 2016-09-10T11:11:51
#
#-------------------------------------------------

QT       += core gui testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Graph
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    globaldefines.cpp \
    T_Calculator.cpp \
    t_valuesframe.cpp

HEADERS  += mainwindow.h \
    globaldefines.h \
    T_Calculator.h \
    t_valuesframe.h

FORMS    += mainwindow.ui \
    t_valuesframe.ui
