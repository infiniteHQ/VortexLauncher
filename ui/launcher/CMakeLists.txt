cmake_minimum_required(VERSION 3.16)
project(launcher)

set(CMAKE_CXX_STANDARD 17)
if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -O3")
endif()
set(BIN_FOLDER ${CMAKE_BINARY_DIR}/bin)

set(CHERRY_CEF OFF)
set(CHERRY_ENABLE_NET ON)
set(CHERRY_ENABLE_AUDIO ON)
set(IMGUI_ENABLE_FREETYPE ON)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/cherry cherry_build)

file(
    GLOB_RECURSE
    UI_LAUNCHER
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

message(STATUS "UI_LAUNCHER sources: ${UI_LAUNCHER}")

if (UI_LAUNCHER)
    add_library(launcher STATIC ${UI_LAUNCHER})
else()
    message(FATAL_ERROR "No source files found for the launcher target.")
endif()

target_include_directories(launcher PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../main/include")

target_link_libraries(launcher PRIVATE cherry)

if(UNIX AND NOT APPLE)
    target_link_libraries(launcher PRIVATE crypto)
endif()



file(GLOB_RECURSE ASSET_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/*"
)

add_custom_target(sync_assets ALL
    COMMAND ${CMAKE_COMMAND} -E echo "Synchronizing assets..."
    COMMAND ${CMAKE_COMMAND} -E rm -r -f ${BIN_FOLDER}/resources
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${BIN_FOLDER}/
    COMMAND ${CMAKE_COMMAND} -E echo "Assets synchronized."
    DEPENDS ${ASSET_FILES}
)

add_dependencies(launcher sync_assets)
