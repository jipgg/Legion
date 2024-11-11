#include <cstdint>
#include <lualib.h>
#include "builtin.h"
#include "lua_util.h"
namespace bi = builtin;
namespace mm = bi::metamethod;
namespace tn = bi::tname;
static int color_ctor(lua_State *L) {
    uint8_t r{}, g{}, b{}, a{};
    create<bi::color>(L,
        static_cast<uint8_t>(luaL_optinteger(L, 1, 0)),
        static_cast<uint8_t>(luaL_optinteger(L, 2, 0)),
        static_cast<uint8_t>(luaL_optinteger(L, 3, 0)), 
        static_cast<uint8_t>(luaL_optinteger(L, 4, 0xff)));
    return 1;
}
static int color_index(lua_State *L) {
    const auto& self = check<bi::color>(L, 1);
    const char key = *luaL_checkstring(L, 2);
    switch(key) {
        case 'r': lua_pushinteger(L, self.r); return 1;
        case 'g': lua_pushinteger(L, self.g); return 1;
        case 'b': lua_pushinteger(L, self.b); return 1;
        case 'a': lua_pushinteger(L, self.a); return 1;
        }
    return 0;
}
static int color_newindex(lua_State *L) {
    luaL_error(L, "members are read-only");
}
void color_init(lua_State *L) {
    luaL_newmetatable(L, metatable_name<bi::color>());
    const luaL_Reg metadata[] = {
        {mm::index, color_index},
        {mm::newindex, color_newindex},
        {nullptr, nullptr}
    };
    lua_pushstring(L, tn::color);
    lua_setfield(L, -2, mm::type);
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, color_ctor, "color_ctor");
    lua_setglobal(L, "Color");
}
//font
static void font_init(lua_State* L) {
    luaL_newmetatable(L, metatable_name<bi::opaque_font>());
    lua_pushstring(L, tn::opaque_font);
    lua_setfield(L, -2, mm::type);
    lua_pop(L, 1);
}
//rect
static int rectangle_index(lua_State* L) {
    auto& r = check<bi::rectangle>(L, 1);
    char key = *luaL_checkstring(L, 2);
    switch (key) {
        case 'x': lua_pushnumber(L, r.x); return 1;
        case 'y': lua_pushnumber(L, r.y); return 1;
        case 'w': lua_pushnumber(L, r.w); return 1;
        case 'h': lua_pushnumber(L, r.h); return 1;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
};
static int rectangle_newindex(lua_State* L) {
    auto& r = check<bi::rectangle>(L, 1);
    char key = *luaL_checkstring(L, 2);
    int v = luaL_checknumber(L, 3);
    switch (key) {
        case 'x': r.x = v; return 0;
        case 'y': r.y = v; return 0;
        case 'w': r.w = v; return 0;
        case 'h': r.h = v; return 0;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
}
static int rectangle_ctor(lua_State* L) {
    double x = luaL_optnumber(L, 1, 0);
    double y = luaL_optnumber(L, 2, 0);
    double w = luaL_optnumber(L, 3, 0);
    double h = luaL_optnumber(L, 4, 0);
    create<bi::rectangle>(L, x, y, w, h);
    return 1;
}
static void rectangle_init(lua_State* L) {
    luaL_newmetatable(L, metatable_name<bi::rectangle>());
    const luaL_Reg meta[] = {
        {mm::index, rectangle_index},
        {mm::newindex, rectangle_newindex},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, meta);
    lua_pushstring(L, tn::rectangle);
    lua_setfield(L, -2, mm::type);
    lua_pop(L, 1);
    lua_pushcfunction(L, rectangle_ctor, "rectangle_ctor");
    lua_setglobal(L, "Rectangle");
}
static void texture_init(lua_State* L) {
    luaL_newmetatable(L, metatable_name<bi::opaque_texture>());
    lua_pushstring(L, tn::opaque_texture);
    lua_setfield(L, -2, mm::type);
    lua_pop(L, 1);
}
void builtin::init_global_types(lua_State *L) {
    color_init(L);
    font_init(L);
    rectangle_init(L);
    texture_init(L);
}
