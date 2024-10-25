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
find_package(blaze REQUIRED)
add_subdirectory(ext/Luau)

file(GLOB SOURCE_FILES src/*.cpp)
if (LINUX)
    add_executable(Legion ${SOURCE_FILES})
elseif(WIN32)
    add_executable(Legion WIN32 ${SOURCE_FILES})
endif()
target_link_libraries(Legion PRIVATE
    SDL2::SDL2main
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    Luau.VM
    Luau.Compiler
    Luau.CodeGen
    Luau.Config
)
target_link_libraries(Legion PUBLIC blaze::blaze )
