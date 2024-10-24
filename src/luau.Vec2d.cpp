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
static constexpr auto field_count = comptime::count<luau::Vec2d::Field, luau::Vec2d::Field::y>();
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
void luau::Vec2d::init_type(lua_State* L) {
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
int luau::Vec2d::ctor(lua_State *L) {
    double x{}, y{};
    if (lua_isnumber(L, 1)) x = luaL_checknumber(L, 1);
    if (lua_isnumber(L, 2)) y = luaL_checknumber(L, 2);
    init_t(L) = {x, y};
    lua_callbacks(L)->useratom = [](const char* name, size_t idk) {
        static constexpr auto count = comptime::count<Method, Method::magnitude>();
        return static_cast<int16_t>(comptime::enum_item<Method, count>(name).index);
    };
    return 1;
}
int luau::Vec2d::add(lua_State* L) {
    init_t(L) = self(L) + other(L);
    return 1;
}
int luau::Vec2d::index(lua_State *L) {
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
int luau::Vec2d::newindex(lua_State *L) {
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
int luau::Vec2d::mul(lua_State *L) {
    assert(lua_isnumber(L, 2));
    double scalar = luaL_checknumber(L, 2);
    init_t(L) = self(L) * scalar;
    return 1;
}
int luau::Vec2d::tostring(lua_State *L) {
    std::string str = type_name + "{"s
        + std::to_string(self(L).at(0)) + ", "
        + std::to_string(self(L).at(1)) + "}";
    lua_pushlstring(L, str.data(), str.size());
    return 1;
}
int luau::Vec2d::sub(lua_State* L) {
    return 0;
}
int luau::Vec2d::div(lua_State *L) {
    return 0;
}
int luau::Vec2d::unm(lua_State* L) {
    init_t(L) = -self(L);
    return 1;
}
int luau::Vec2d::namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    switch(static_cast<Method>(atom)) {
        case Method::dot:
            assert(type_t(L, 2));
            lua_pushnumber(L, blaze::dot(self(L), other(L)));
            return 1;
        case Method::unit:
            init_t(L) = self(L) / blaze::length(self(L));
            return 1;
        case Method::abs:
            init_t(L) = blaze::abs(self(L));
            return 1;
        case Method::magnitude:
            lua_pushnumber(L, blaze::length(self(L)));
            return 1;
    }
    return 0;
}
