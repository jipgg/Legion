#include "luau.defs.h"
#include <SDL.h>
#include "engine.h"
#include "luau.h"
#include "systems.h"
using namespace  luau;
using namespace engine;
using namespace std::string_literals;
using namespace std::string_view_literals;

static int index_metamethod(lua_State* L) {
    std::string_view index = luaL_checkstring(L, 2);
    SDL_Window* wnd = core::get_window();
    if (index == "title") {
        lua_pushstring(L, SDL_GetWindowTitle(wnd));
        return 1;
    } else if (index == "window_size") {
        int width, height;
        SDL_GetWindowSize(wnd, &width, &height);
        luau::init<common::Vec2d>(L) = {double(width), double(height)};
        return 1;
    }
    return 0;
}
static int newindex_metamethod(lua_State* L) {
    constexpr int key = 2;
    constexpr int val = 3;
    std::string_view index = luaL_checkstring(L, key);
    SDL_Window* wnd = core::get_window();
    if (index == "title") {
        std::string v = luaL_checkstring(L, val);
        SDL_SetWindowTitle(core::get_window(), v.c_str());
        return 0;
    } else if (index == "window_size") {
        auto& new_size = luau::ref<common::Vec2d>(L, val);
        SDL_SetWindowSize(wnd, int(new_size.at(0)), int(new_size.at(1)));
        return 0;
    }
    return 0;
}
static int maximize_window(lua_State* L) {
    SDL_MaximizeWindow(core::get_window());
    return 0;
}
static int mouse_pos(lua_State* L) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    init<common::Vec2d>(L) = {double(x), double(y)};
    return 1;
}
static int key_down(lua_State* L) {
    std::string_view key = luaL_checkstring(L, 1);
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    lua_pushboolean(L, state[string_to_scancode(key)]);
    return 1;
}
static int point_in_rect(lua_State* L) {
    auto& p = luau::ref<common::Vec2d>(L, 1);
    auto& r = luau::ref<common::Recti64>(L, 2);
    lua_pushboolean(L, systems::solvers::is_in_bounds(p, r));
    return 1;
}
void luau::builtin::init_lib(lua_State *L) {
    const luaL_Reg lib[] = {
        {"maximize_window", maximize_window},
        {"mouse_pos", mouse_pos},
        {"key_down", key_down},
        {"point_in_rect", point_in_rect},
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
