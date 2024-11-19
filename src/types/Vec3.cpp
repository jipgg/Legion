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
static constexpr auto type = "Vec3";
using builtin::Vec3;

static int ctor(lua_State *L) {
    const double x = luaL_optnumber(L, 1, 0);
    const double y = luaL_optnumber(L, 2, 0);
    const double z = luaL_optnumber(L, 3, 0);
    create<Vec3>(L) = {x, y, z};
    return 1;
}
static int add(lua_State* L) {
    create<Vec3>(L) = check<Vec3>(L, 1) + check<Vec3>(L, 2);
    return 1;
}
static int index(lua_State *L) {
    const char index = *luaL_checkstring(L, 2);
    const auto& self = check<Vec3>(L, 1);
    switch (index) {
        case 'X': lua_pushnumber(L, self.at(0)); return 1;
        case 'Y': lua_pushnumber(L, self.at(1)); return 1;
        case 'Z': lua_pushnumber(L, self.at(2)); return 1;
        default: return lua_err::invalid_member(L, type);
    }
}
static int newindex(lua_State *L) {
    const double n = luaL_checknumber(L, 3);
    auto& self = check<Vec3>(L, 1);
    switch (*luaL_checkstring(L, 2)) {
        case 'X': luaL_error(L, "Property 'X' is readonly."); return 0;
        case 'Y': luaL_error(L, "Property 'Y' is readonly"); return 0;
        case 'Z': luaL_error(L, "Property 'Z' is readonly."); return 0;
        default: return lua_err::invalid_member(L, type);
    }
    return lua_err::invalid_type(L);
}
static int mul(lua_State *L) {
    assert(lua_isnumber(L, 2));
    double scalar = luaL_checknumber(L, 2);
    create<Vec3>(L) = check<Vec3>(L, 1) * scalar;
    return 1;
}
static int tostring(lua_State *L) {
    const auto& self = check<Vec3>(L, 1);
    double x_v = self.at(0);
    double y_v = self.at(1);
    double z_v = self.at(2);
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    std::string z = std::to_string(z_v);
    if (std::floor(x_v) == x_v) x.erase(x.find('.'));
    if (std::floor(y_v) == y_v) y.erase(y.find('.'));
    if (std::floor(z_v) == z_v) z.erase(z.find('.'));
    std::string str = type + ": {"s + x + ", " + y + ", " + z + "}";
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
    create<Vec3>(L) = -check<Vec3>(L, 1);
    return 1;
}
static int namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    auto& self = check<Vec3>(L, 1);
    using la = lua_atom;
    switch(static_cast<la>(atom)) {
        case la::dot: {
            const double dot = blaze::dot(check<Vec3>(L, 1), check<Vec3>(L, 2));
            lua_pushnumber(L, dot);
            return 1;
        }
        case la::normalized: {
            create<Vec3>(L) = blaze::normalize(check<Vec3>(L, 1));
            return 1;
        }
        case la::abs: {
            create<Vec3>(L) = blaze::abs(check<Vec3>(L, 1));
            return 0;
        }
        case la::norm: {
            auto& r = check<Vec3>(L, 1);
            lua_pushnumber(L, blaze::length(r));
            return 1;
        };
        default: luaL_error(L, "invalid method name");
    }
    return 0;
}
namespace builtin {
void register_vec3_type(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<Vec3>())) {
        const luaL_Reg meta[] = {
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
