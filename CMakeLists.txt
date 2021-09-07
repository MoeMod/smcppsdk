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

# add_definitions(-DENABLE_#ModuleName#) to enable which module(s) you want to use
if (ENABLE_JSON)
    #add_definitions(-DENABLE_JSON)
    add_library(json INTERFACE)
    target_link_libraries(json INTERFACE ${CMAKE_CURRENT_LIST_DIR}/3rdparty/json)
endif()
if (ENABLE_INI)
    add_definitions(-DENABLE_INI)
endif()
if (ENABLE_TOML)
    add_definitions(-DENABLE_TOML)
endif()
if (ENABLE_MYSQL)
    add_definitions(-DENABLE_MYSQL)
endif()
if (ENABLE_HTTP)
    add_definitions(-DENABLE_HTTP)
endif()
if (ENABLE_SOCKET)
    add_definitions(-DENABLE_SOCKET)
endif()
if (ENABLE_WEBSOCKET)
    add_definitions(-DENABLE_WEBSOCKET)
endif()
if (ENABLE_PGSQL)
    add_definitions(-DENABLE_PGSQL)
endif()
if (ENABLE_REDIS)
    add_definitions(-DENABLE_REDIS)
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    target_compile_options(smcppsdk INTERFACE -fcoroutines)
endif()
target_link_libraries(smcppsdk INTERFACE smsdk_ext)
target_link_libraries(smcppsdk INTERFACE CDetour hl2sdk_protobuf_csgo)
target_link_libraries(smcppsdk INTERFACE Boost::boost)