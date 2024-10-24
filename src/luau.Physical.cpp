#include "luau.defs.h"
#include "luau.h"
#include "component.h"
#include "types.h"
#include "comptime.h"
#include <lualib.h>
using Self = component::Wrapper<types::Physical>;
static constexpr auto fields = comptime::to_array<luau::Physical::Field, size_t(luau::Physical::Field::size) + 1>();
namespace cmm = common;

void luau::Physical::init_type(lua_State* L) {
    luaL_newmetatable(L, luau::metatable_name<Self>());
    const luaL_Reg metadata[] = {
        {"__index", index},
        {"__newindex", newindex},
        {"__namecall", namecall},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, type_name);
    lua_setglobal(L, type_name);
}

int luau::Physical::ctor(lua_State *L) {
    init<Self>(L, types::Physical{});
    lua_callbacks(L)->useratom = [](const char* name, size_t idk) {
        static constexpr auto count = comptime::count<Method, Method::bounds>();
        return static_cast<int16_t>(comptime::enum_item<Method, count>(name).index);
    };
    return 1;
}

int luau::Physical::index(lua_State *L) {
    auto& wrapper = luau::ref<Self>(L, 1);
    auto& self = wrapper.get();
    std::string_view key = luaL_checkstring(L, 2);
    for (const auto& f : fields) {
        if (f.name == key) {
            switch (static_cast<Field>(f.index)) {
                case Field::position:
                    luau::init<cmm::Vec2f>(L) = self.position;
                return 1;
                case Field::velocity:
                    init<cmm::Vec2f>(L) = self.velocity;
                return 1;
                case Field::acceleration:
                    init<cmm::Vec2f>(L) = self.acceleration;
                return 1;
                case Field::welded:
                    lua_pushboolean(L, self.welded);
                return 1;
                case Field::falling:
                    lua_pushboolean(L, self.falling);
                return 1;
                case Field::obstructed:
                    lua_pushboolean(L, self.obstructed);
                return 1;
                case Field::mass:
                    lua_pushnumber(L, self.mass);
                return 1;
                case Field::size:
                    init<cmm::Sizei32>(L, self.size);
                return 1;
                case Field::friction_coeff:
                    lua_pushnumber(L, self.friction_coeff);
                return 1;
                case Field::elasticity_coeff:
                    lua_pushnumber(L, self.elasticity_coeff);
                return 1;
            }
        }
    }
    common::printerr("invalid index", key);
    return 0;
}
int luau::Physical::newindex(lua_State *L) {
    auto& wrapper = luau::ref<Self>(L, 1);
    auto& self = wrapper.get();
    std::string_view key = luaL_checkstring(L, 2);
    for (const auto& f : fields) {
        common::print(f.name);
        if (f.name == key) {
            auto check_vec2f = [](lua_State* L) {
                if (is_type<cmm::Vec2f>(L, 3)) {
                    return std::make_optional(std::ref(luau::ref<cmm::Vec2f>(L, 3)));
                } else {
                    lua_pushstring(L, "invalid argument type");
                    lua_error(L);
                }
            };
            switch (static_cast<Field>(f.index)) {
                case Field::position:
                    self.position = *check_vec2f(L);
                return 0;
                case Field::velocity:
                    self.velocity = *check_vec2f(L);
                return 0;
                case Field::acceleration:
                    self.acceleration = *check_vec2f(L);
                return 0;
                case Field::welded:
                    self.welded = luaL_checkboolean(L, 3);
                return 0;
                case Field::falling:
                    self.falling = luaL_checkboolean(L, 3);
                return 0;
                case Field::obstructed:
                    self.obstructed = luaL_checkboolean(L, 3);
                return 0;
                case Field::mass:
                    self.mass = luaL_checknumber(L, 3);
                return 0;
                case Field::size:
                    self.size = luau::ref<common::Sizei32>(L, 3);
                return 0;
                case Field::friction_coeff:
                    self.friction_coeff = luaL_checknumber(L, 3);
                return 0;
                case Field::elasticity_coeff:
                    self.elasticity_coeff = luaL_checknumber(L, 3);
                return 0;
            }
        }
    }
    return 0;
}
int luau::Physical::namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    auto& self = luau::ref<Self>(L, 1).get();
    switch(static_cast<Method>(atom)) {
        case Method::bounds:
            init<common::Recti64>(L, self.bounds());
        return 1;
    }
    return 0;
}
