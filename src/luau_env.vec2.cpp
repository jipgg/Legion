#include "luau_env/Vec2.h"
#include "legion/common.h"
#include "luau_env/userdata_utility.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include <source_location>
namespace luau_env {
void vec2::init_metadata(lua_State* L) {
    luaL_newmetatable(L, metatable_name<Vec2d>());
    const luaL_Reg metadata [] = {
        {"__index", index},
        {"__add", add},
        {"__mul", mul},
        {"__namecall", namecall},
        {"__newindex", newindex},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, "V2");
    lua_setglobal(L, "V2");
}
int vec2::ctor(lua_State *L) {
    double x = luaL_checknumber(L, 1);
    double y = luaL_checknumber(L, 2);
    init<Vec2d>(L, {x, y});
    std::string aa;
    aa.find()
    lua_callbacks(L)->useratom = [](const char* name, size_t idk) {
        return int16_t(1);
    };
    return 1;
}
int vec2::add(lua_State* L) {
    const auto& a = get<Vec2d>(L, 1);
    const auto& b = get<Vec2d>(L, 2);
    init<Vec2d>(L, {a + b});
    return 1;
}
int vec2::mul(lua_State *L) {
    const auto& a = get<Vec2d>(L, 1);
    const auto& b = get<Vec2d>(L, 2);
    init<Vec2d>(L, {a * b});
    return 1;
}
int vec2::namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    switch(atom) {
    }
    return 0;
}
}
