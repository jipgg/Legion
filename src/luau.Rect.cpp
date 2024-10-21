#include "legion/luau.types.h"
#include "legion/common.h"
#include "legion/luau.userdata_utility.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include "legion/comptime_enum.h"
using namespace std::string_literals;
namespace ct = comptime;
static Recti64& rself(lua_State* L) {
    return luau::ref<Recti64>(L, 1);
}
namespace luau {
void Rect::init_metadata(lua_State* L) {
    luaL_newmetatable(L, metatable_name<Recti64>());
    const luaL_Reg metadata [] = {
        {"__index", index},
        {"__tostring", tostring},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, type_name);
    lua_setglobal(L, type_name);
}
int Rect::ctor(lua_State *L) {
    int16_t x{}, y{}, width{}, height{};
    if (lua_isnumber(L, 1)) x = luaL_checkinteger(L, 1);
    if (lua_isnumber(L, 2)) y = luaL_checkinteger(L, 2);
    if (lua_isnumber(L, 3)) width = luaL_checkinteger(L, 3);
    if (lua_isnumber(L, 4)) height = luaL_checkinteger(L, 4);
    init<Recti64>(L) = {x, y, width, height};
    return 1;
}
int Rect::tostring(lua_State *L) {
    auto& self = rself(L);
    const std::string str{type_name + "{"s
        + std::to_string(self.x()) + ", "
        + std::to_string(self.y()) + ", "
        + std::to_string(self.width()) + ", "
        + std::to_string(self.height()) + "}"};
    lua_pushstring(L, str.c_str());
    return 1;
}
int Rect::index(lua_State *L) {
    static constexpr auto count = ct::count<Field, Field::height>();
    static constexpr auto fields = ct::to_array<Field, count>();
    auto& self = rself(L);
    const std::string_view key = luaL_checkstring(L, 2);
    for (const auto& field : fields) {
        if (field.name == key) {
            switch (static_cast<Field>(field.index)) {
                case Field::x:
                    lua_pushinteger(L, self.x());
                    return 1;
                case Field::y:
                    lua_pushinteger(L, self.y());
                    return 1;
                case Field::width:
                    lua_pushinteger(L, self.width());
                    return 1;
                case Field::height:
                    lua_pushinteger(L, self.height());
                    return 1;
            }
        }
    }
    printerr("invalid key", key);
    return 0;
}
}
