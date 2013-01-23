QT -= core \
    gui
TARGET = AgentServer
INCLUDEPATH = handler \
    manager \
    packet \
    query \
    ../Blowfish/include \
    ../SRNL/include \
    ../SOL/include \
    ../SQL \
    ../EPL/include \
    ../khamaileon/include \
    /usr/include/mysql
LIBS += -L../SRNL \
    -lSRNL \
    -L../khamaileon \
    -lkhamaileon \
    -L../SOL \
    -lSOL \
    -L../SQL \
    -lSQL \
    -L../Blowfish \
    -lBlowfish \
    -L../EPL \
    -lEPL \
    -lmysqlclient \
    -lmysqlpp \
    -lboost_system-mt \
    -lboost_thread-mt
PRE_TARGETDEPS += ../khamaileon/libkhamaileon.a \
    ../SOL/libSOL.a \
    ../SRNL/libSRNL.a \
    ../EPL/libEPL.a \
    ../SQL/libSQL.a
debug:DEFINES += DEBUG \
    GLIBCXX_DEBUG
debug:QMAKE_CXX_FLAGS = -Wall
HEADERS += zone.hpp \
    world.hpp \
    task_background.hpp \
    state_master.hpp \
    state_handshake.hpp \
    stallnetwork.hpp \
    server_state_login.hpp \
    server_state_lobby.hpp \
    server_state_handshake.hpp \
    server_state_game.hpp \
    server_connection.hpp \
    server_channel.hpp \
    server.hpp \
    opcodes_client.hpp \
    near_objects.hpp \
    login_request.hpp \
    formulas.hpp \
    emote_actions.hpp \
    drop.hpp \
    client_connection.hpp \
    broadcast.hpp \
    manager/weather_manager.hpp \
    manager/union_manager.hpp \
    manager/trade_manager.hpp \
    manager/stall_manager.hpp \
    manager/skill_manager.hpp \
    manager/script_manager.hpp \
    manager/resource_manager.hpp \
    manager/party_manager.hpp \
    manager/map_manager.hpp \
    manager/guild_manager.hpp \
    manager/fortress_manager.hpp \
    manager/event_manager.hpp \
    manager/academy_manager.hpp \
    query/skill_query.hpp \
    query/shop_query.hpp \
    query/player_query.hpp \
    query/message_query.hpp \
    query/mastery_query.hpp \
    query/manager_query.hpp \
    query/job_query.hpp \
    query/item_query.hpp \
    query/ip_query.hpp \
    query/hotkeys_query.hpp \
    query/guild_query.hpp \
    query/guild_member_query.hpp \
    query/friend_query.hpp \
    query/drop_query.hpp \
    query/connection.hpp \
    query/block_query.hpp \
    query/alliance_query.hpp \
    query/academy_query.hpp \
    manager/friend_manager.hpp
SOURCES += zone.cpp \
    world.cpp \
    state_master.cpp \
    state_handshake.cpp \
    server_state_login.cpp \
    server_state_lobby.cpp \
    server_state_handshake.cpp \
    server_state_game.cpp \
    server_connection.cpp \
    server_channel.cpp \
    server.cpp \
    main.cpp \
    formulas.cpp \
    client_connection.cpp \
    handler/handler_union.cpp \
    handler/handler_transport.cpp \
    handler/handler_teleport.cpp \
    handler/handler_talk.cpp \
    handler/handler_storage.cpp \
    handler/handler_stats.cpp \
    handler/handler_stall.cpp \
    handler/handler_skill.cpp \
    handler/handler_settings.cpp \
    handler/handler_quest_event.cpp \
    handler/handler_player.cpp \
    handler/handler_party.cpp \
    handler/handler_offline_msg.cpp \
    handler/handler_msg.cpp \
    handler/handler_logout.cpp \
    handler/handler_login.cpp \
    handler/handler_job.cpp \
    handler/handler_item.cpp \
    handler/handler_invite.cpp \
    handler/handler_guild.cpp \
    handler/handler_gm.cpp \
    handler/handler_friend.cpp \
    handler/handler_fortress.cpp \
    handler/handler_exchange.cpp \
    handler/handler_block.cpp \
    handler/handler_alchemy.cpp \
    handler/handler_academy.cpp \
    manager/weather_manager.cpp \
    manager/union_manager.cpp \
    manager/trade_manager.cpp \
    manager/stall_manager.cpp \
    manager/skill_manager.cpp \
    manager/script_manager.cpp \
    manager/resource_manager.cpp \
    manager/party_manager.cpp \
    manager/map_manager.cpp \
    manager/guild_manager.cpp \
    manager/fortress_manager.cpp \
    manager/event_manager.cpp \
    manager/academy_manager.cpp \
    query/skill_query.cpp \
    query/shop_query.cpp \
    query/player_query.cpp \
    query/message_query.cpp \
    query/mastery_query.cpp \
    query/manager_query.cpp \
    query/job_query.cpp \
    query/item_query.cpp \
    query/ip_query.cpp \
    query/hotkeys_query.cpp \
    query/guild_query.cpp \
    query/guild_member_query.cpp \
    query/friend_query.cpp \
    query/drop_query.cpp \
    query/connection.cpp \
    query/block_query.cpp \
    query/alliance_query.cpp \
    manager/friend_manager.cpp
OTHER_FILES += query/CMakeLists.txt \
    CMakeLists.txt
