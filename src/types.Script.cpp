#include "types.h"
#include "engine.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include <exception>
#include <string_view>
#include "luau.defs.h"
using namespace std::string_literals;
namespace fs = std::filesystem;
namespace types {
Script::Script(const fs::path& file):
    script_thread_(lua_newthread(engine::core::get_lua_state()), lua_close) {
    lua_State* L = script_thread_.get();
    luaL_sandboxthread(L);
    load_file(file);
    lua_xmove(L, engine::core::get_lua_state(), 0);
}
Script::Script(std::string_view string):
    script_thread_(lua_newthread(engine::core::get_lua_state()), lua_close) {
    luaL_sandboxthread(script_thread_.get());
    load_string(string);
}
void Script::load_string(std::string_view str) {
    size_t outsize{};
    char* bytecode_data = luau_compile(str.data(), str.size(), nullptr, &outsize);
    if (luau_load(script_thread_.get(), "string", bytecode_data, outsize, 0)) {
        common::printerr("Failed to load:", bytecode_data);
        std::free(bytecode_data);
    }
    std::free(bytecode_data);
    call();
}
void Script::load_file(const fs::path& filepath) {
    std::optional<std::string> source = common::read_file(filepath);
    if (not source) {
        common::printerr("failed to read the file '"s + filepath.string() + "'");
        return;
    }
    load_string(*source);
}
void Script::call(int argc, int retc) {
    try {
        lua_call(script_thread_.get(), argc, retc);
    } catch (std::exception& e) {
        common::printerr("Luau error:", e.what());
    }
}
}
