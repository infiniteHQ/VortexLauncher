cmake_minimum_required(VERSION 3.16)
project(vortex_launcher VERSION 1.5.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(VORTEX_VERSION "1.5.2")
set(VORTEX_DIST "beta")
set(VORTEX_BUILD "dev")

if (MSVC)
    add_compile_options(/utf-8)
endif()

if(VORTEX_BUILD STREQUAL "prod")
    string(TIMESTAMP BUILD_DATE "%b %d, %Y - %H:%M" UTC)
    add_compile_definitions(BUILD_DATE_STR="${BUILD_DATE}")
    add_compile_definitions(BUILD_DIST="${BUILD_DATE}")

    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    add_compile_definitions(GIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
else()
    add_compile_definitions(BUILD_DATE_STR="DEV_BUILD")
    add_compile_definitions(BUILD_DIST="DEV_BUILD")
    add_compile_definitions(GIT_COMMIT_HASH="0000000")
endif()

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fdata-sections -ffunction-sections -D_GLIBCXX_USE_CXX11_ABI=1")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -s")
endif()

file(GLOB_RECURSE VORTEX_CORE_SRC 
    "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.c"
)

add_library(vortex_launcher_shared STATIC ${VORTEX_CORE_SRC})

target_include_directories(vortex_launcher_shared PUBLIC 
    main/include 
    main/src
    lib/spdlog/include 
    lib/restclient-cpp/include
)

find_package(Vulkan REQUIRED)
find_package(CURL REQUIRED)

add_subdirectory(ui/launcher ui_launcher_build)

target_link_libraries(vortex_launcher_shared PUBLIC launcher httpcl Vulkan::Vulkan)

add_executable(vortex_launcher main.cpp)
target_link_libraries(vortex_launcher PRIVATE vortex_launcher_shared)

if(WIN32)
    set_target_properties(vortex_launcher PROPERTIES WIN32_EXECUTABLE TRUE)
    target_link_libraries(vortex_launcher PRIVATE Shell32)
else()
    target_link_libraries(vortex_launcher PRIVATE CURL::libcurl pthread dl)
endif()

# Installation
install(TARGETS vortex_launcher vortex_launcher_shared DESTINATION bin)
install(DIRECTORY main/include/ DESTINATION include/main)