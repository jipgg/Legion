#include "builtin.h"
#include "builtin_types.h"
#include "lua_util.h"
#include "lua_atom.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
using namespace std::string_literals;
static constexpr auto type = "Vec2";
using builtin::Vec2;

static int ctor(lua_State *L) {
    if (lua_isnone(L, 1)) {//default constructor
        create<Vec2>(L) = {};
        return 1;
    } else if (is_type<Vec2>(L, 1)) {//copy constructor
        auto& v = check<Vec2>(L, 1);
        create<Vec2>(L) = v;
        return 1;
    } else if (lua_isnumber(L, 1)) {
        create<Vec2>(L) = {luaL_checknumber(L, 1), luaL_optnumber(L, 2, 0)};
        return 1;
    } else if (lua_istable(L, 1)) {//from table
        double x{}, y{};
        if (lua_rawgeti(L, 1, 1) == LUA_TNUMBER) {
            x = luaL_checknumber(L, -1);
        }
        lua_pop(L, 1);
        if (lua_rawgeti(L, 1, 2) == LUA_TNUMBER) {
            y = luaL_checknumber(L, -1);
        }
        lua_pop(L, 1);
        if (lua_rawgetfield(L, 1, "x") == LUA_TNUMBER) {
            x = luaL_checknumber(L, -1);
        }
        lua_pop(L, 1);
        if (lua_rawgetfield(L, 1, "y") == LUA_TNUMBER) {
            y = luaL_checknumber(L, -1);
        }
        lua_pop(L, 1);
        create<Vec2>(L) = {x, y};
        return 1;
    }
    luaL_error(L, "invalid initializer arguments");
    return 0;
}
static int add(lua_State* L) {
    const auto& self = check<Vec2>(L, 1);
    const auto& other = check<Vec2>(L, 2);
    create<Vec2>(L) = self + other;
    return 1;
}
static int index(lua_State *L) {
    const char index = *luaL_checkstring(L, 2);
    auto& r = check<Vec2>(L, 1);
    switch (index) {
        case 'X': lua_pushnumber(L, r[0]); return 1;
        case 'Y': lua_pushnumber(L, r[1]); return 1;
    }
    return 0;
}
static int newindex(lua_State *L) {
    const double n = luaL_checknumber(L, 3);
    Vec2& r = check<Vec2>(L, 1);
    switch (*luaL_checkstring(L, 2)) {
        case 'X': luaL_error(L, "property 'X' is readonly."); return 0;
        case 'Y': luaL_error(L, "Property 'Y' is readonly."); return 0;
        default: return lua_err::invalid_member(L, type);
    }
    return 0;
}
static int mul(lua_State *L) {
    double scalar = luaL_checknumber(L, 2);
    create<Vec2>(L) = check<Vec2>(L, 1) * scalar;
    return 1;
}
static int tostring(lua_State *L) {
    Vec2& r = check<Vec2>(L, 1);
    double x_v = r[0];
    double y_v = r[1];
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    if (std::floor(x_v) == x_v) x.erase(x.find('.'));
    if (std::floor(y_v) == y_v) y.erase(y.find('.'));
    std::string str = type + ": {"s + x + ", " + y + "}";
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
    create<Vec2>(L) = -check<Vec2>(L, 1);
    return 1;
}
static int namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    auto& self = check<Vec2>(L, 1);
    using la = lua_atom;
    switch(static_cast<la>(atom)) {
        case la::DotProduct:
            lua_pushnumber(L, blaze::dot(self, check<Vec2>(L, 2)));
            return 1;
        case la::ToUnitVector:
            create<Vec2>(L) = self / blaze::length(self);
            return 1;
        case la::Abs:
            create<Vec2>(L) = blaze::abs(self);
            return 1;
        case la::Length:
            lua_pushnumber(L, blaze::length(self));
            return 1;
        default:
        return 0;
    }
}
namespace builtin {
void register_vec2_type(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<Vec2>())) {
        const luaL_Reg meta [] = {
            {metamethod::index, index},
            {metamethod::add, add},
            {metamethod::mul, mul},
            {metamethod::unm, unm},
            {metamethod::div, div},
            {metamethod::sub, sub},
            {metamethod::namecall, namecall},
            {metamethod::newindex, newindex},
            {metamethod::tostring, tostring},
            {nullptr, nullptr}
        };
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
        luaL_register(L, nullptr, meta);
    }
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, type);
    lua_setglobal(L, type);
}

}
