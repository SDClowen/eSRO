# -------------------------------------------------
# Project created by QtCreator 2010-06-14T10:48:21
# -------------------------------------------------
QT += opengl
TARGET = eView
TEMPLATE = app
INCLUDEPATH += ../SOL/include ../khamaileon/include
LIBS += -lboost_thread-mt \
	-L../SOL -lSOL -L../khamaileon -lkhamaileon
PRE_TARGETDEPS += ../khamaileon/libkhamaileon.a
SOURCES += main.cpp \
    mainwindow.cpp \
    glworldwidget.cpp \
    zone.cpp \
    map_manager.cpp
HEADERS += mainwindow.hpp \
    glworldwidget.hpp \
    zone.hpp \
    map_manager.hpp
FORMS += mainwindow.ui
