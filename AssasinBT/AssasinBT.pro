# -------------------------------------------------
# Project created by QtCreator 2010-01-14T11:01:50
# -------------------------------------------------
QT += xml
TARGET = AssasinBT
TEMPLATE = app
INCLUDEPATH += ../PKFM \
    ../Blowfish \
    ../SRNL
SOURCES += main.cpp \
    mainwindow.cpp \
    dlgpreference.cpp \
    srvlistclient.cpp \
    client_state_version.cpp \
    client_state_handshake.cpp \
    packet/client/packet_login.cpp
HEADERS += mainwindow.h \
    dlgpreference.h \
    srvlistclient.h \
    client_state_version.hpp \
    client_state_handshake.hpp \
    client_state_login.hpp \
    packet/client/packet_login.hpp
FORMS += mainwindow.ui \
    dlgpreference.ui
LIBS += -L../PKFM \
    -lPKFM
LIBS += -L../Blowfish \
    -lBlowfish
LIBS += -L../SRNL \
    -lSRNL
LIBS += -lboost_system-mt \
    -lboost_thread-mt
QMAKE_CXXFLAGS += -std=c++0x
