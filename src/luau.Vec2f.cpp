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
using Self = common::Vec2f;
static constexpr auto field_count = comptime::count<luau::Vec2f::Field, luau::Vec2f::Field::y>();
void luau::Vec2f::init_type(lua_State* L) {
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
int luau::Vec2f::ctor(lua_State *L) {
    float x{}, y{};
    if (lua_isnumber(L, 1)) x = luaL_checknumber(L, 1);
    if (lua_isnumber(L, 2)) y = luaL_checknumber(L, 2);
    init<Self>(L) = {x, y};
    lua_callbacks(L)->useratom = [](const char* name, size_t idk) {
        static constexpr auto count = comptime::count<Method, Method::magnitude>();
        return static_cast<int16_t>(comptime::enum_item<Method, count>(name).index);
    };
    return 1;
}
int luau::Vec2f::add(lua_State* L) {
    const auto& a = ref<Self>(L, 1);
    const auto& b = ref<Self>(L, 2);
    auto& v = init<Self>(L);
    v = a + b;
    return 1;
}
int luau::Vec2f::index(lua_State *L) {
    static constexpr auto array = comptime::to_array<Field, field_count>();
    auto& self = ref<Self>(L, 1);
    const std::string_view field_name = luaL_checkstring(L, 2);
    for (const auto& v : array) {
        if (v.name == field_name) {
            switch (static_cast<Field>(v.index)) {
                case Field::x:
                    lua_pushnumber(L, self.at(0));
                    break;
                case Field::y:
                    lua_pushnumber(L, self.at(1));
                    break;
            }
            return 1;
        }
    }
    common::printerr("invalid index");
    return 0;
}
int luau::Vec2f::newindex(lua_State *L) {
    static constexpr auto array = comptime::to_array<Field, field_count>();
    auto& self = ref<Self>(L, 1);
    const std::string_view field_name = luaL_checkstring(L, 2);
    const double n = luaL_checknumber(L, 3);
    for (const auto& v : array) {
        if (v.name == field_name) {
            switch (static_cast<Field>(v.index)) {
                case Field::x:
                    self.at(0) = n;
                    return 0;
                case Field::y:
                    self.at(1) = n;
                    return 0;
            }
        }
    }
    return 0;
}
int luau::Vec2f::mul(lua_State *L) {
    const auto& a = ref<Self>(L, 1);
    assert(lua_isnumber(L, 2));
    double scalar = luaL_checknumber(L, 2);
    init<Self>(L) = a * scalar;
    return 1;
}
int luau::Vec2f::tostring(lua_State *L) {
    auto& self = ref<Self>(L, 1);
    std::string str = type_name + "{"s + std::to_string(self.at(0)) + ", " + std::to_string(self.at(1)) + "}";
    lua_pushlstring(L, str.data(), str.size());
    return 1;
}
int luau::Vec2f::sub(lua_State* L) {
    return 0;
}
int luau::Vec2f::div(lua_State *L) {
    return 0;
}
int luau::Vec2f::unm(lua_State* L) {
    init<Self>(L) = -ref<Self>(L, 1);
    return 1;
}
int luau::Vec2f::namecall(lua_State *L) {
    auto& self = ref<Self>(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    switch(static_cast<Method>(atom)) {
        case Method::dot:
            assert(is_type<Self>(L, 2));
            lua_pushnumber(L, blaze::dot(self, ref<Self>(L, 2)));
            return 1;
        case Method::unit:
            init<Self>(L) = self / blaze::length(self);
            return 1;
        case Method::abs:
            init<Self>(L) = blaze::abs(self);
            return 1;
        case Method::magnitude:
            lua_pushnumber(L, blaze::length(self));
            return 1;
    }
    return 0;
}
