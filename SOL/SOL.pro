# -------------------------------------------------
# Project created by QtCreator 2010-03-31T06:13:28
# -------------------------------------------------
QT -= core \
    gui
TARGET = SOL
TEMPLATE = lib
CONFIG += staticlib
INCLUDEPATH += include
HEADERS += include/utility_function.hpp \
    include/fortress.hpp \
    include/friend.hpp \
    include/skill_area.hpp \
    include/object.hpp \
    include/exchange_window.hpp \
    include/academy.hpp \
    include/quest.hpp \
    include/party_entry.hpp \
    include/hotkey.hpp \
    include/mastery_tree.hpp \
    include/job.hpp \
    include/invitation_type.hpp \
    include/guild_constants.hpp \
    include/skill_requirement.hpp \
    include/npc_list.hpp \
    include/guild_member.hpp \
    include/entity_constants.hpp \
    include/autopot.hpp \
    include/magicoption_property.hpp \
    include/npc_task.hpp \
    include/npc_util.hpp \
    include/item_factory.hpp \
    include/skill_imbue.hpp \
    include/mastery.hpp \
    include/player_property.hpp \
    include/spawnobject.hpp \
    include/skill.hpp \
    include/skill_status.hpp \
    include/skill_builder.hpp \
    include/entity_state.hpp \
    include/coord.hpp \
    include/storage.hpp \
    include/storage_constants.hpp \
    include/skill_attack.hpp \
    include/item.hpp \
    include/stall.hpp \
    include/union.hpp \
    include/item_types.hpp \
    include/dropable_item.hpp \
    include/guild.hpp \
    include/teleport_building.hpp \
    include/skill_buff.hpp \
    include/market_stock.hpp \
    include/party.hpp \
    include/level_property.hpp \
    include/spawn_point.hpp \
    include/building_property.hpp \
    include/shoptab.hpp \
    include/shopgroup.hpp \
    include/shopgood.hpp \
    include/scrapofpackageitem.hpp \
    include/itemprice.hpp \
    include/packageitem.hpp \
    include/entity_property.hpp \
    include/skill_property.hpp \
    include/transport.hpp \
    include/player.hpp \
    include/npc.hpp \
    include/types.hpp \
    include/mob.hpp \
    include/item_property.hpp \
    include/interactive_npc.hpp \
    include/character_factory.hpp \
    include/fortress_property.hpp
SOURCES += src/skill_status.cpp \
    src/object.cpp \
    src/academy.cpp \
    src/party_entry.cpp \
    src/job.cpp \
    src/mastery.cpp \
    src/guild_member.cpp \
    src/mastery_tree.cpp \
    src/item_factory.cpp \
    src/player_property.cpp \
    src/spawnobject.cpp \
    src/coord.cpp \
    src/storage.cpp \
    src/stall.cpp \
    src/union.cpp \
    src/guild.cpp \
    src/skill_buff.cpp \
    src/npc_util.cpp \
    src/market_stock.cpp \
    src/party.cpp \
    src/transport.cpp \
    src/skill_builder.cpp \
    src/skill.cpp \
    src/player.cpp \
    src/npc.cpp \
    src/mob.cpp \
    src/item_property.cpp \
    src/item.cpp \
    src/interactive_npc.cpp \
    src/entity_property.cpp \
    src/character_factory.cpp
debug:DEFINES += GLIBCXX_DEBUG
OTHER_FILES += CMakeLists.txt \
    src/CMakeLists.txt
