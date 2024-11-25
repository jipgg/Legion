#include "builtin.h"
#include "builtin_types.h"
#include "engine.h"
#include "lua_util.h"
using builtin::Recti;
static constexpr auto type = "Recti";

static int index(lua_State* L) {
    auto& r = check<Recti>(L, 1);
    std::string_view key = luaL_checkstring(L, 2);
    switch (key[0]) {
        case 'x':
            lua_pushinteger(L, r.x);
            return 1;
        case 'y':
            lua_pushinteger(L, r.y);
            return 1;
        case 'w':
            lua_pushinteger(L, r.w);
            return 1;
        case 'h':
            lua_pushinteger(L, r.h);
            return 1;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
};
static int newindex(lua_State* L) {
    auto& r = check<Recti>(L, 1);
    using sv = std::string_view;
    size_t length;
    char initial = *luaL_checklstring(L, 2, &length);
    int v = luaL_checkinteger(L, 3);
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
    const int x = luaL_optinteger(L, 1, 0);
    const int y = luaL_optinteger(L, 2, 0);
    const int w = luaL_optinteger(L, 3, 0);
    const int h = luaL_optinteger(L, 4, 0);
    create<Recti>(L, x, y, w, h);
    return 1;
}
namespace builtin {
void register_recti_type(lua_State* L) {
    luaL_newmetatable(L, metatable_name<Recti>());
    const luaL_Reg meta[] = {
        {metamethod::index, index},
        {metamethod::newindex, newindex},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, meta);
    lua_pushstring(L, type);
    lua_setfield(L, -2, metamethod::type);
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, "Recti_ctor");
    lua_setglobal(L, type);
}
}
