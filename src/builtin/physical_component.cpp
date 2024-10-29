#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "builtin/method_atom.h"
#include "component.h"
#include "types.h"
#include "common/comptime.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
using method = builtin::method_atom;
enum class field{position, velocity, acceleration,
    welded, falling, obstructed, elasticity_coeff,
    friction_coeff, mass, size};
static constexpr auto type_name{"physical_component"};
using type = component::raii_wrapper<types::physical_component>;
namespace cm = common;
static constexpr auto fields = comptime::to_array<field, size_t(field::size) + 1>();
static int index(lua_State *L) {
    auto& wrapper = builtin::check<type>(L, 1);
    auto& self = wrapper.get();
    std::string_view key = luaL_checkstring(L, 2);
    for (const auto& f : fields) {
        if (f.name == key) {
            switch (static_cast<field>(f.index)) {
                case field::position:
                    builtin::create<cm::vec2d>(L) = self.position;
                return 1;
                case field::velocity:
                    builtin::create<cm::vec2d>(L) = self.velocity;
                return 1;
                case field::acceleration:
                    builtin::create<cm::vec2d>(L) = self.acceleration;
                return 1;
                case field::welded:
                    lua_pushboolean(L, self.welded);
                return 1;
                case field::falling:
                    lua_pushboolean(L, self.falling);
                return 1;
                case field::obstructed:
                    lua_pushboolean(L, self.obstructed);
                return 1;
                case field::mass:
                    lua_pushnumber(L, self.mass);
                return 1;
                case field::size:
                    builtin::create<cm::vec2d>(L) = self.size;
                return 1;
                case field::friction_coeff:
                    lua_pushnumber(L, self.friction_coeff);
                return 1;
                case field::elasticity_coeff:
                    lua_pushnumber(L, self.elasticity_coeff);
                return 1;
            }
        }
    }
    common::printerr("invalid index", key);
    return 0;
}
static int newindex(lua_State *L) {
    auto& wrapper = builtin::check<type>(L, 1);
    auto& self = wrapper.get();
    std::string_view key = luaL_checkstring(L, 2);
    for (const auto& f : fields) {
        if (f.name == key) {
            auto check_vec2 = [](lua_State* L) {
                if (builtin::is_type<cm::vec2d>(L, 3)) {
                    return std::make_optional(std::ref(builtin::check<cm::vec2d>(L, 3)));
                } else {
                    lua_pushstring(L, "invalid argument type");
                    lua_error(L);
                }
            };
            switch (static_cast<field>(f.index)) {
                case field::position:
                    if (auto v2 = check_vec2(L)) {
                        cm::vec2d& v = *v2;
                        self.position.at(0) = v.at(0);
                        self.position.at(1) = v.at(1);
                    } else common::printerr("failed to assign value");
                return 0;
                case field::velocity:
                    if (auto v2 = check_vec2(L)) {
                        cm::vec2d& v = *v2;
                        self.velocity.at(0) = v.at(0);
                        self.velocity.at(1) = v.at(1);
                    } else common::printerr("failed to assign value");
                return 0;
                case field::acceleration:
                    if (auto v2 = check_vec2(L)) {
                        cm::vec2d& v = *v2;
                        self.acceleration.at(0) = v.at(0);
                        self.acceleration.at(1) = v.at(1);
                    } else common::printerr("failed to assign value");
                return 0;
                case field::welded:
                    self.welded = luaL_checkboolean(L, 3);
                return 0;
                case field::falling:
                    self.falling = luaL_checkboolean(L, 3);
                return 0;
                case field::obstructed:
                    self.obstructed = luaL_checkboolean(L, 3);
                return 0;
                case field::mass:
                    self.mass = luaL_checknumber(L, 3);
                return 0;
                case field::size:
                    if (auto v2 = check_vec2(L)) {
                        cm::vec2d& v = *v2;
                        self.size.at(0) = v.at(0);
                        self.size.at(1) = v.at(1);
                    } else common::printerr("failed to assign value");
                return 0;
                case field::friction_coeff:
                    self.friction_coeff = luaL_checknumber(L, 3);
                return 0;
                case field::elasticity_coeff:
                    self.elasticity_coeff = luaL_checknumber(L, 3);
                return 0;
            }
        }
    }
    return 0;
}
static int namecall(lua_State *L) {
    common::print("namecalling");
    int atom{};
    lua_namecallatom(L, &atom);
    auto& self = builtin::check<type>(L, 1).get();
    switch(static_cast<method>(atom)) {
        case method::bounds:
            builtin::create<cm::recti64>(L, self.bounds()); 
        return 1;
        default:
        return 0;
    }
}
static constexpr int16_t nullmethod = -1;
static int ctor(lua_State *L) {
    builtin::create<type>(L, component::raii_wrapper(types::physical_component{}));
    return 1;
}

void builtin::physical_init_type(lua_State* L) {
    luaL_newmetatable(L, builtin::metatable_name<type>());
    const luaL_Reg metadata[] = {
        {metamethod::index, index},
        {metamethod::newindex, newindex},
        {metamethod::namecall, namecall},
        {nullptr, nullptr}
    };
    lua_pushstring(L, type_name);
    lua_setfield(L, -2, metamethod::type);
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, type_name);
    lua_setglobal(L, type_name);
}

