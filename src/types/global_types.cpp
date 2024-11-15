#include <cstdint>
#include <lualib.h>
#include "builtin.h"
#include "lua_util.h"
#include "engine.h"
#include "lua_atom.h"
#include <SDL_image.h>
#include "util.h"
using namespace std::string_literals;

namespace bi = builtin;
namespace mm = bi::metamethod;
namespace tn = bi::tname;
static constexpr size_t x_length{std::string("x").length()};
static constexpr size_t y_length{std::string("x").length()};
static constexpr size_t width_length{std::string("width").length()};
static constexpr size_t height_length{std::string("height").length()};
static constexpr size_t file_path_length{std::string("file_path").length()};
static constexpr size_t pt_size_length(std::string("pt_size").length());
static constexpr size_t red_length{std::string("red").length()};
static constexpr size_t green_length{std::string("green").length()};
static constexpr size_t blue_length{std::string("blue").length()};
static constexpr size_t alpha_length{std::string("alpha").length()};
static const std::string invalid_member_key = "invalid member key "; 
//color
static int color_ctor(lua_State *L) {
    uint8_t r{}, g{}, b{}, a{};
    create<bi::color>(L,
        static_cast<uint8_t>(luaL_optinteger(L, 1, 0)),
        static_cast<uint8_t>(luaL_optinteger(L, 2, 0)),
        static_cast<uint8_t>(luaL_optinteger(L, 3, 0)), 
        static_cast<uint8_t>(luaL_optinteger(L, 4, 0xff)));
    return 1;
}
static int color_index(lua_State *L) {
    const auto& self = check<bi::color>(L, 1);
    size_t length;
    const char key = *luaL_checklstring(L, 2, &length);
    switch(key) {
        case 'r':
            engine::expect(length == red_length);
            lua_pushinteger(L, self.r);
            return 1;
        case 'g':
            engine::expect(length == green_length);
            lua_pushinteger(L, self.g);
            return 1;
        case 'b':
            engine::expect(length == blue_length);
            lua_pushinteger(L, self.b);
            return 1;
        case 'a':
            engine::expect(length == alpha_length);
            lua_pushinteger(L, self.a);
            return 1;
        }
    return 0;
}
static int color_newindex(lua_State *L) {
    auto& self = check<bi::color>(L, 1);
    size_t length;
    const char key = *luaL_checklstring(L, 2, &length);
    const int to_assign = luaL_checkinteger(L, 3);
    engine::expect(to_assign >= 0 and to_assign <= 0xff, "color value exceeded range of [0, 255]");
    switch(key) {
        case 'r':
            engine::expect(length == red_length);
            self.r = to_assign;
            return 0;
        case 'g':
            engine::expect(length == green_length);
            self.g = to_assign;
            return 0;
        case 'b':
            engine::expect(length == blue_length);
            self.b = to_assign;
            return 0;
        case 'a':
            engine::expect(length == alpha_length);
            self.a = to_assign;
            return 0;
        }
    return 0;
}
static int color_namecall(lua_State* L) {
    int atom;
    lua_namecallatom(L, &atom);
    const auto& self = check<bi::color>(L, 1);
    auto to_percent = [](uint8_t v) {return v / 255.f;};
    auto to_color = [](float r, float g, float b, float a) {
        constexpr int max = 0xff;
        return bi::color{
            static_cast<uint8_t>(std::min<int>(r * max, max)),
            static_cast<uint8_t>(std::min<int>(g * max, max)),
            static_cast<uint8_t>(std::min<int>(b * max, max)),
            static_cast<uint8_t>(std::min<int>(a * max, max)),
        };
    };
    using la  = lua_atom;
    switch (static_cast<lua_atom>(atom)) {
        case la::modulate: {//should refactor these for using it in draw_texture
            const vec3f dst_rgb{to_percent(self.r), to_percent(self.g), to_percent(self.b)};
            const float dst_a{to_percent(self.a)};
            const auto& other = check<bi::color>(L, 2);
            const vec3f src_rgb{to_percent(other.r), to_percent(other.g), to_percent(other.b)};
            const vec3f result =  src_rgb * dst_rgb;
            create<bi::color>(L, to_color(result[0], result[1], result[2], dst_a));
            return 1;
        }
        case la::invert: {
            auto invert = [](uint8_t v) {return static_cast<uint8_t>(0xff - v);};
            create<bi::color>(L, invert(self.r), invert(self.g), invert(self.b), self.a);
            return 1;
        }
        case la::multiply: {
            const vec3f dst_rgb{to_percent(self.r), to_percent(self.g), to_percent(self.b)};
            const float dst_a{to_percent(self.a)};
            const auto& other = check<bi::color>(L, 2);
            const vec3f src_rgb{to_percent(other.r), to_percent(other.g), to_percent(other.b)};
            const float src_a{to_percent(other.a)};
            const vec3f result = (src_rgb * dst_rgb) + dst_rgb * (1 - src_a);
            create<bi::color>(L, to_color(result[0], result[1], result[2], dst_a));
            return 1;
        }
        case la::additive_blend: {
            const vec3f dst_rgb{to_percent(self.r), to_percent(self.g), to_percent(self.b)};
            const float dst_a{to_percent(self.a)};
            const auto& other = check<bi::color>(L, 2);
            const vec3f src_rgb{to_percent(other.r), to_percent(other.g), to_percent(other.b)};
            const float src_a{to_percent(other.a)};
            const vec3f result = src_rgb * src_a + dst_rgb;
            create<bi::color>(L, to_color(result[0], result[1], result[2], dst_a));
            return 1;
        }
        case la::alpha_blend: {
            const vec3f dst_rgb{to_percent(self.r), to_percent(self.g), to_percent(self.b)};
            const float dst_a{to_percent(self.a)};
            const auto& other = check<bi::color>(L, 2);
            const vec3f src_rgb{to_percent(other.r), to_percent(other.g), to_percent(other.b)};
            const float src_a{to_percent(other.a)};
            const vec3f result = src_rgb * src_a + dst_rgb * (1 - src_a);
            const float result_a = src_a + (dst_a * (1 - src_a));
            create<bi::color>(L, to_color(result[0], result[1], result[2], result_a));
            return 1;
        }
        default:
            return lua_err::invalid_method(L, tn::color);
    }
}
void color_init(lua_State *L) {
    luaL_newmetatable(L, metatable_name<bi::color>());
    const luaL_Reg metadata[] = {
        {mm::index, color_index},
        {mm::newindex, color_newindex},
        {mm::namecall, color_namecall},
        {nullptr, nullptr}
    };
    lua_pushstring(L, tn::color);
    lua_setfield(L, -2, mm::type);
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, color_ctor, "color_ctor");
    lua_setglobal(L, "color");
}
//font
static void opaque_font_init(lua_State* L) {
    luaL_newmetatable(L, metatable_name<bi::font_ptr>());
    lua_pushstring(L, tn::opaque_font);
    lua_setfield(L, -2, mm::type);
    lua_pop(L, 1);
}
//rect
static int rectangle_index(lua_State* L) {
    auto& r = check<bi::rectangle>(L, 1);
    size_t length;
    std::string_view key = luaL_checklstring(L, 2, &length);
    switch (key[0]) {
        case 'x':
            engine::expect(length == x_length, invalid_member_key);
            lua_pushnumber(L, r.x);
            return 1;
        case 'y':
            engine::expect(length == y_length, invalid_member_key);
            lua_pushnumber(L, r.y);
            return 1;
        case 'w':
            engine::expect(length == width_length, invalid_member_key);
            lua_pushnumber(L, r.w);
            return 1;
        case 'h':
            engine::expect(length == height_length, invalid_member_key);
            lua_pushnumber(L, r.h);
            return 1;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
};
static int rectangle_newindex(lua_State* L) {
    auto& r = check<bi::rectangle>(L, 1);
    using sv = std::string_view;
    size_t length;
    char initial = *luaL_checklstring(L, 2, &length);
    int v = luaL_checknumber(L, 3);
    switch (initial) {
        case 'x':
            engine::expect(length == x_length, invalid_member_key);
            r.x = v;
            return 0;
        case 'y':
            engine::expect(length == y_length, invalid_member_key);
            r.y = v;
            return 0;
        case 'w':
            engine::expect(length == width_length, invalid_member_key);
            r.w = v;
            return 0;
        case 'h': 
            engine::expect(length == height_length, invalid_member_key);
            r.h = v;
            return 0;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
}
static int rectangle_ctor(lua_State* L) {
    double x = luaL_optnumber(L, 1, 0);
    double y = luaL_optnumber(L, 2, 0);
    double w = luaL_optnumber(L, 3, 0);
    double h = luaL_optnumber(L, 4, 0);
    create<bi::rectangle>(L, x, y, w, h);
    return 1;
}
static void rectangle_init(lua_State* L) {
    luaL_newmetatable(L, metatable_name<bi::rectangle>());
    const luaL_Reg meta[] = {
        {mm::index, rectangle_index},
        {mm::newindex, rectangle_newindex},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, meta);
    lua_pushstring(L, tn::rectangle);
    lua_setfield(L, -2, mm::type);
    lua_pop(L, 1);
    lua_pushcfunction(L, rectangle_ctor, "rectangle_ctor");
    lua_setglobal(L, "rectangle");
}
//texture
static constexpr size_t color_length{std::string("color").length()};
static constexpr size_t blend_mode_length{std::string("blend_mode").length()};
static int texture_index(lua_State* L) {
    auto& r = check<bi::texture>(L, 1);
    size_t length;
    const char key = *luaL_checklstring(L, 2, &length);
    switch (key) {
        case 'w':
            engine::expect(length == width_length, invalid_member_key + luaL_checkstring(L, 2));
            lua_pushinteger(L, r.w);
            return 1;
        case 'h':
            engine::expect(length == height_length, invalid_member_key + luaL_checkstring(L, 2));
            lua_pushinteger(L, r.h);
            return 1;
        case 'c':
            engine::expect(length == color_length, invalid_member_key + luaL_checkstring(L, 2));
            bi::color color;
            SDL_GetTextureColorMod(r.ptr.get(), &color.r, &color.g, &color.b);
            SDL_GetTextureAlphaMod(r.ptr.get(), &color.a);
            create<bi::color>(L, std::move(color));
            return 1;
        case 'b':
            engine::expect(length == blend_mode_length, invalid_member_key + luaL_checkstring(L, 2));
            SDL_BlendMode bm;
            SDL_GetTextureBlendMode(r.ptr.get(), &bm);
            lua_pushstring(L, blendmode_to_string(bm));
            return 1;
        default: return lua_err::invalid_member(L, tn::texture);
    }
}
static int texture_newindex(lua_State* L) {
    auto& r = check<bi::texture>(L, 1);
    size_t length;
    const char key = *luaL_checklstring(L, 2, &length);
    switch (key) {
        case 'w': {
            engine::expect(length == width_length);
            const int v = luaL_checkinteger(L, 3);
            r.w = v;
            return 0;
        }
        case 'h': {
            engine::expect(length == height_length);
            const int v = luaL_checkinteger(L, 3);
            r.h = v;
            return 0;
        }
        case 'c': {
            engine::expect(length == color_length, invalid_member_key + luaL_checkstring(L, 2));
            const auto& new_color = check<bi::color>(L, 3);
            SDL_SetTextureColorMod(r.ptr.get(), new_color.r, new_color.g, new_color.b);
            SDL_SetTextureAlphaMod(r.ptr.get(), new_color.a);
            return 0;
        }
        case 'b': {
            engine::expect(length == blend_mode_length, invalid_member_key + luaL_checkstring(L, 2));
            SDL_BlendMode bm = string_to_blendmode(luaL_checkstring(L, 3));
            SDL_SetTextureBlendMode(r.ptr.get(), bm);
            return 0;
        }
        default:
            return lua_err::invalid_member(L, tn::texture);
    }
}
static int texture_ctor_call(lua_State* L) {
    auto path = resolve_path_type(L, 2);
    if (not path) {
        return lua_err::invalid_argument(L, 2, "path | string");
    }
    SDL_Surface* surface = IMG_Load(path->c_str());
    if (not surface) {
        luaL_error(L, SDL_GetError());
        return 0;
    }
    scope_guard d([&surface] {SDL_FreeSurface(surface);});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(util::renderer(), surface);
    if (not texture) {
        luaL_error(L, SDL_GetError());
        return 0;
    }
    create<bi::texture>(L, bi::texture{
        bi::texture_ptr(texture, SDL_DestroyTexture),
        surface->w,
        surface->h
    });
    return 1;
}
static int texture_ctor_from_string(lua_State* L) {
    const char* string = luaL_checkstring(L, 1);
    bi::font& font = is_type<bi::font>(L, 2) ?
        check<bi::font>(L, 2) : engine::default_font();
    const bi::color& color = is_type<bi::color>(L, 3) ?
        check<bi::color>(L, 3) : bi::color{0xff, 0xff, 0xff, 0xff};
    SDL_Surface* surface = TTF_RenderText_Blended(
        font.ptr.get(), string, color);
    if (not surface) {
        luaL_error(L, SDL_GetError());
        return 0;
    }
    scope_guard d([&surface]{SDL_FreeSurface(surface);});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(util::renderer(), surface);
    if (not texture) {
        luaL_error(L, SDL_GetError());
        return 0;
    }
    create<bi::texture>(L, bi::texture{
        bi::texture_ptr(texture, SDL_DestroyTexture),
        surface->w,
        surface->h
    });
    return 1;
}
static void texture_init(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<bi::texture>())) {
        const luaL_Reg meta[] = {
            {mm::index, texture_index},
            {mm::newindex, texture_newindex},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, tn::texture);
        lua_setfield(L, -2, mm::type);
    }
    lua_pop(L, 1);
    const std::string texture_ctor_tname{tn::texture + "_ctor"s};
    if (luaL_newmetatable(L, texture_ctor_tname.c_str())) {
        const luaL_Reg meta[] = {
            {mm::call, texture_ctor_call},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
    }
    lua_pop(L, 1);
    const luaL_Reg ctors[] = {
        {"from_string", texture_ctor_from_string},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, ctors);
    luaL_getmetatable(L, texture_ctor_tname.c_str());
    lua_setmetatable(L, -2);
    lua_setglobal(L, "texture");
}
static void opaque_texture_init(lua_State* L) {
    luaL_newmetatable(L, metatable_name<bi::texture_ptr>());
    lua_pushstring(L, tn::opaque_texture);
    lua_setfield(L, -2, mm::type);
    lua_pop(L, 1);
}
static int font_index(lua_State* L) {
    auto& r = check<bi::font>(L, 1);
    size_t len;
    const char initial = *luaL_checklstring(L, 2, &len);
    switch (initial) {
        case 'f':
            engine::expect(len == file_path_length);
            create<bi::path>(L, r.file_path);
            return 1;
        case 'p': {
            engine::expect(len == file_path_length);
            lua_pushinteger(L, r.pt_size);
            return 1;
        }
    } 
    return lua_err::invalid_member(L, "font");
}
static int font_newindex(lua_State* L) {
    luaL_error(L, "member access is read-only");
    return 0;
}
static int font_ctor(lua_State* L) {
    const auto& font_path = check<bi::path>(L, 1);
    const int pt_size = luaL_checkinteger(L, 2);
    TTF_Font* font_resource = TTF_OpenFont(font_path.string().c_str(), pt_size);
    engine::expect(font_resource != nullptr, SDL_GetError());

    create<builtin::font>(L, builtin::font{
        .ptr{font_resource, TTF_CloseFont},
        .pt_size = pt_size,
        .file_path = font_path
    });
    return 1;
}
static void font_init(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<bi::font>())) {
        const luaL_Reg meta[] = {
            {mm::index, texture_index},
            {mm::newindex, texture_newindex},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, "font");
        lua_setfield(L, -2, mm::type);
    }
    lua_pop(L, 1);
    lua_pushcfunction(L, font_ctor, "font_ctor");
    lua_setglobal(L, "font");
}
namespace builtin {
void init_global_types(lua_State *L) {
    color_init(L);
    opaque_font_init(L);
    font_init(L);
    rectangle_init(L);
    opaque_texture_init(L);
    texture_init(L);
}
}
