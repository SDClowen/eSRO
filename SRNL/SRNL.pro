# -------------------------------------------------
# Project created by QtCreator 2010-01-22T17:50:44
# -------------------------------------------------
QT -= core \
    gui
TARGET = SRNL
TEMPLATE = lib
CONFIG += staticlib
INCLUDEPATH += include ../Blowfish/include
debug:DEFINES += DEBUG \
    GLIBCXX_DEBUG
HEADERS += include/data.hpp \
    include/opcodes_gateway_client.hpp \
    include/opcodes_gateway_server.hpp \
    include/packet_util_functions.hpp \
    include/opcodes_global_client.hpp \
    include/opcodes_manager_gateway_client.hpp \
    include/server_state_handshake_interface.hpp \
    include/client_state_handshake_interface.hpp \
    include/host_info.hpp \
    include/news.hpp \
    include/shard.hpp \
    include/security.hpp \
    include/client_connection_state.hpp \
    include/packet_group.hpp \
    include/sro_version.hpp \
    include/server_connection_state.hpp \
    include/opcodes_global_server.hpp \
    include/opcodes_manager_gateway_server.hpp \
    include/packet_authentication.hpp \
    include/client_connection_interface.hpp \
    include/server_connection_interface.hpp \
    include/packet.hpp \
    include/server_interface.hpp
OTHER_FILES += src/CMakeLists.txt \
    CMakeLists.txt
SOURCES += src/packet_util_functions.cpp \
    src/security.cpp \
    src/client_connection_state.cpp \
    src/server_connection_state.cpp \
    src/packet_authentication.cpp \
    src/server_state_handshake_interface.cpp \
    src/client_connection_interface.cpp \
    src/client_state_handshake_interface.cpp \
    src/packet.cpp \
    src/server_interface.cpp \
    src/server_connection_interface.cpp
