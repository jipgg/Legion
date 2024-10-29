#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "common/common.h"
#include <lualib.h>
#include "common/comptime.h"
using namespace std::string_literals;
enum class field{red, green, blue, alpha};
static constexpr auto type_name = "coloru32";
using type = common::coloru32;

static int ctor(lua_State *L) {
    uint8_t r{}, g{}, b{}, a{};
    if (lua_isnumber(L, 1)) r = luaL_checkinteger(L, 1);
    if (lua_isnumber(L, 2)) g = luaL_checkinteger(L, 2);
    if (lua_isnumber(L, 3)) b = luaL_checkinteger(L, 3);
    if (lua_isnumber(L, 4)) a = luaL_checkinteger(L, 4);
    builtin::create<type>(L, 0ui8, 0ui8, 0ui8, 0ui8) = {r, g, b, a};
    return 1;
}
static int index(lua_State *L) {
    static constexpr auto count = static_cast<std::size_t>(field::alpha); 
    static constexpr auto fields = comptime::to_array<field, count>();
    const auto& self = builtin::check<type>(L, 1);
    const std::string_view key = luaL_checkstring(L, 2);
    for (const auto& f : fields) {
        if (f.name == key) {
            switch(static_cast<field>(f.index)) {
                case field::red:
                    lua_pushinteger(L, self.red());
                return 1;
                case field::green:
                    lua_pushinteger(L, self.green());
                return 1;
                case field::blue:
                    lua_pushinteger(L, self.blue());
                return 1;
                case field::alpha:
                    lua_pushinteger(L, self.alpha());
                return 1;
            }
        }
    }
    return 0;
}
void builtin::coloru32_init_type(lua_State *L) {
    luaL_newmetatable(L, builtin::metatable_name<type>());
    const luaL_Reg metadata[] = {
        {metamethod::index, index},
        {nullptr, nullptr}
    };
    lua_pushstring(L, type_name);
    lua_setfield(L, -2, metamethod::type);
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, type_name);
    lua_setglobal(L, type_name);
}
