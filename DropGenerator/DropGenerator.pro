#-------------------------------------------------
#
# Project created by QtCreator 2010-04-08T10:03:50
#
#-------------------------------------------------

QT       -= core gui

TARGET = DropGenerator
CONFIG   += console
CONFIG   -= app_bundle
INCLUDEPATH += /usr/include/mysql ../SOL ../SQL
LIBS += -lmysqlclient \
    -lmysqlpp
LIBS += -L../SOL -lSOL -L../SQL -lSQL
TEMPLATE = app


SOURCES += main.cpp
