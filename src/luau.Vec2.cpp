#include "luau.defs.h"
#include "common.h"
#include "luau.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include "comptime.h"
using namespace std::string_literals;
static constexpr auto field_count = comptime::count<luau::Vec2::Field, luau::Vec2::Field::y>();
using Self = common::Vec2d;
static Self& self(lua_State* L) {
    return luau::ref<Self>(L, 1);
}
static Self& init_t(lua_State* L) {
    return luau::init<Self>(L);
}
static bool type_t(lua_State* L, int idx) {
    return luau::is_type<Self>(L, idx);
}
static Self& ref_t(lua_State* L, int idx) {
    return luau::ref<Self>(L, idx);
}
static Self& other(lua_State* L) {
    return luau::ref<Self>(L, 2);
}
void luau::Vec2::init_type(lua_State* L) {
    luaL_newmetatable(L, metatable_name<Self>());
    const luaL_Reg metadata [] = {
        {"__index", index},
        {"__add", add},
        {"__mul", mul},
        {"__unm", unm},
        {"__div", div},
        {"__sub", sub},
        {"__namecall", namecall},
        {"__newindex", newindex},
        {"__tostring", tostring},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, type_name);
    lua_setglobal(L, type_name);
}
int luau::Vec2::ctor(lua_State *L) {
    double x{}, y{};
    if (lua_isnumber(L, 1)) x = luaL_checknumber(L, 1);
    if (lua_isnumber(L, 2)) y = luaL_checknumber(L, 2);
    init_t(L) = {x, y};
    /*
    lua_callbacks(L)->useratom = [](const char* raw_name, size_t s) {
        std::string_view name = std::string_view(raw_name, s);
        static constexpr auto count = comptime::count<Method, Method::magnitude>();
        auto e = comptime::enum_item<Method, count>(name);
        const auto v = static_cast<int16_t>(e.index);
        return v;
    };
    */
    return 1;
}
int luau::Vec2::add(lua_State* L) {
    init_t(L) = self(L) + other(L);
    return 1;
}
int luau::Vec2::index(lua_State *L) {
    static constexpr auto array = comptime::to_array<Field, field_count>();
    const std::string_view field_name = luaL_checkstring(L, 2);
    for (const auto& v : array) {
        if (v.name == field_name) {
            switch (static_cast<Field>(v.index)) {
                case Field::x:
                    lua_pushnumber(L, self(L).at(0));
                    break;
                case Field::y:
                    lua_pushnumber(L, self(L).at(1));
                    break;
            }
            return 1;
        }
    }
    common::printerr("invalid index");
    return 0;
}
int luau::Vec2::newindex(lua_State *L) {
    static constexpr auto array = comptime::to_array<Field, field_count>();
    const std::string_view field_name = luaL_checkstring(L, 2);
    const double n = luaL_checknumber(L, 3);
    for (const auto& v : array) {
        if (v.name == field_name) {
            switch (static_cast<Field>(v.index)) {
                case Field::x:
                    self(L).at(0) = n;
                    return 0;
                case Field::y:
                    self(L).at(1) = n;
                    return 0;
            }
        }
    }
    return 0;
}
int luau::Vec2::mul(lua_State *L) {
    assert(lua_isnumber(L, 2));
    double scalar = luaL_checknumber(L, 2);
    init_t(L) = self(L) * scalar;
    return 1;
}
int luau::Vec2::tostring(lua_State *L) {
    double x_v = self(L).at(0);
    double y_v = self(L).at(1);
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    if (std::floor(x_v) == x_v) x.erase(x.find('.'));
    if (std::floor(y_v) == y_v) y.erase(y.find('.'));

    std::string str = type_name + "{"s + x + ", " + y + "}";
    lua_pushlstring(L, str.data(), str.size());
    return 1;
}
int luau::Vec2::sub(lua_State* L) {
    return 0;
}
int luau::Vec2::div(lua_State *L) {
    return 0;
}
int luau::Vec2::unm(lua_State* L) {
    init_t(L) = -self(L);
    return 1;
}
int luau::Vec2::namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    switch(static_cast<Method_atom>(atom)) {
        case Method_atom::dot:
            assert(type_t(L, 2));
            lua_pushnumber(L, blaze::dot(self(L), other(L)));
            return 1;
        case Method_atom::unit:
            init_t(L) = self(L) / blaze::length(self(L));
            return 1;
        case Method_atom::abs:
            init_t(L) = blaze::abs(self(L));
            return 1;
        case Method_atom::magnitude:
            lua_pushnumber(L, blaze::length(self(L)));
            return 1;
        default:
        return 0;
    }
}
