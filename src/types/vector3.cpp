#include "builtin.h"
#include "lua_util.h"
#include "lua_atom.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
using namespace std::string_literals;
namespace bi = builtin;
using bi::vector3;
static constexpr auto tn = bi::tname::vector3;

static int ctor(lua_State *L) {
    const double x = luaL_optnumber(L, 1, 0);
    const double y = luaL_optnumber(L, 2, 0);
    const double z = luaL_optnumber(L, 3, 0);
    create<vector3>(L) = {x, y, z};
    return 1;
}
static int add(lua_State* L) {
    create<vector3>(L) = check<vector3>(L, 1) + check<vector3>(L, 2);
    return 1;
}
static int index(lua_State *L) {
    const char index = *luaL_checkstring(L, 2);
    const auto& self = check<vector3>(L, 1);
    switch (index) {
        case 'x': lua_pushnumber(L, self.at(0)); return 1;
        case 'y': lua_pushnumber(L, self.at(1)); return 1;
        case 'z': lua_pushnumber(L, self.at(2)); return 1;
        default: return lua_err::invalid_member(L, tn);
    }
}
static int newindex(lua_State *L) {
    const double n = luaL_checknumber(L, 3);
    auto& self = check<vector3>(L, 1);
    if (lua_isstring(L, 2)) {
        switch (*luaL_checkstring(L, 2)) {
            case 'x': self.at(0) = n; return 0;
            case 'y': self.at(1) = n; return 0;
            case 'z': self.at(2) = n; return 0;
            default: return lua_err::invalid_member(L, tn);
        }
    } else if (lua_isnumber(L, 2)) {
        const int index = luaL_checkinteger(L, 2);
        if (index >= self.size() or index < 0) return lua_err::out_of_range(L, tn);
        self[index] = n;
        return 0;
    }
    return lua_err::invalid_type(L);
}
static int mul(lua_State *L) {
    assert(lua_isnumber(L, 2));
    double scalar = luaL_checknumber(L, 2);
    create<vector3>(L) = check<vector3>(L, 1) * scalar;
    return 1;
}
static int tostring(lua_State *L) {
    const auto& self = check<vector3>(L, 1);
    double x_v = self.at(0);
    double y_v = self.at(1);
    double z_v = self.at(2);
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    std::string z = std::to_string(z_v);
    if (std::floor(x_v) == x_v) x.erase(x.find('.'));
    if (std::floor(y_v) == y_v) y.erase(y.find('.'));
    if (std::floor(z_v) == z_v) z.erase(z.find('.'));
    std::string str = tn + ": {"s + x + ", " + y + ", " + z + "}";
    lua_pushstring(L, str.c_str());
    return 1;
}
static int sub(lua_State* L) {
    return 0;
}
static int div(lua_State *L) {
    return 0;
}
static int unm(lua_State* L) {
    create<vector3>(L) = -check<vector3>(L, 1);
    return 1;
}
static int namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    auto& self = check<vector3>(L, 1);
    using la = lua_atom;
    switch(static_cast<la>(atom)) {
        case la::DotProduct: {
            const double dot = blaze::dot(check<vector3>(L, 1), check<vector3>(L, 2));
            lua_pushnumber(L, dot);
            return 1;
        }
        case la::ToUnitVector: {
            create<vector3>(L) = blaze::normalize(check<vector3>(L, 1));
            return 1;
        }
        case la::Abs: {
            create<vector3>(L) = blaze::abs(check<vector3>(L, 1));
            return 0;
        }
        case la::Length: {
            auto& r = check<vector3>(L, 1);
            lua_pushnumber(L, blaze::length(r));
            return 1;
        };
        default: luaL_error(L, "invalid method name");
    }
    return 0;
}
int builtin::class_vector3(lua_State* L) {
    namespace mm = bi::metamethod;
    if (luaL_newmetatable(L, metatable_name<vector3>())) {
        const luaL_Reg meta[] = {
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
    lua_pushstring(L, tn);
    lua_setfield(L, -2, mm::type);
    luaL_register(L, nullptr, meta);

    }
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, tn);
    return 1;
}
