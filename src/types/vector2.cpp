#include "builtin.h"
#include "lua_util.h"
#include "lua_atom.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
using namespace std::string_literals;
static constexpr auto tname = "Vector2";
namespace bi = builtin;
namespace mm = bi::metamethod;
using bi::vector2;

static int ctor(lua_State *L) {
    if (lua_isnone(L, 2) and is_type<vector2>(L, 1)) {
        auto& v = check<vector2>(L, 1);
        create<vector2>(L) = {double(v.at(0)), double(v.at(1))};
        return 1;
    }
    double x{}, y{};
    if (lua_isnumber(L, 1)) x = luaL_checknumber(L, 1);
    if (lua_isnumber(L, 2)) y = luaL_checknumber(L, 2);
    create<vector2>(L) = {x, y};
    return 1;
}
static int add(lua_State* L) {
    const auto& self = check<vector2>(L, 1);
    const auto& other = check<vector2>(L, 2);
    create<vector2>(L) = self + other;
    return 1;
}
static int index(lua_State *L) {
    const char index = *luaL_checkstring(L, 2);
    auto& r = check<vector2>(L, 1);
    switch (index) {
        case 'x': lua_pushnumber(L, r[0]); return 1;
        case 'y': lua_pushnumber(L, r[1]); return 1;
    }
    return 0;
}
static int newindex(lua_State *L) {
    const double n = luaL_checknumber(L, 3);
    vector2& r = check<vector2>(L, 1);
    switch (*luaL_checkstring(L, 2)) {
        case 'x': r[0] = n; return 0;
        case 'y': r[1] = n; return 0;
        default: return err_invalid_member(L, tname);
    }
    return 0;
}
static int mul(lua_State *L) {
    double scalar = luaL_checknumber(L, 2);
    create<vector2>(L) = check<vector2>(L, 1) * scalar;
    return 1;
}
static int tostring(lua_State *L) {
    vector2& r = check<vector2>(L, 1);
    double x_v = r[0];
    double y_v = r[1];
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    if (std::floor(x_v) == x_v) x.erase(x.find('.'));
    if (std::floor(y_v) == y_v) y.erase(y.find('.'));
    std::string str = tname + ": {"s + x + ", " + y + "}";
    lua_pushlstring(L, str.data(), str.size());
    return 1;
}
static int sub(lua_State* L) {
    return 0;
}
static int div(lua_State *L) {
    return 0;
}
static int unm(lua_State* L) {
    create<vector2>(L) = -check<vector2>(L, 1);
    return 1;
}
static int namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    auto& self = check<vector2>(L, 1);
    using la = lua_atom;
    switch(static_cast<la>(atom)) {
        case la::dot:
            lua_pushnumber(L, blaze::dot(self, check<vector2>(L, 2)));
            return 1;
        case la::normalize:
            create<vector2>(L) = self / blaze::length(self);
            return 1;
        case la::abs:
            create<vector2>(L) = blaze::abs(self);
            return 1;
        case la::magnitude:
            lua_pushnumber(L, blaze::length(self));
            return 1;
        default:
        return 0;
    }
}
int builtin::class_vector2(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<vector2>())) {
        const luaL_Reg meta [] = {
            {mm::index, index},
            {mm::add, add},
            {mm::mul, mul},
            {mm::unm, unm},
            {mm::div, div},
            {mm::sub, sub},
            {mm::namecall, namecall},
            {mm::newindex, newindex},
            {mm::tostring, tostring},
            {nullptr, nullptr}
        };
        lua_pushstring(L, tname);
        lua_setfield(L, -2, mm::type);
        luaL_register(L, nullptr, meta);
    }
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, tname);
    return 1;
}
