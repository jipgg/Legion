#include "common/common.h"
#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "builtin/typedefs.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include "common/comptime.h"
using namespace std::string_literals;
enum class field {x, y, width, height};
static constexpr auto type_name{"Recti64"};
using type = builtin::recti64_t;
static type& rself(lua_State* L) {
    return builtin::check<type>(L, 1);
}
static int ctor(lua_State *L) {
    int16_t x{}, y{}, width{}, height{};
    if (lua_isnumber(L, 1)) x = luaL_checkinteger(L, 1);
    if (lua_isnumber(L, 2)) y = luaL_checkinteger(L, 2);
    if (lua_isnumber(L, 3)) width = luaL_checkinteger(L, 3);
    if (lua_isnumber(L, 4)) height = luaL_checkinteger(L, 4);
    builtin::create<type>(L, x, y, width, height);
    return 1;
}
static int tostring(lua_State *L) {
    auto& self = rself(L);
    const std::string str{type_name + "{"s
        + std::to_string(self.x()) + ", "
        + std::to_string(self.y()) + ", "
        + std::to_string(self.width()) + ", "
        + std::to_string(self.height()) + "}"};
    lua_pushstring(L, str.c_str());
    return 1;
}
static int index(lua_State *L) {
    static constexpr auto count = comptime::count<field, field::height>();
    static constexpr auto fields = comptime::to_array<field, count>();
    auto& self = rself(L);
    const std::string_view key = luaL_checkstring(L, 2);
    for (const auto& field : fields) {
        if (field.name == key) {
            switch (static_cast<enum field>(field.index)) {
                case field::x:
                    lua_pushinteger(L, self.x());
                    return 1;
                case field::y:
                    lua_pushinteger(L, self.y());
                    return 1;
                case field::width:
                    lua_pushinteger(L, self.width());
                    return 1;
                case field::height:
                    lua_pushinteger(L, self.height());
                    return 1;
            }
        }
    }
    common::printerr("invalid key", key);
    return 0;
}
void builtin::recti64_init_type(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<type>())) {
     const luaL_Reg metadata [] = {
            {metamethod::index, index},
            {metamethod::tostring, tostring},
            {nullptr, nullptr}
        };
        lua_pushstring(L, type_name);
        lua_setfield(L, -2, metamethod::type);
        luaL_register(L, nullptr, metadata);
        lua_pop(L, 1);
        lua_pushcfunction(L, ctor, type_name);
        lua_setglobal(L, type_name);
    }
}
