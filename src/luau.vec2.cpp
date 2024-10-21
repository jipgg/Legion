#include "legion/luau.types.h"
#include "legion/common.h"
#include "legion/luau.userdata_utility.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include "legion/comptime_enum.h"
namespace ct = comptime;
namespace luau {
void vec2::init_metadata(lua_State* L) {
    luaL_newmetatable(L, metatable_name<Vec2d>());
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
    lua_pushcfunction(L, ctor, "V2");
    lua_setglobal(L, "V2");
}
int vec2::ctor(lua_State *L) {
    double x{}, y{};
    if (lua_isnumber(L, 1)) x = luaL_checknumber(L, 1);
    if (lua_isnumber(L, 2)) y = luaL_checknumber(L, 2);
    init<Vec2d>(L) = {x, y};
    lua_callbacks(L)->useratom = [](const char* name, size_t idk) {
        static constexpr auto count = ct::count<Method, Method::magnitude>();
        return static_cast<int16_t>(ct::enum_item<Method, count>(name).index);
    };
    return 1;
}
int vec2::add(lua_State* L) {
    const auto& a = ref<Vec2d>(L, 1);
    const auto& b = ref<Vec2d>(L, 2);
    auto& v = init<Vec2d>(L);
    v = a + b;
    return 1;
}
static const auto& property_array() {
    namespace v2 = luau::vec2;
    static constexpr auto array = ct::to_array<v2::Property, ct::count<v2::Property, v2::Property::y>()>();
    return array;
}
int vec2::index(lua_State *L) {
    auto& array = property_array();
    auto& self = ref<Vec2d>(L, 1);
    const std::string_view field_name = luaL_checkstring(L, 2);
    for (const auto& v : array) {
        if (v.name == field_name) {
            switch (static_cast<Property>(v.index)) {
                case Property::x:
                    lua_pushnumber(L, self.at(0));
                    break;
                case Property::y:
                    lua_pushnumber(L, self.at(1));
                    break;
            }
            return 1;
        }
    }
    printerr("invalid index");
    return 0;
}
int vec2::newindex(lua_State *L) {
    auto& array = property_array();
    auto& self = ref<Vec2d>(L, 1);
    const std::string_view field_name = luaL_checkstring(L, 2);
    const double n = luaL_checknumber(L, 3);
    for (const auto& v : array) {
        if (v.name == field_name) {
            switch (static_cast<Property>(v.index)) {
                case Property::x:
                    self.at(0) = n;
                    return 0;
                case Property::y:
                    self.at(1) = n;
                    return 0;
            }
        }
    }
    return 0;
}
int vec2::mul(lua_State *L) {
    const auto& a = ref<Vec2d>(L, 1);
    assert(lua_isnumber(L, 2));
    double scalar = luaL_checknumber(L, 2);
    init<Vec2d>(L) = a * scalar;
    return 1;
}
int vec2::tostring(lua_State *L) {
    auto& self = ref<Vec2d>(L, 1);
    std::string str{"{"}; 
    str += std::to_string(self.at(0)) + ", " + std::to_string(self.at(1)) + "}";
    lua_pushlstring(L, str.data(), str.size());
    return 1;
}
int vec2::sub(lua_State* L) {
    return 0;
}
int vec2::div(lua_State *L) {
    return 0;
}
int vec2::unm(lua_State* L) {
    init<Vec2d>(L) = -ref<Vec2d>(L, 1);
    return 1;
}
int vec2::namecall(lua_State *L) {
    auto& self = ref<Vec2d>(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    switch(static_cast<Method>(atom)) {
        case Method::dot:
            assert(is_type<Vec2d>(L, 2));
            lua_pushnumber(L, blaze::dot(self, ref<Vec2d>(L, 2)));
            return 1;
        case Method::unit:
            init<Vec2d>(L) = self / blaze::length(self);
            return 1;
        case Method::abs:
            init<Vec2d>(L) = blaze::abs(self);
            return 1;
        case Method::magnitude:
            lua_pushnumber(L, blaze::length(self));
            return 1;
    }
    return 0;
}
}
