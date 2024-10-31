#include "builtin/definitions.h"
#include "builtin/utility.h"
#include <SDL.h>
#include "engine.h"
#include "systems.h"
using namespace std::string_literals;
using namespace std::string_view_literals;
static constexpr auto lib_name = "builtin";
using engine::core::window;

static int maximize_window(lua_State* L) {
    SDL_MaximizeWindow(window());
    return 0;
}
int builtin::get_mouse_position(lua_State* L) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    builtin::create<common::vec2i>(L) = {x, y};
    return 1;
}
int builtin::is_key_down(lua_State* L) {
    std::string_view key = luaL_checkstring(L, 1);
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    lua_pushboolean(L, state[builtin::string_to_scancode(key)]);
    return 1;
}
static int point_in_rect(lua_State* L) {
    auto& p = builtin::check<common::vec2d>(L, 1);
    auto& r = builtin::check<common::recti64>(L, 2);
    lua_pushboolean(L, systems::solvers::is_in_bounds(p, r));
    return 1;
}
int builtin::read_file(lua_State* L) {
    std::filesystem::path path = luaL_checkstring(L, 1);
    if (auto contents = common::read_file(path)) {
        lua_pushstring(L, contents->c_str());
        return 1;
    }
    return 0;
}
/*
void builtin::builtin_init_lib(lua_State *L) {
    const luaL_Reg lib[] = {
        {"maximize_window", maximize_window},
        {"mouse_pos", mouse_pos},
        {"key_down", key_down},
        {"point_in_rect", point_in_rect},
        {"read_text_file", read_file},
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, lib_name, lib);
    lua_pop(L, 1);
    const std::string meta = lib_name + "_metatable"s;
    luaL_newmetatable(L, meta.c_str());
    const luaL_Reg metadata[] = {
        {"__index", index_metamethod},
        {"__newindex", newindex_metamethod},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_getglobal(L, lib_name);
    luaL_getmetatable(L, meta.c_str());
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
}
*/