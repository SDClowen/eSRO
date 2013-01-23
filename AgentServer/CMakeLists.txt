project (AgentServer)

include_directories(/usr/include/mysql)
include_directories (${ESRO_BLOWFISH_SOURCE_DIR}/include)
include_directories (${ESRO_SRNL_SOURCE_DIR}/include)
include_directories (${ESRO_SOL_SOURCE_DIR}/include)
include_directories (${ESRO_SQL_SOURCE_DIR})
include_directories (${ESRO_EPL_SOURCE_DIR}/include)
include_directories (${ESRO_KHAMAILEON_SOURCE_DIR}/include)
include_directories (${AgentServer_SOURCE_DIR} ${AgentServer_SOURCE_DIR}/handler ${AgentServer_SOURCE_DIR}/manager ${AgentServer_SOURCE_DIR}/query)

add_subdirectory(query)
add_subdirectory(handler)
add_subdirectory(manager)

add_executable (agentd client_connection.cpp main.cpp server.cpp server_channel.cpp server_connection.cpp server_state_game.cpp server_state_handshake.cpp 	
	server_state_lobby.cpp server_state_login.cpp state_handshake.cpp state_master.cpp world.cpp zone.cpp formulas.cpp)

target_link_libraries (agentd agentquery agenthandler agentmanager Blowfish SRNL SOL SQL EPL khamaileon boost_system boost_thread mysqlclient mysqlpp)

# INSTALLATION OF THE PROGRAM

install (TARGETS agentd 
		RUNTIME DESTINATION bin
		PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
