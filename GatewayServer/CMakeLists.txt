project (GatewayServer)

include_directories (${ESRO_BLOWFISH_SOURCE_DIR}/include)
include_directories (${ESRO_SRNL_SOURCE_DIR}/include)

add_subdirectory(packet)

add_executable (gatewayd client_connection.cpp main.cpp server.cpp server_connection.cpp server_state_handshake.cpp server_state_login.cpp 
	server_state_version.cpp state_handshake.cpp state_master.cpp)

target_link_libraries (gatewayd gatewaypkt Blowfish SRNL boost_system boost_thread)

# INSTALLATION OF THE PROGRAM

install (TARGETS gatewayd 
		RUNTIME DESTINATION bin
		PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

# INSTALLATION OF THE CONFIGURATION FILE

#install (PROGRAMS /etc/gatewayd.conf)

