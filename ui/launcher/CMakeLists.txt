cmake_minimum_required(VERSION 3.16)
project(launcher)

# Options
option(CHERRY_STATIC "Link Cherry statically" ON)
option(CHERRY_DEBUG "Enable Cherry debug tools" ON)
option(CHERRY_ENABLE_NET "Enable net features" ON)
option(CHERRY_ENABLE_AUDIO "Enable audio features" OFF)

add_subdirectory(../../lib/cherry cherry_build)

file(GLOB_RECURSE UI_SRC 
    "src/*.cpp" 
    "src/*.hpp" 
    "src/*.h"
    "../../main/include/*.hpp" 
    "../../main/src/*.cpp"
)

add_library(launcher STATIC ${UI_SRC})

target_include_directories(launcher PRIVATE "../../main/include")
target_link_libraries(launcher PUBLIC cherry)

if(UNIX AND NOT APPLE)
    target_link_libraries(launcher PRIVATE crypto)
endif()

add_custom_target(sync_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/bin
    COMMENT "Syncing assets to bin..."
)
add_dependencies(launcher sync_assets)


