#include "builtin.h"
#include "builtin_types.h"
#include "engine.h"
#include "lua_util.h"
using builtin::Rect;
static constexpr auto type = "Rect";

static int index(lua_State* L) {
    auto& r = check<Rect>(L, 1);
    size_t length;
    std::string_view key = luaL_checklstring(L, 2, &length);
    switch (key[0]) {
        case 'x':
            lua_pushnumber(L, r.x);
            return 1;
        case 'y':
            lua_pushnumber(L, r.y);
            return 1;
        case 'w':
            lua_pushnumber(L, r.w);
            return 1;
        case 'h':
            lua_pushnumber(L, r.h);
            return 1;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
};
static int newindex(lua_State* L) {
    auto& r = check<Rect>(L, 1);
    using sv = std::string_view;
    size_t length;
    char initial = *luaL_checklstring(L, 2, &length);
    int v = luaL_checknumber(L, 3);
    switch (initial) {
        case 'x':
            r.x = v;
            return 0;
        case 'y':
            r.y = v;
            return 0;
        case 'w':
            r.w = v;
            return 0;
        case 'h': 
            r.h = v;
            return 0;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
}
static int ctor(lua_State* L) {
    double x = luaL_optnumber(L, 1, 0);
    double y = luaL_optnumber(L, 2, 0);
    double w = luaL_optnumber(L, 3, 0);
    double h = luaL_optnumber(L, 4, 0);
    create<Rect>(L, x, y, w, h);
    return 1;
}
namespace builtin {
void register_rect_type(lua_State* L) {
    luaL_newmetatable(L, metatable_name<Rect>());
    const luaL_Reg meta[] = {
        {metamethod::index, index},
        {metamethod::newindex, newindex},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, meta);
    lua_pushstring(L, type);
    lua_setfield(L, -2, metamethod::type);
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, "rectangle_ctor");
    lua_setglobal(L, type);
}
}
