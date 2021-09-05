cmake_minimum_required(VERSION 3.13)
project(smcppsdk)

add_library(smcppsdk INTERFACE)
target_sources(smcppsdk INTERFACE 
    ${CMAKE_CURRENT_LIST_DIR}/src/sm/sdkhooks.cpp
)

target_include_directories(smcppsdk INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/include
)

target_include_directories(smcppsdk INTERFACE
    ${HL2SDK_PATH}/public
    ${HL2SDK_PATH}/public/engine
    ${HL2SDK_PATH}/game
    ${HL2SDK_PATH}/public/game
    ${HL2SDK_PATH}/public/game/server
    ${HL2SDK_PATH}/public/game/shared
)

if (ENABLE_JSON MATCHES ON)
    #add_library()
endif()
if (ENABLE_INI MATCHES ON)
    #add_library()
endif()
if (ENABLE_TOML MATCHES ON)
    #add_library()
endif()
if (ENABLE_MYSQL MATCHES ON)
    #add_library()
endif()
if (ENABLE_HTTP MATCHES ON)
    #add_library()
endif()
if (ENABLE_SOCKET MATCHES ON)
    #add_library()
endif()
if (ENABLE_WEBSOCKET MATCHES ON)
    #add_library()
endif()
if (ENABLE_PGSQL MATCHES ON)
    #add_library()
endif()
if (ENABLE_REDIS MATCHES ON)
    #add_library()
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    target_compile_options(smcppsdk INTERFACE -fcoroutines)
endif()
target_link_libraries(smcppsdk INTERFACE smsdk_ext)
target_link_libraries(smcppsdk INTERFACE CDetour hl2sdk_protobuf_csgo)
target_link_libraries(smcppsdk INTERFACE Boost::boost)