project (ESRO_SQL)

include_directories(/usr/include/mysql)
include_directories (${ESRO_SOL_SOURCE_DIR}/include)
include_directories (${ESRO_SQL_SOURCE_DIR}/include)

add_library (SQL STATIC property_query.cpp)
