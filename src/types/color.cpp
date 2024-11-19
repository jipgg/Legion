#include <cstdint>
#include <lualib.h>
#include "builtin.h"
#include "lua_util.h"
#include "engine.h"
#include "lua_atom.h"
#include <SDL_image.h>
static constexpr auto type = "Color";
static constexpr std::string_view red = "Red";
static constexpr std::string_view green = "Green";
static constexpr std::string_view blue = "Blue";
static constexpr std::string_view alpha = "Alpha";
using builtin::color;
using engine::expect;

static int color_ctor(lua_State *L) {
    uint8_t r{}, g{}, b{}, a{};
    create<color>(L,
        static_cast<uint8_t>(luaL_optinteger(L, 1, 0)),
        static_cast<uint8_t>(luaL_optinteger(L, 2, 0)),
        static_cast<uint8_t>(luaL_optinteger(L, 3, 0)), 
        static_cast<uint8_t>(luaL_optinteger(L, 4, 0xff)));
    return 1;
}
static int index(lua_State *L) {
    const auto& self = check<color>(L, 1);
    const std::string_view key = luaL_checkstring(L, 2);
    switch(key.at(0)) {
        case red.at(0):
            expect(key == red);
            lua_pushinteger(L, self.r);
            return 1;
        case green.at(0):
            expect(key == green);
            lua_pushinteger(L, self.g);
            return 1;
        case blue.at(0):
            expect(key == blue);
            lua_pushinteger(L, self.b);
            return 1;
        case alpha.at(0):
            expect(key == alpha);
            lua_pushinteger(L, self.a);
            return 1;
        }
    return 0;
}
static int newindex(lua_State *L) {
    auto& self = check<color>(L, 1);
    size_t length;
    const std::string_view key = luaL_checkstring(L, 2);
    const int to_assign = luaL_checkinteger(L, 3);
    expect(to_assign >= 0 and to_assign <= 0xff, "color value exceeded range of [0, 255]");
    switch(key.at(0)) {
        case red.at(0):
            expect(key == red);
            self.r = to_assign;
            return 0;
        case green.at(0):
            self.g = to_assign;
            return 0;
        case blue.at(0):
            self.b = to_assign;
            return 0;
        case alpha.at(0):
            self.a = to_assign;
            return 0;
        }
    return 0;
}
static int namecall(lua_State* L) {
    int atom;
    lua_namecallatom(L, &atom);
    const auto& self = check<color>(L, 1);
    auto to_percent = [](uint8_t v) {return v / 255.f;};
    auto to_color = [](float r, float g, float b, float a) {
        constexpr int max = 0xff;
        return color{
            static_cast<uint8_t>(std::min<int>(r * max, max)),
            static_cast<uint8_t>(std::min<int>(g * max, max)),
            static_cast<uint8_t>(std::min<int>(b * max, max)),
            static_cast<uint8_t>(std::min<int>(a * max, max)),
        };
    };
    using la  = lua_atom;
    switch (static_cast<lua_atom>(atom)) {
        case la::Modulate: {//should refactor these for using it in draw_texture
            const vec3f dst_rgb{to_percent(self.r), to_percent(self.g), to_percent(self.b)};
            const float dst_a{to_percent(self.a)};
            const auto& other = check<color>(L, 2);
            const vec3f src_rgb{to_percent(other.r), to_percent(other.g), to_percent(other.b)};
            const vec3f result =  src_rgb * dst_rgb;
            create<color>(L, to_color(result[0], result[1], result[2], dst_a));
            return 1;
        }
        case la::Invert: {
            auto invert = [](uint8_t v) {return static_cast<uint8_t>(0xff - v);};
            create<color>(L, invert(self.r), invert(self.g), invert(self.b), self.a);
            return 1;
        }
        case la::Multiply: {
            const vec3f dst_rgb{to_percent(self.r), to_percent(self.g), to_percent(self.b)};
            const float dst_a{to_percent(self.a)};
            const auto& other = check<color>(L, 2);
            const vec3f src_rgb{to_percent(other.r), to_percent(other.g), to_percent(other.b)};
            const float src_a{to_percent(other.a)};
            const vec3f result = (src_rgb * dst_rgb) + dst_rgb * (1 - src_a);
            create<color>(L, to_color(result[0], result[1], result[2], dst_a));
            return 1;
        }
        case la::AdditiveBlend: {
            const vec3f dst_rgb{to_percent(self.r), to_percent(self.g), to_percent(self.b)};
            const float dst_a{to_percent(self.a)};
            const auto& other = check<color>(L, 2);
            const vec3f src_rgb{to_percent(other.r), to_percent(other.g), to_percent(other.b)};
            const float src_a{to_percent(other.a)};
            const vec3f result = src_rgb * src_a + dst_rgb;
            create<color>(L, to_color(result[0], result[1], result[2], dst_a));
            return 1;
        }
        case la::AlphaBlend: {
            const vec3f dst_rgb{to_percent(self.r), to_percent(self.g), to_percent(self.b)};
            const float dst_a{to_percent(self.a)};
            const auto& other = check<color>(L, 2);
            const vec3f src_rgb{to_percent(other.r), to_percent(other.g), to_percent(other.b)};
            const float src_a{to_percent(other.a)};
            const vec3f result = src_rgb * src_a + dst_rgb * (1 - src_a);
            const float result_a = src_a + (dst_a * (1 - src_a));
            create<color>(L, to_color(result[0], result[1], result[2], result_a));
            return 1;
        }
        default:
            return lua_err::invalid_method(L, builtin::tname::color);
    }
}
namespace builtin {
void register_color_type(lua_State *L) {
    luaL_newmetatable(L, metatable_name<color>());
    const luaL_Reg metadata[] = {
        {metamethod::index, index},
        {metamethod::newindex, newindex},
        {metamethod::namecall, namecall},
        {nullptr, nullptr}
    };
    lua_pushstring(L, type);
    lua_setfield(L, -2, metamethod::type);
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, color_ctor, "color_ctor");
    lua_setglobal(L, type);
}
}
