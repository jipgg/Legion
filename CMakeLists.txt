cmake_minimum_required(VERSION 3.22.2)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(source_dir ${CMAKE_SOURCE_DIR}/src)
set(include_dir ${CMAKE_SOURCE_DIR}/include)
set(resource_dir ${CMAKE_SOURCE_DIR}/resources)
set(luau_library_dir ${resource_dir}/luau_library)
set(resource_output_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources)
project(luWaw LANGUAGES CXX C)

find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(blaze REQUIRED)
add_subdirectory(extern/luau)

#file(COPY ${lib_dir} DESTINATION ${resource_output_dir})
#file(COPY ${CMAKE_SOURCE_DIR}/.luaurc DESTINATION ${resource_output_dir}/build_resources/.luaurc)

file(GLOB_RECURSE source_files ${source_dir}/*.cpp)
if (LINUX)
    add_executable(luWaw ${source_files})
elseif(WIN32)
    add_executable(luWaw WIN32 ${source_files})
endif()
target_include_directories(luWaw PUBLIC ${include_dir})
target_link_libraries(luWaw PRIVATE
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
target_link_libraries(luWaw PUBLIC blaze::blaze )
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${resource_dir}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/resources")
