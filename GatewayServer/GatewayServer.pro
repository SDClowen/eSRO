QT -= core \
    gui
TARGET = GatewayServer
INCLUDEPATH = ../Blowfish/include \
    ../SRNL/include
LIBS += -L../SRNL \
    -lSRNL
LIBS += -L../Blowfish \
    -lBlowfish
LIBS += -lboost_system-mt \
    -lboost_thread-mt
HEADERS += state_master.hpp \
    state_handshake.hpp \
    server_state_version.hpp \
    server_state_login.hpp \
    server_state_handshake.hpp \
    server_connection.hpp \
    server.hpp \
    opcodes_client.hpp \
    client_connection.hpp \
    packet/packet_gateway.hpp \
    packet/opcodes_gateway_server.hpp \
    packet/packet_information.hpp \
    packet/opcodes_gateway_client.hpp
SOURCES += state_master.cpp \
    state_handshake.cpp \
    server_state_version.cpp \
    server_state_login.cpp \
    server_state_handshake.cpp \
    server_connection.cpp \
    server.cpp \
    main.cpp \
    client_connection.cpp \
    packet/packet_gateway.cpp \
    packet/packet_information.cpp
debug:DEFINES += DEBUG \
    GLIBCXX_DEBUG
OTHER_FILES += CMakeLists.txt \
    packet/CMakeLists.txt
