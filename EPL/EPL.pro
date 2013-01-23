# -------------------------------------------------
# Project created by QtCreator 2010-04-16T17:46:53
# -------------------------------------------------
QT -= core \
    gui
TARGET = EPL
TEMPLATE = lib
CONFIG += staticlib
INCLUDEPATH += include \
    ../Blowfish/include \
    ../SRNL/include \
    ../SOL/include
LIBS += -L../SRNL \
    -lSRNL \
    -L../Blowfish \
    -lBlowfish \
    -L../SOL \
    -lSOL \
    -lboost_thread
debug:DEFINES += GLIBCXX_DEBUG
HEADERS += include/packet_uniques.hpp \
    include/packet_tutorial.hpp \
    include/packet_talk.hpp \
    include/packet_storage.hpp \
    include/packet_spawn.hpp \
    include/packet_msg.hpp \
    include/packet_academy.hpp \
    include/packet_message.hpp \
    include/packet_mastery.hpp \
    include/packet_invitation.hpp \
    include/packet_friend.hpp \
    include/packet_fortress.hpp \
    include/packet_exchange.hpp \
    include/packet_block.hpp \
    include/packet_base_item.hpp \
    include/packet_shard.hpp \
    include/packet_login.hpp \
    include/packet_attack.hpp \
    include/opcodes_shard_client.hpp \
    include/packet_npc.hpp \
    include/packet_transport.hpp \
    include/packet_item.hpp \
    include/packet_party.hpp \
    include/packet_stall.hpp \
    include/packet_alliance.hpp \
    include/packet_player.hpp \
    include/packet_skill.hpp \
    include/packet_lobby.hpp \
    include/packet_misc.hpp \
    include/guild_error.hpp \
    include/packet_teleport.hpp \
    include/guild_notify.hpp \
    include/packet_guild.hpp \
    include/opcodes_shard_server.hpp \
    include/packet_job.hpp \
    include/packet_error_global.hpp \
    include/packet_error_skill_cast.hpp
SOURCES += src/packet_uniques.cpp \
    src/packet_tutorial.cpp \
    src/packet_talk.cpp \
    src/packet_storage.cpp \
    src/packet_msg.cpp \
    src/packet_shard.cpp \
    src/packet_academy.cpp \
    src/packet_message.cpp \
    src/packet_mastery.cpp \
    src/packet_login.cpp \
    src/packet_item.cpp \
    src/packet_block.cpp \
    src/packet_attack.cpp \
    src/packet_fortress.cpp \
    src/packet_base_item.cpp \
    src/packet_teleport.cpp \
    src/packet_invitation.cpp \
    src/packet_alliance.cpp \
    src/packet_exchange.cpp \
    src/packet_player.cpp \
    src/packet_stall.cpp \
    src/packet_skill.cpp \
    src/packet_party.cpp \
    src/packet_lobby.cpp \
    src/packet_friend.cpp \
    src/packet_guild.cpp \
    src/packet_transport.cpp \
    src/packet_misc.cpp \
    src/packet_spawn.cpp \
    src/packet_npc.cpp \
    src/packet_job.cpp
