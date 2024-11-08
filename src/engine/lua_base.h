#pragma once
#include <filesystem>
#include <Luau/Compiler.h>
struct lua_State;


Luau::CompileOptions compile_options();
void lua_register_globals(lua_State* L);
void set_luau_module_global(lua_State* L, const std::filesystem::path& path);
void push_luau_module(lua_State* L, const std::filesystem::path& path);
