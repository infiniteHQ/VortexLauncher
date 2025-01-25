cmake_minimum_required(VERSION 3.16)
project(vortex_launcher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(VORTEX_VERSION "1.3.3")

set(CHERRY_CEF OFF)
set(CHERRY_NET ON)

file(GLOB_RECURSE vortex_launcher_SHARED_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")

add_library(vortex_launcher_shared SHARED ${vortex_launcher_SHARED_SOURCES})

target_include_directories(vortex_launcher_shared PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/main/include
    ${CMAKE_CURRENT_SOURCE_DIR}/main/src
)

find_package(Vulkan REQUIRED)
include_directories(${Vulkan_INCLUDE_DIRS})
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib/spdlog/include")

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ui/launcher ui_launcher_build)
target_link_libraries(vortex_launcher_shared PUBLIC launcher)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/lib/restcpp restcpp_build)
target_link_libraries(vortex_launcher_shared PUBLIC restclient-cpp)

file(GLOB_RECURSE VORTEX_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp")

add_executable(vortex_launcher ${VORTEX_SOURCES} main.cpp)
target_link_libraries(vortex_launcher PRIVATE vortex_launcher_shared)

#TODO : Do only in cherry
####target_link_libraries(vortex_launcher PRIVATE cherry SDL2)
####set(CEF_RELEASE_DIR "${CMAKE_BINARY_DIR}/Release")
####set(CEF_EXTRA_LIBRARIES
####    "${CEF_RELEASE_DIR}/libEGL.so"
####    "${CEF_RELEASE_DIR}/libGLESv2.so"
####    "${CEF_RELEASE_DIR}/libvk_swiftshader.so"
####    "${CEF_RELEASE_DIR}/libvulkan.so.1"
   #### "${CEF_RELEASE_DIR}/libcef.so"
####)
####target_link_libraries(vortex_launcher PRIVATE ${CEF_EXTRA_LIBRARIES})
#################

# Set the installation directories
if(UNIX AND NOT APPLE)
    set(INSTALL_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/dist/)
    set(INSTALL_MAIN_DIR ${INSTALL_BASE_DIR}/)
    set(INSTALL_BIN_DIR ${INSTALL_BASE_DIR}/bin)
    set(INSTALL_LIB_DIR ${INSTALL_BASE_DIR}/lib)
    set(INSTALL_INCLUDE_DIR ${INSTALL_BASE_DIR}/include)
elseif(WIN32)
    set(USER_PROFILE $ENV{USERPROFILE})
    set(INSTALL_BASE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/build/dist/)
    set(INSTALL_BIN_DIR ${INSTALL_BASE_DIR}/bin)
    set(INSTALL_LIB_DIR ${INSTALL_BASE_DIR}/lib)
    set(INSTALL_MAIN_DIR ${INSTALL_BASE_DIR}/)
    set(INSTALL_INCLUDE_DIR ${INSTALL_BASE_DIR}/include)
endif()

set_target_properties(vortex_launcher PROPERTIES
    INSTALL_RPATH "${INSTALL_LIB_DIR};${CEF_RELEASE_DIR}"
    BUILD_WITH_INSTALL_RPATH TRUE
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

#TODO : Do only in cherry
####install(FILES
####    ${CEF_RELEASE_DIR}/libcef.so
   #### ${CEF_RELEASE_DIR}/libvk_swiftshader.so
####    DESTINATION ${INSTALL_LIB_DIR}
####)
####
# Install targets
install(TARGETS vortex_launcher DESTINATION ${INSTALL_BIN_DIR})
install(TARGETS vortex_launcher_shared LIBRARY DESTINATION ${INSTALL_LIB_DIR})

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/main/include/
        DESTINATION ${INSTALL_INCLUDE_DIR}/main/include/
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ui/launcher/src/
        DESTINATION ${INSTALL_INCLUDE_DIR}/ui/launcher/src/
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/
        DESTINATION ${INSTALL_INCLUDE_DIR}/
        FILES_MATCHING 
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "*.inl"
)

# Add installation commands for other libraries if needed
install(TARGETS restclient-cpp
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR})

install(TARGETS spdlog
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR})

install(TARGETS cherry
        LIBRARY DESTINATION ${INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${INSTALL_BIN_DIR})

target_include_directories(vortex_launcher_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/lib/restclient-cpp/include)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ui/launcher/assets/
        DESTINATION ${INSTALL_BIN_DIR}/
        FILES_MATCHING PATTERN "*"
)
