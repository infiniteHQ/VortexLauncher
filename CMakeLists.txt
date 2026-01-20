cmake_minimum_required(VERSION 3.16)
project(vortex_launcher VERSION 1.5.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(VORTEX_VERSION "1.5.3")
set(VORTEX_DIST "beta")
set(VORTEX_BUILD "dev")

if (MSVC)
    add_compile_options(/O1 /GL /Gw /Gy)
    add_link_options(/OPT:REF /OPT:ICF /LTCG)
    add_compile_options(/utf-8 /O2 /permissive-)
    add_compile_definitions(NOMINMAX WIN32_LEAN_AND_MEAN)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -fdata-sections -ffunction-sections")
    if(NOT APPLE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=1")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--gc-sections -s")
    endif()
endif()

if(VORTEX_BUILD STREQUAL "prod")
    string(TIMESTAMP BUILD_DATE "%b %d, %Y - %H:%M" UTC)
    add_compile_definitions(BUILD_DATE_STR="${BUILD_DATE}" BUILD_DIST="${VORTEX_DIST}")
    execute_process(
        COMMAND git rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(NOT GIT_COMMIT_HASH) 
        set(GIT_COMMIT_HASH "unknown") 
    endif()
    add_compile_definitions(GIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
else()
    add_compile_definitions(BUILD_DATE_STR="DEV_BUILD" BUILD_DIST="DEV_BUILD" GIT_COMMIT_HASH="0000000")
endif()

file(GLOB_RECURSE VORTEX_CORE_SRC 
    "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/main/src/*.c"
)

add_library(vortex_launcher_shared STATIC ${VORTEX_CORE_SRC})

target_include_directories(vortex_launcher_shared PUBLIC 
    main/include 
    main/src
    lib/restclient-cpp/include
)

find_package(Vulkan REQUIRED)

add_subdirectory(ui/launcher ui_launcher_build)

target_link_libraries(vortex_launcher_shared PUBLIC launcher httpcl Vulkan::Vulkan)

if(WIN32)
    target_link_libraries(vortex_launcher_shared PRIVATE Shell32)
endif()

if(WIN32)
    add_executable(vortex_launcher WIN32 main.cpp)
else()
    add_executable(vortex_launcher main.cpp)
endif()

target_link_libraries(vortex_launcher PRIVATE vortex_launcher_shared)

if(WIN32)
    set(INSTALL_BIN_DIR "bin")
    set(INSTALL_INC_DIR "include/main")
else()
    set(INSTALL_BIN_DIR "bin")
    set(INSTALL_INC_DIR "include/main")
endif()

install(TARGETS vortex_launcher vortex_launcher_shared 
        RUNTIME DESTINATION ${INSTALL_BIN_DIR}
        ARCHIVE DESTINATION lib)

install(DIRECTORY main/include/ DESTINATION ${INSTALL_INC_DIR})

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ui/launcher/assets/")
    install(DIRECTORY ui/launcher/assets/ DESTINATION ${INSTALL_BIN_DIR}/assets)
endif()