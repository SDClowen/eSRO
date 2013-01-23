# -------------------------------------------------
# Project created by QtCreator 2010-04-08T13:17:21
# -------------------------------------------------
QT -= core \
    gui
TARGET = SQL
TEMPLATE = lib
CONFIG += staticlib
INCLUDEPATH += /usr/include/mysql \
    ../SOL/include
SOURCES += property_query.cpp
HEADERS += property_query.hpp
debug:DEFINES += GLIBCXX_DEBUG
OTHER_FILES += CMakeLists.txt
