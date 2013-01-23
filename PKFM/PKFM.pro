# -------------------------------------------------
# Project created by QtCreator 2010-01-14T11:06:03
# -------------------------------------------------
QT -= core \
    gui
TARGET = PKFM
TEMPLATE = lib
CONFIG += staticlib
INCLUDEPATH += ../Blowfish
SOURCES += pk2Reader.cpp
HEADERS += pk2Reader.h
QMAKE_CXXFLAGS+=-std=c++0x
LIBS += -L ../Blowfish -lBlowfish
