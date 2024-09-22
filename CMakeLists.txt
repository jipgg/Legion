cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Legion LANGUAGES CXX C)
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
add_subdirectory(extern/Luau)
file(GLOB SOURCES src/*.cpp)
if (LINUX)
    add_executable(Legion ${SOURCES})
elseif(WIN32)
    add_executable(Legion WIN32 ${SOURCES})
endif()
target_include_directories(Legion PRIVATE src)
target_link_libraries(Legion PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
)
target_link_libraries(Legion PRIVATE
    Luau.VM
    Luau.Compiler
    Luau.CodeGen
)

