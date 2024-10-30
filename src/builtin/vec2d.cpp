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
#include "common/comptime.h"
using namespace std::string_literals;
namespace bi = builtin;
namespace cm = common;
using method = bi::method_atom;
using field = bi::vec2_field;
static constexpr auto type_name = "V2";
static constexpr auto field_count = comptime::count<field, field::y>();
using type = bi::vec2d_t;
static type& self(lua_State* L) {
    return bi::check<type>(L, 1);
}
static type& init_t(lua_State* L) {
    return bi::create<type>(L);
}
static bool type_t(lua_State* L, int idx) {
    return bi::is_type<type>(L, idx);
}
static type& ref_t(lua_State* L, int idx) {
    return bi::check<type>(L, idx);
}
static type& other(lua_State* L) {
    return bi::check<type>(L, 2);
}
static int ctor(lua_State *L) {
    if (lua_isnone(L, 2) and bi::is_type<bi::vec2i_t>(L, 1)) {
        auto& v = bi::check<bi::vec2i_t>(L, 1);
        bi::create<bi::vec2d_t>(L) = {double(v.at(0)), double(v.at(1))};
        return 1;
    }
    double x{}, y{};
    if (lua_isnumber(L, 1)) x = luaL_checknumber(L, 1);
    if (lua_isnumber(L, 2)) y = luaL_checknumber(L, 2);
    init_t(L) = {x, y};
    return 1;
}
static int add(lua_State* L) {
    init_t(L) = self(L) + other(L);
    return 1;
}
static int index(lua_State *L) {
    static constexpr auto array = comptime::to_array<field, field_count>();
    const std::string_view field_name = luaL_checkstring(L, 2);
    for (const auto& v : array) {
        if (v.name == field_name) {
            switch (static_cast<field>(v.index)) {
                case field::x:
                    lua_pushnumber(L, self(L).at(0));
                    break;
                case field::y:
                    lua_pushnumber(L, self(L).at(1));
                    break;
            }
            return 1;
        }
    }
    common::printerr("invalid index");
    return 0;
}
static int newindex(lua_State *L) {
    static constexpr auto array = comptime::to_array<field, field_count>();
    const std::string_view field_name = luaL_checkstring(L, 2);
    const double n = luaL_checknumber(L, 3);
    for (const auto& v : array) {
        if (v.name == field_name) {
            switch (static_cast<field>(v.index)) {
                case field::x:
                    self(L).at(0) = n;
                    return 0;
                case field::y:
                    self(L).at(1) = n;
                    return 0;
            }
        }
    }
    return 0;
}
static int mul(lua_State *L) {
    assert(lua_isnumber(L, 2));
    double scalar = luaL_checknumber(L, 2);
    init_t(L) = self(L) * scalar;
    return 1;
}
static int tostring(lua_State *L) {
    double x_v = self(L).at(0);
    double y_v = self(L).at(1);
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    if (std::floor(x_v) == x_v) x.erase(x.find('.'));
    if (std::floor(y_v) == y_v) y.erase(y.find('.'));

    std::string str = type_name + ": {"s + x + ", " + y + "}";
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
    init_t(L) = -self(L);
    return 1;
}
static int namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    switch(static_cast<method>(atom)) {
        case method::dot:
            assert(type_t(L, 2));
            lua_pushnumber(L, blaze::dot(self(L), other(L)));
            return 1;
        case method::unit:
            init_t(L) = self(L) / blaze::length(self(L));
            return 1;
        case method::abs:
            init_t(L) = blaze::abs(self(L));
            return 1;
        case method::magnitude:
            lua_pushnumber(L, blaze::length(self(L)));
            return 1;
        default:
        return 0;
    }
}
void bi::vec2d_init_type(lua_State* L) {
    luaL_newmetatable(L, metatable_name<type>());
    namespace mm = metamethod;
    const luaL_Reg metadata [] = {
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
    lua_pushstring(L, type_name);
    lua_setfield(L, -2, mm::type);
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, type_name);
    lua_setglobal(L, type_name);
}
