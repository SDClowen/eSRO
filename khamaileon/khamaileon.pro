# -------------------------------------------------
# Project created by QtCreator 2010-05-12T17:27:37
# -------------------------------------------------
QT -= core \
    gui
TARGET = khamaileon
TEMPLATE = lib
CONFIG += staticlib
INCLUDEPATH += include \
    ../SOL/include
LIBS += -L../SOL \
	-lSOL . -lSOIL
PRE_TARGETDEPS += ../SOL/libSOL.a
debug:DEFINES += DEBUG \
    GLIBCXX_DEBUG
debug:QMAKE_CXX_FLAGS = -Wall
SOURCES += src/parser_manager.cpp \
    src/parser/characterdata.cpp \
    src/parser/itemdata.cpp \
    src/parser/skilldata.cpp \
    src/parser/refshoptab.cpp \
    src/parser/refshopgoods.cpp \
    src/parser/refscrapofpackageitem.cpp \
    src/parser/refpackageitem.cpp \
    src/parser/refmappingshopwithtab.cpp \
    src/parser/refmappingshopgroup.cpp \
    src/parser/npcpos.cpp \
    src/parser/levelgold.cpp \
    src/parser/leveldata.cpp \
    src/parser/refpricepolicyofitem.cpp \
    src/parser/refshopgroup.cpp \
    src/parser/teleportoptional.cpp \
    src/parser/teleportlink.cpp \
    src/parser/teleportdata.cpp \
    src/parser/teleportbuilding.cpp \
    src/parser/siegefortress.cpp \
    src/model_manager.cpp \
    src/texture_manager.cpp \
    src/strutil.cpp \
    src/rtree.cpp \
    src/navigation_entity.cpp \
    src/model_mesh.cpp \
    src/model.cpp \
    src/line.cpp \
    src/model_mesh_collision.cpp \
    src/navigation_mesh.cpp \
    src/navigation_cell.cpp \
    src/rectangle.cpp
HEADERS += include/parser_manager.hpp \
    include/parser/characterdata.hpp \
    include/parser/itemdata.hpp \
    include/parser/skilldata.hpp \
    include/parser/refshoptab.hpp \
    include/parser/refshopgroup.hpp \
    include/parser/refshopgoods.hpp \
    include/parser/refscrapofpackageitem.hpp \
    include/parser/refpricepolicyofitem.hpp \
    include/parser/refpackageitem.hpp \
    include/parser/refmappingshopwithtab.hpp \
    include/parser/refmappingshopgroup.hpp \
    include/parser/npcpos.hpp \
    include/parser/levelgold.hpp \
    include/parser/leveldata.hpp \
    include/parser/teleportoptional.hpp \
    include/parser/teleportlink.hpp \
    include/parser/teleportdata.hpp \
    include/parser/teleportbuilding.hpp \
    include/parser/siegefortress.hpp \
    include/model_manager.hpp \
    include/texture_manager.hpp \
    include/strutil.hpp \
    include/rtree.hpp \
    include/rectangle.hpp \
    include/navigation_entity.hpp \
    include/model_mesh.hpp \
    include/model.hpp \
    include/line.hpp \
    include/model_mesh_collision.hpp \
    include/navigation_mesh.hpp \
    include/navigation_cell.hpp \
    include/vector.hpp \
    include/triangle.hpp \
    include/segment.hpp \
    include/ray.hpp \
    include/teleport.hpp
OTHER_FILES += src/CMakeLists.txt \
    src/parser/CMakeLists.txt \
    CMakeLists.txt
