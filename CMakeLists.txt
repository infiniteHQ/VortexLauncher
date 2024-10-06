cmake_minimum_required(VERSION 3.16)
project(vortex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC")
set(VORTEX_VERSION "1.1")

file(GLOB_RECURSE VORTEX_SHARED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")

add_library(vortex_shared SHARED ${VORTEX_SHARED_SOURCES})

target_include_directories(vortex_shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/main/include
    ${CMAKE_CURRENT_SOURCE_DIR}/main/src
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ui/launcher ui_launcher_build)
target_link_libraries(vortex_shared PUBLIC launcher)

file(GLOB_RECURSE VORTEX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")

add_executable(vortex ${VORTEX_SOURCES} main.cpp)
target_link_libraries(vortex PRIVATE vortex_shared)

install(TARGETS vortex RUNTIME DESTINATION bin)
install(TARGETS vortex_shared LIBRARY DESTINATION lib)
