cmake_minimum_required(VERSION 2.6)
project (ESRO_KHAMAILEON)

include_directories (${ESRO_KHAMAILEON_SOURCE_DIR}/include)
include_directories (${ESRO_SOL_SOURCE_DIR}/include)

add_subdirectory(src)

add_library (khamaileon src/line.cpp src/model.cpp src/model_manager.cpp src/model_mesh.cpp src/model_mesh_collision.cpp
   src/navigation_cell.cpp src/navigation_entity.cpp src/navigation_mesh.cpp src/parser_manager.cpp src/rectangle.cpp
   src/rtree.cpp src/strutil.cpp src/texture_manager.cpp)

target_link_libraries (khamaileon khama_parsers SOL SOIL)
