cmake_minimum_required(VERSION 3.16)
project(launcher)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Ajout du sous-répertoire cherry
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../lib/cherry cherry_build)

# Ajout des fichiers source
file(
    GLOB_RECURSE
    UI_LAUNCHER
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Debug: Affiche les fichiers détectés
message(STATUS "UI_LAUNCHER sources: ${UI_LAUNCHER}")

# Vérifie que des fichiers ont bien été trouvés
if (UI_LAUNCHER)
    add_library(launcher STATIC ${UI_LAUNCHER})
else()
    message(FATAL_ERROR "No source files found for the launcher target.")
endif()

# Lien avec la bibliothèque cherry
target_link_libraries(launcher PRIVATE cherry)

# Copie les assets après la compilation
add_custom_command(TARGET launcher POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/
)

