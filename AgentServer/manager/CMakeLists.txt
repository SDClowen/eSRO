add_library (agentmanager STATIC academy_manager.cpp event_manager.cpp friend_manager.cpp fortress_manager.cpp guild_manager.cpp map_manager.cpp party_manager.cpp resource_manager.cpp script_manager.cpp skill_manager.cpp stall_manager.cpp trade_manager.cpp union_manager.cpp weather_manager.cpp)

target_link_libraries(agentmanager agentquery)
