#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "builtin/method_atom.h"
#include "builtin/typedefs.h"
#include "common/common.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
using namespace std::string_literals;
static constexpr auto v3_tname = "V3"; 
static constexpr auto v2_tname = "V2";
namespace bi = builtin;
namespace mm = bi::metamethod;
using method = bi::method_atom;
using bi::vec2d_t;
using bi::vec2i_t;
using bi::vec3d_t;
static int lua_err_invalid_index(lua_State* L, const char* tname) {
    constexpr auto err_index_msg = "invalid %s member '%s'.";
    luaL_error(L, err_index_msg, tname, luaL_checkstring(L, 2));
    return 0;
}
static int v3_ctor(lua_State *L) {
    const double x = luaL_optnumber(L, 1, 0);
    const double y = luaL_optnumber(L, 2, 0);
    const double z = luaL_optnumber(L, 3, 0);
    bi::create<vec3d_t>(L) = {x, y, z};
    return 1;
}
static int v3_add(lua_State* L) {
    bi::create<vec3d_t>(L) = bi::check<vec3d_t>(L, 1) + bi::check<vec3d_t>(L, 2);
    return 1;
}
static int v3_index(lua_State *L) {
    const char field_name = *luaL_checkstring(L, 2);
    const auto& self = bi::check<vec3d_t>(L, 1);
    switch (field_name) {
        case 'x': lua_pushnumber(L, self.at(0)); return 1;
        case 'y': lua_pushnumber(L, self.at(1)); return 1;
        case 'z': lua_pushnumber(L, self.at(2)); return 1;
    }
    lua_err_invalid_index(L, v3_tname);
    return 0;
}
static int v3_newindex(lua_State *L) {
    const double n = luaL_checknumber(L, 3);
    auto& self = bi::check<vec3d_t>(L, 1);
    switch (*luaL_checkstring(L, 2)) {
        case 'x': self.at(0) = n; return 0;
        case 'y': self.at(1) = n; return 0;
        case 'z': self.at(2) = n; return 0;
    }
    return 0;
}
static int v3_mul(lua_State *L) {
    assert(lua_isnumber(L, 2));
    double scalar = luaL_checknumber(L, 2);
    bi::create<vec3d_t>(L) = bi::check<vec3d_t>(L, 1) * scalar;
    return 1;
}
static int v3_tostring(lua_State *L) {
    common::print("tostring", bi::check<vec3d_t>(L, 1).at(0));
    const auto& self = bi::check<vec3d_t>(L, 1);
    double x_v = self.at(0);
    double y_v = self.at(1);
    double z_v = self.at(2);
    common::print("cpp:", self.at(0), self.at(1), self.at(2), x_v, y_v, z_v);
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    std::string z = std::to_string(z_v);
    //if (std::floor(x_v) == x_v) x.erase(x.find('.'));
    //if (std::floor(y_v) == y_v) y.erase(y.find('.'));
    //if (std::floor(z_v) == z_v) z.erase(z.find('.'));
    std::string str = v3_tname + ": {"s + x + ", " + y + ", " + z + "}";
    common::print(str);
    lua_pushstring(L, str.c_str());
    return 1;
}
static int v3_sub(lua_State* L) {
    return 0;
}
static int v3_div(lua_State *L) {
    return 0;
}
static int v3_unm(lua_State* L) {
    bi::create<vec3d_t>(L) = -bi::check<vec3d_t>(L, 1);
    return 1;
}
inline static int v3_unit(lua_State* L) {
    vec3d_t to_unit = bi::check<vec3d_t>(L, 1);
    to_unit = blaze::normalize(to_unit);
    bi::create<vec3d_t>(L) = blaze::normalize(bi::check<vec2d_t>(L, 1));
    return 1;
}
inline static int v3_abs(lua_State* L) {
    bi::create<vec3d_t>(L) = blaze::abs(bi::check<vec3d_t>(L, 1));
    return 0;
}
inline static int v3_magnitude(lua_State* L) {
    auto& r = bi::check<vec3d_t>(L, 1);
    common::printerr(r[0], r[1], r[2], r.size(), r.capacity());
    lua_pushnumber(L, blaze::length(r));
    return 1;
}
inline static int v3_dot(lua_State* L) {
    double dot = blaze::dot(bi::check<vec3d_t>(L, 1), bi::check<vec3d_t>(L, 2));
    lua_pushnumber(L, dot);
    return 1;
}
static int v3_namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    switch(static_cast<method>(atom)) {
        case method::dot: return v3_dot(L);
        case method::unit: return v3_unit(L);
        case method::abs: return v3_abs(L);
        case method::magnitude: return v3_magnitude(L);
        default: luaL_error(L, "invalid method name");
    }
    return 0;
}
static int v2_ctor(lua_State *L) {
    if (lua_isnone(L, 2) and bi::is_type<vec2i_t>(L, 1)) {
        auto& v = bi::check<vec2i_t>(L, 1);
        bi::create<vec2d_t>(L) = {double(v.at(0)), double(v.at(1))};
        return 1;
    }
    double x{}, y{};
    if (lua_isnumber(L, 1)) x = luaL_checknumber(L, 1);
    if (lua_isnumber(L, 2)) y = luaL_checknumber(L, 2);
    bi::create<vec2d_t>(L) = {x, y};
    return 1;
}
static int v2_add(lua_State* L) {
    const auto& self = bi::check<vec2d_t>(L, 1);
    const auto& other = bi::check<vec2d_t>(L, 2);
    bi::create<vec2d_t>(L) = self + other;
    return 1;
}
static int v2_index(lua_State *L) {
    const char field_name = *luaL_checkstring(L, 2);
    switch (field_name) {
        case 'x':
        lua_pushnumber(L, bi::check<vec2d_t>(L, 1).at(0));
        break;
    case 'y':
        lua_pushnumber(L, bi::check<vec2d_t>(L, 1).at(1));
        break;
    }
    common::printerr("invalid index");
    return 0;
}
static int v2_newindex(lua_State *L) {
    const double n = luaL_checknumber(L, 3);
    switch (*luaL_checkstring(L, 2)) {
        case 'x':
            bi::check<vec2d_t>(L, 1).at(0) = n;
            return 0;
        case 'y':
            bi::check<vec2d_t>(L, 1).at(1) = n;
            return 0;
    }
    return 0;
}
static int v2_mul(lua_State *L) {
    assert(lua_isnumber(L, 2));
    double scalar = luaL_checknumber(L, 2);
    bi::create<vec2d_t>(L) = bi::check<vec2d_t>(L, 1) * scalar;
    return 1;
}
static int v2_tostring(lua_State *L) {
    double x_v = bi::check<vec2d_t>(L, 1).at(0);
    double y_v = bi::check<vec2d_t>(L, 1).at(1);
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    if (std::floor(x_v) == x_v) x.erase(x.find('.'));
    if (std::floor(y_v) == y_v) y.erase(y.find('.'));

    std::string str = v2_tname + ": {"s + x + ", " + y + "}";
    lua_pushlstring(L, str.data(), str.size());
    return 1;
}
static int v2_sub(lua_State* L) {
    return 0;
}
static int v2_div(lua_State *L) {
    return 0;
}
static int v2_unm(lua_State* L) {
    bi::create<vec2d_t>(L) = -bi::check<vec2d_t>(L, 1);
    return 1;
}
static int v2_namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    auto& self = bi::check<vec2d_t>(L, 1);
    switch(static_cast<method>(atom)) {
        case method::dot:
            lua_pushnumber(L, blaze::dot(self, bi::check<vec2d_t>(L, 2)));
            return 1;
        case method::unit:
            bi::create<vec2d_t>(L) = self / blaze::length(self);
            return 1;
        case method::abs:
            bi::create<vec2d_t>(L) = blaze::abs(self);
            return 1;
        case method::magnitude:
            lua_pushnumber(L, blaze::length(self));
            return 1;
        default:
        return 0;
    }
}
static void v3_init_type(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<vec3d_t>());
    const luaL_Reg v3_meta[] = {
        {mm::index, v3_index},
        {mm::add, v3_add},
        {mm::mul, v3_mul},
        {mm::unm, v3_unm},
        {mm::div, v3_div},
        {mm::sub, v3_sub},
        {mm::namecall, v3_namecall},
        {mm::newindex, v3_newindex},
        {mm::tostring, v3_tostring},
        {nullptr, nullptr}
    };
    lua_pushstring(L, v3_tname);
    lua_setfield(L, -2, mm::type);
    luaL_register(L, nullptr, v3_meta);
    lua_pop(L, 1);
    lua_pushcfunction(L, v3_ctor, v3_tname);
    lua_setglobal(L, v3_tname);
}
void bi::vec2d_init_type(lua_State* L) {
    v3_init_type(L);
    luaL_newmetatable(L, metatable_name<vec2d_t>());
    const luaL_Reg v2_meta [] = {
        {mm::index, v2_index},
        {mm::add, v2_add},
        {mm::mul, v2_mul},
        {mm::unm, v2_unm},
        {mm::div, v2_div},
        {mm::sub, v2_sub},
        {mm::namecall, v2_namecall},
        {mm::newindex, v2_newindex},
        {mm::tostring, v2_tostring},
        {nullptr, nullptr}
    };
    lua_pushstring(L, v2_tname);
    lua_setfield(L, -2, mm::type);
    luaL_register(L, nullptr, v2_meta);
    lua_pop(L, 1);
    lua_pushcfunction(L, v2_ctor, v2_tname);
    lua_setglobal(L, v2_tname);
}
