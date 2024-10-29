#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "builtin/method_atom.h"
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
using field = bi::vec2_field;
using type = cm::vec2i;
using method = bi::method_atom;
static constexpr auto type_name = "vec2i";
static constexpr auto field_count = comptime::count<field, field::y>();
static type& self(lua_State* L) {return bi::check<type>(L, 1);}
static type& init_t(lua_State* L) {return bi::create<type>(L);}
static bool type_t(lua_State* L, int idx) {return bi::is_type<type>(L, idx);}
static type& ref_t(lua_State* L, int idx) {return bi::check<type>(L, idx);}
static type& other(lua_State* L) {return bi::check<type>(L, 2);}

static int vec2i_ctor(lua_State *L) {
    int x{}, y{};
    if (lua_isnumber(L, 1)) x = luaL_checkinteger(L, 1);
    if (lua_isnumber(L, 2)) y = luaL_checkinteger(L, 2);
    init_t(L) = {x, y};
    return 1;
}
static int vec2i_add(lua_State* L) {
    init_t(L) = self(L) + other(L);
    return 1;
}
static int vec2i_index(lua_State *L) {
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
static int vec2i_newindex(lua_State *L) {
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
static int vec2i_mul(lua_State *L) {
    assert(lua_isnumber(L, 2));
    int scalar = luaL_checknumber(L, 2);
    init_t(L) = self(L) * scalar;
    return 1;
}
static int vec2i_tostring(lua_State *L) {
    int x_v = self(L).at(0);
    int y_v = self(L).at(1);
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    std::string str = type_name + "{"s + x + ", " + y + "}";
    lua_pushlstring(L, str.data(), str.size());
    return 1;
}
static int vec2i_sub(lua_State* L) {
    return 0;
}
static int vec2i_div(lua_State *L) {
    return 0;
}
static int vec2i_unm(lua_State* L) {
    init_t(L) = -self(L);
    return 1;
}
static int vec2i_namecall(lua_State *L) {
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
void bi::vec2i_init_type(lua_State* L) {
    luaL_newmetatable(L, metatable_name<type>());
    namespace mm = metamethod;
    const luaL_Reg metadata [] = {
        {mm::index, vec2i_index},
        {mm::add, vec2i_add},
        {mm::mul, vec2i_mul},
        {mm::unm, vec2i_unm},
        {mm::div, vec2i_div},
        {mm::sub, vec2i_sub},
        {mm::namecall, vec2i_namecall},
        {mm::newindex, vec2i_newindex},
        {mm::tostring, vec2i_tostring},
        {nullptr, nullptr}
    };
    lua_pushstring(L, type_name);
    lua_setfield(L, -2, mm::type);
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, vec2i_ctor, type_name);
    lua_setglobal(L, type_name);
}
