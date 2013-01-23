QT -= core \
    gui
TARGET = MasterServer
INCLUDEPATH = ../Blowfish/include \
    ../SRNL/include \
    /usr/include/mysql
LIBS += -L../SRNL \
    -lSRNL
LIBS += -L../Blowfish \
    -lBlowfish \
    -lmysqlclient \
    -lmysqlpp \
    -lboost_system-mt \
    -lboost_thread-mt
HEADERS += state_shard.hpp \
    state_handshake.hpp \
    state_gateway.hpp \
    state_emblem.hpp \
    state_download.hpp \
    state_agent.hpp \
    server.hpp \
    opcodes_client.hpp \
    connection.hpp \
    query/manager_query.hpp \
    query/account_query.hpp \
    packet/opcodes_manager_gateway_client.hpp \
    packet/packet_manager_shard.hpp \
    packet/packet_manager_gateway.hpp \
    packet/opcodes_manager_gateway_server.hpp
SOURCES += state_shard.cpp \
    state_handshake.cpp \
    state_gateway.cpp \
    state_emblem.cpp \
    state_download.cpp \
    state_agent.cpp \
    server.cpp \
    main.cpp \
    connection.cpp \
    query/manager_query.cpp \
    query/account_query.cpp \
    packet/packet_manager_shard.cpp \
    packet/packet_manager_gateway.cpp
debug:DEFINES += DEBUG \
    GLIBCXX_DEBUG
OTHER_FILES += CMakeLists.txt \
    packet/CMakeLists.txt \
    query/CMakeLists.txt
