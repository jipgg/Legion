#include "luau.defs.h"
#include "luau.h"
#include "common.h"
#include <lualib.h>
#include "comptime.h"
using namespace std::string_literals;
using Self = common::Coloru32;
void luau::Color::init_type(lua_State *L) {
    luaL_newmetatable(L, luau::metatable_name<Self>());
    const luaL_Reg metadata[] = {
        {"__index", index},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, type_name);
    lua_setglobal(L, type_name);
}
int luau::Color::ctor(lua_State *L) {
    uint8_t r{}, g{}, b{}, a{};
    if (lua_isnumber(L, 1)) r = luaL_checkinteger(L, 1);
    if (lua_isnumber(L, 2)) g = luaL_checkinteger(L, 2);
    if (lua_isnumber(L, 3)) b = luaL_checkinteger(L, 3);
    if (lua_isnumber(L, 4)) a = luaL_checkinteger(L, 4);
    luau::init<Self>(L, 0ui8, 0ui8, 0ui8, 0ui8) = {r, g, b, a};
    return 1;
}
int luau::Color::index(lua_State *L) {
    static constexpr auto count = static_cast<std::size_t>(Field::alpha); 
    static constexpr auto fields = comptime::to_array<Field, count>();
    const auto& self = ref<Self>(L, 1);
    const std::string_view key = luaL_checkstring(L, 2);
    for (const auto& f : fields) {
        if (f.name == key) {
            switch(static_cast<Field>(f.index)) {
                case Field::red:
                    lua_pushinteger(L, self.red());
                return 1;
                case Field::green:
                    lua_pushinteger(L, self.green());
                return 1;
                case Field::blue:
                    lua_pushinteger(L, self.blue());
                return 1;
                case Field::alpha:
                    lua_pushinteger(L, self.alpha());
                return 1;
            }
        }
    }
    return 0;
}
