# -------------------------------------------------
# Project created by QtCreator 2010-01-20T09:26:44
# -------------------------------------------------
TARGET = ServerList
TEMPLATE = app
INCLUDEPATH += ../PKFM \
    ../Blowfish \
    ../SRNL
LIBS += -L../PKFM \
    -lPKFM
LIBS += -L../Blowfish \
    -lBlowfish
LIBS += -L../SRNL \
    -lSRNL
LIBS += -lboost_system-mt \
    -lboost_thread-mt
SOURCES += main.cpp \
    serverwindow.cpp \
    networkthread.cpp \
    client_state_version.cpp \
    client_state_handshake.cpp \
    packet/client/packet_login.cpp \
    srvlistclient.cpp \
    client_state_login.cpp
HEADERS += serverwindow.h \
    networkthread.h \
    client_state_version.hpp \
    client_state_login.hpp \
    client_state_handshake.hpp \
    packet/client/packet_login.hpp \
    srvlistclient.h
FORMS += serverwindow.ui
QMAKE_CXXFLAGS += -std=c++0x
