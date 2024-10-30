#include "builtin/typedefs.h"
#include "builtin/definitions.h"
#include <cstdint>
#include <lualib.h>
#include "builtin/utility.h"
namespace bi = builtin;
namespace mm = bi::metamethod;
//coloru32
static constexpr auto coloru32_tname = "Coloru32";
static constexpr auto texture_tname = "SDL_Texture";
static constexpr auto font_tname = "TTF_Font";
static constexpr auto rect_tname = "Rect";
static constexpr auto recti_tname = "Recti";
static int coloru32_ctor(lua_State *L) {
    uint32_t r{};
    uint8_t g{}, b{}, a{};
    if (lua_isnumber(L, 1)) r = luaL_checkinteger(L, 1);
    if (lua_isnone(L, 2)) {
        bi::create<bi::coloru32_t>(L, static_cast<uint32_t>(r));
        return 1;
    }
    if (lua_isnumber(L, 2)) g = luaL_checkinteger(L, 2);
    if (lua_isnumber(L, 3)) b = luaL_checkinteger(L, 3);
    if (lua_isnumber(L, 4)) a = luaL_checkinteger(L, 4);
    bi::create<bi::coloru32_t>(L, static_cast<uint8_t>(r), g, b, a);
    return 1;
}
static int coloru32_index(lua_State *L) {
    const auto& self = bi::check<bi::coloru32_t>(L, 1);
    const char key = *luaL_checkstring(L, 2);
    switch(key) {
        case 'r':
            lua_pushinteger(L, self.red());
            return 1;
        case 'g':
            lua_pushinteger(L, self.green());
            return 1;
        case 'b':
            lua_pushinteger(L, self.blue());
            return 1;
        case 'a':
            lua_pushinteger(L, self.alpha());
            return 1;
        }
    return 0;
}
static int coloru32_newindex(lua_State *L) {
    luaL_error(L, "members are read-only");
}
void coloru32_init(lua_State *L) {
    luaL_newmetatable(L, bi::metatable_name<bi::coloru32_t>());
    const luaL_Reg metadata[] = {
        {mm::index, coloru32_index},
        {mm::newindex, coloru32_newindex},
        {nullptr, nullptr}
    };
    lua_pushstring(L, coloru32_tname);
    lua_setfield(L, -2, mm::type);
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, coloru32_ctor, coloru32_tname);
    lua_setglobal(L, coloru32_tname);
}
//font
static void font_init(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<bi::font_t>());
    lua_pushstring(L, font_tname);
    lua_setfield(L, -2, mm::type);
    lua_pop(L, 1);
}
//rect
static int rect_index(lua_State* L) {
    auto& r = bi::check<bi::rect_t>(L, 1);
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
static int rect_newindex(lua_State* L) {
    auto& r = bi::check<bi::rect_t>(L, 1);
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
static int rect_ctor(lua_State* L) {
    if (lua_isnone(L, 2)) {
        if (bi::is_type<bi::recti_t>(L, 1)) {
            auto& r = bi::check<bi::recti_t>(L, 1);
            bi::create<bi::rect_t>(L, double(r.x), double(r.y), double(r.w), double(r.h));
            return 1;
        }
    }
    double x = luaL_optnumber(L, 1, 0);
    double y = luaL_optnumber(L, 2, 0);
    double w = luaL_optnumber(L, 3, 0);
    double h = luaL_optnumber(L, 4, 0);
    bi::create<bi::rect_t>(L, x, y, w, h);
    return 1;
}
static void rect_init(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<bi::rect_t>());
    const luaL_Reg rect[] = {
        {mm::index, rect_index},
        {mm::newindex, rect_newindex},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, rect);
    lua_pushstring(L, rect_tname);
    lua_setfield(L, -2, mm::type);
    lua_pop(L, 1);
    lua_pushcfunction(L, rect_ctor, rect_tname);
    lua_setglobal(L, rect_tname);
}
//recti
static int recti_index(lua_State* L) {
    auto& r = bi::check<bi::recti_t>(L, 1);
    char key = *luaL_checkstring(L, 2);
    switch (key) {
        case 'x': lua_pushinteger(L, r.x); return 1;
        case 'y': lua_pushinteger(L, r.y); return 1;
        case 'w': lua_pushinteger(L, r.w); return 1;
        case 'h': lua_pushinteger(L, r.h); return 1;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
};
static int recti_newindex(lua_State* L) {
    auto& r = bi::check<bi::recti_t>(L, 1);
    char key = *luaL_checkstring(L, 2);
    int v = luaL_checkinteger(L, 3);
    switch (key) {
        case 'x': r.x = v; return 0;
        case 'y': r.y = v; return 0;
        case 'w': r.w = v; return 0;
        case 'h': r.h = v; return 0;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
}
static int recti_ctor(lua_State* L) {
    if (lua_isnone(L, 2)) {
        if (bi::is_type<bi::rect_t>(L, 1)) {
            auto& r = bi::check<bi::rect_t>(L, 1);
            bi::create<bi::recti_t>(L, int(r.x), int(r.y), int(r.w), int(r.h));
            return 1;
        }
    }
    int x = luaL_optinteger(L, 1, 0);
    int y = luaL_optinteger(L, 2, 0);
    int w = luaL_optinteger(L, 3, 0);
    int h = luaL_optinteger(L, 4, 0);
    bi::create<bi::recti_t>(L, x, y, w, h);
    return 1;
}
static void recti_init(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<bi::recti_t>());
    const luaL_Reg recti[] = {
        {mm::index, recti_index},
        {mm::newindex, recti_newindex},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, recti);
    lua_pushstring(L, recti_tname);
    lua_setfield(L, -2, mm::type);
    lua_pop(L, 1);
    lua_pushcfunction(L, recti_ctor, recti_tname);
    lua_setglobal(L, recti_tname);
}
//texture
static void texture_init(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<bi::texture_t>());
    lua_pushstring(L, texture_tname);
    lua_setfield(L, -2, mm::type);
    lua_pop(L, 1);
}
void builtin::init_global_types(lua_State *L) {
    coloru32_init(L);
    font_init(L);
    rect_init(L);
    recti_init(L);
    texture_init(L);
}
