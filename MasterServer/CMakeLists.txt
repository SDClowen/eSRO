project (MasterServer)

include_directories (/usr/include/mysql)
include_directories (${ESRO_BLOWFISH_SOURCE_DIR}/include)
include_directories (${ESRO_SRNL_SOURCE_DIR}/include)

add_subdirectory(packet)
add_subdirectory(query)

add_executable (masterd connection.cpp main.cpp server.cpp state_agent.cpp state_download.cpp state_emblem.cpp state_gateway.cpp state_handshake.cpp
	state_shard.cpp)

target_link_libraries (masterd masterpkt masterquery SRNL boost_system boost_thread mysqlpp mysqlclient)

# INSTALLATION OF THE PROGRAM

install (TARGETS masterd 
		RUNTIME DESTINATION bin
		PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
