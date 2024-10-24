#include "luau.defs.h"
#include "comptime.h"
#include "luau.h"
#include "common.h"
#include <lualib.h>
using namespace std::string_literals;
using Self = common::Sizei32;

void luau::Sizei32::init_type(lua_State *L) {
    luaL_newmetatable(L, metatable_name<Self>());
    const luaL_Reg metadata[] = {
        {"__index", index},
        {"__tostring", tostring},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, type_name);
    lua_setglobal(L, type_name);
}
int luau::Sizei32::ctor(lua_State *L) {
    int16_t width{}, height{};
    if (lua_isnumber(L, 1)) width = luaL_checkinteger(L, 1);
    if (lua_isnumber(L, 2)) height = luaL_checkinteger(L, 2);
    init<Self>(L, 0ui8, 0ui8) = {width, height};
    return 1;
}
int luau::Sizei32::index(lua_State *L) {
    static constexpr auto count = comptime::count<Field, Field::height>();
    static constexpr auto fields = comptime::to_array<Field, count>();
    auto& self = ref<Self>(L, 1);
    const std::string_view key = luaL_checkstring(L, 2);
    for (const auto& field : fields) {
        if (field.name == key) {
            switch (static_cast<Field>(field.index)) {
                case Field::width:
                    lua_pushinteger(L, self.width());
                    return 1;
                case Field::height:
                    lua_pushinteger(L, self.height());
                    return 1;
            }
        }
    }
    return 0;
}
int luau::Sizei32::tostring(lua_State *L) {
    auto& self = ref<Self>(L, 1);
    const std::string str{type_name + "{"s + std::to_string(self.width())
        + ", " + std::to_string(self.height()) + "}"};
    lua_pushstring(L, str.c_str());
    return 1;
}
