#include "luaulib/Vec2.h"
#include "legion/common.h"
#include "luaulib/userdata_utility.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include "legion/comptime_enum.h"
namespace luaulib {
namespace vec2 {
enum class Method {
    dot, unit, magnitude
};
}
void vec2::init_metadata(lua_State* L) {
    luaL_newmetatable(L, metatable_name<Vec2d>());
    const luaL_Reg metadata [] = {
        {"__index", index},
        {"__add", add},
        {"__mul", mul},
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
    double x = luaL_checknumber(L, 1);
    double y = luaL_checknumber(L, 2);
    init<Vec2d>(L) = {x,y};
    lua_callbacks(L)->useratom = [](const char* name, size_t idk) {
        namespace ct = comptime;
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
int vec2::index(lua_State *L) {
    return 0;
}
int vec2::newindex(lua_State *L) {
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
int vec2::namecall(lua_State *L) {
    auto& self = ref<Vec2d>(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    switch(static_cast<Method>(atom)) {
        case Method::dot:
            if (lua_userdatatag(L, -1) == type_tag<Vec2d>()) {
                lua_pushnumber(L, blaze::dot(self, ref<Vec2d>(L, 2)));
                return 1;
            } else {
                printerr("invalid right hand side operand");
                return 0;
            }
        case Method::unit:
            init<Vec2d>(L) = self / blaze::length(self);
            return 1;
        case Method::magnitude:
            lua_pushnumber(L, blaze::length(self));
            return 1;
        default:
            assert(false);
            return 0;
    }
}
}
