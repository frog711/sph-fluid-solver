cmake_minimum_required(VERSION 3.28)
project(SPH_Fluid_Solver LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

add_library(structures src/structures.cpp)

add_library(parser src/parser.cpp)

add_library(kernel src/kernel.cpp)
target_compile_features(kernel PRIVATE cxx_std_17)
target_link_libraries(kernel PRIVATE SFML::Graphics structures)

add_library(simulation src/simulate.cpp)
target_compile_features(simulation PRIVATE cxx_std_17)
target_link_libraries(simulation PRIVATE SFML::Graphics parser structures)

add_library(render src/render.cpp)
target_compile_features(render PRIVATE cxx_std_17)
target_link_libraries(render PRIVATE SFML::Graphics simulation)

add_executable(main src/main.cpp)
target_compile_features(main PRIVATE cxx_std_17)
target_link_libraries(main PRIVATE SFML::Graphics simulation render kernel)
