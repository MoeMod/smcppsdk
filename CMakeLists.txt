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

if(${CMAKE_CXX_COMPILER_ID} MATCHES "GNU")
    target_compile_options(smcppsdk INTERFACE -fcoroutines)
endif()
target_link_libraries(smcppsdk INTERFACE smsdk_ext)
target_link_libraries(smcppsdk INTERFACE CDetour hl2sdk_protobuf_csgo)