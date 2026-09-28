cmake_minimum_required(VERSION 3.16)
project(vortex_launcher VERSION 1.5.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(VORTEX_VERSION "1.5.5")
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
find_package(Threads REQUIRED)

if(UNIX)
    find_package(OpenSSL REQUIRED)
endif()

target_link_libraries(vortex_launcher_shared PUBLIC
    launcher
    httpcl
    Vulkan::Vulkan
    Threads::Threads
)

if(UNIX)
    target_link_libraries(vortex_launcher_shared PUBLIC OpenSSL::Crypto)
endif()

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
if(UNIX)
    set(INSTALL_BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build/dist/)
    set(INSTALL_MAIN_DIR ${INSTALL_BASE_DIR}/)
    set(INSTALL_BIN_DIR ${INSTALL_BASE_DIR}/bin)
    set(INSTALL_LIB_DIR ${INSTALL_BASE_DIR}/bin)
    set(INSTALL_INCLUDE_DIR ${INSTALL_BASE_DIR}/include)
elseif(WIN32)
    set(USER_PROFILE $ENV{USERPROFILE})
    set(INSTALL_BASE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/build/dist/)
    set(INSTALL_BIN_DIR ${INSTALL_BASE_DIR}/bin)
    set(INSTALL_LIB_DIR ${INSTALL_BASE_DIR}/bin)
    set(INSTALL_MAIN_DIR ${INSTALL_BASE_DIR}/)
    set(INSTALL_INCLUDE_DIR ${INSTALL_BASE_DIR}/include)
endif()

if(APPLE)
    find_package(Vulkan REQUIRED)

    set(_vk_hints $ENV{VULKAN_SDK}/lib $ENV{VULKAN_SDK}/macOS/lib
                  /opt/homebrew/lib /usr/local/lib)
    find_library(MOLTENVK_LIB NAMES libMoltenVK.dylib HINTS ${_vk_hints} NO_DEFAULT_PATH)
    if(NOT MOLTENVK_LIB)
        message(FATAL_ERROR "Unable to find libMoltenVK.dylib")
    endif()

    # symlinks
    get_filename_component(MOLTENVK_REAL "${MOLTENVK_LIB}" REALPATH)
    get_filename_component(VKLOADER_REAL "${Vulkan_LIBRARY}" REALPATH)

    # declare MoltenVK as primary driver
    file(WRITE ${CMAKE_BINARY_DIR}/MoltenVK_icd.json
"{
  \"file_format_version\": \"1.0.0\",
  \"ICD\": {
    \"library_path\": \"../../libMoltenVK.dylib\",
    \"api_version\": \"1.2.0\",
    \"is_portability_driver\": false
  }
}
")

    install(FILES ${MOLTENVK_REAL} DESTINATION ${INSTALL_BIN_DIR} RENAME libMoltenVK.dylib)
    install(FILES ${VKLOADER_REAL} DESTINATION ${INSTALL_BIN_DIR} RENAME libvulkan.1.dylib)
    install(FILES ${CMAKE_BINARY_DIR}/MoltenVK_icd.json
            DESTINATION ${INSTALL_BIN_DIR}/vulkan/icd.d)

    set_target_properties(vortex_launcher PROPERTIES INSTALL_RPATH "@loader_path")

    set(APPLE_CODESIGN_IDENTITY "-" CACHE STRING "Identity")
    install(CODE "
        set(_bin \"${INSTALL_BIN_DIR}\")
        execute_process(COMMAND install_name_tool -id @rpath/libMoltenVK.dylib \${_bin}/libMoltenVK.dylib)
        execute_process(COMMAND install_name_tool -id @rpath/libvulkan.1.dylib \${_bin}/libvulkan.1.dylib)
        foreach(_f libMoltenVK.dylib libvulkan.1.dylib vortex_launcher)
            execute_process(COMMAND codesign --force --options runtime
                            --sign \"${APPLE_CODESIGN_IDENTITY}\" \${_bin}/\${_f})
        endforeach()
    ")
endif()

# Set the installation directories
if(APPLE)
    set_target_properties(vortex_launcher PROPERTIES
        INSTALL_RPATH "@loader_path"
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH_USE_LINK_PATH FALSE)
else()
    set_target_properties(vortex_launcher PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()
set_target_properties(vortex_launcher PROPERTIES
    CMAKE_INSTALL_RPATH "\$ORIGIN"
)

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


install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/ui/launcher/assets/
        DESTINATION ${INSTALL_BIN_DIR}/
        FILES_MATCHING PATTERN "*"
)