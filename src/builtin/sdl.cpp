#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "builtin/method_atom.h"
#include "builtin/typedefs.h"
#include <SDL.h>
#include <SDL_image.h>

static constexpr auto texture_type_name = "Texture";
static constexpr auto font_type_name = "Font";
static constexpr auto rect_type_name = "Rect";
static constexpr auto recti_type_name = "Recti";
using method = builtin::method_atom;
namespace lmm = builtin::metamethod;
namespace bi = builtin;
namespace cm = common;
static SDL_Renderer* renderer() {
    return SDL_GetRenderer(engine::core::window());
}
static int open_font(lua_State* L) {
    TTF_Font* font = TTF_OpenFont(luaL_checkstring(L, 1), luaL_checkinteger(L, 2));
    if (not font) return 0;
    bi::create<bi::font_t>(L, font, TTF_CloseFont);
    return 1;
}
static int load_image(lua_State* L) {
    SDL_Surface* surface = IMG_Load(luaL_checkstring(L, 1));
    if (not surface) {
        cm::printerr("surface is not");
        return 0;
    } else {
        cm::deferred _([&surface] {
            SDL_FreeSurface(surface);
            surface = nullptr;
            common::print("destroyed surface");
        });
        SDL_Texture* text = SDL_CreateTextureFromSurface(SDL_GetRenderer(engine::core::window()), surface);
        if (not text) {
            common::printerr("failed to create texture");
            return 0;
        }
        bi::create<bi::texture_t>(L, text, SDL_DestroyTexture);
        common::print("created texture");
        lua_pushinteger(L, surface->w);
        lua_pushinteger(L, surface->h);
        return 3;
    }
}
static void font_init(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<bi::font_t>());
    lua_pushstring(L, font_type_name);
    lua_setfield(L, -2, lmm::type);
    lua_pop(L, 1);
}
static int rect_index(lua_State* L) {
    auto& r = bi::check<bi::rect_t>(L, 1);
    char key = *luaL_checkstring(L, 2);
    switch (key) {
        case 'x': lua_pushnumber(L, r.x); return 1;
        case 'y': lua_pushnumber(L, r.y); return 1;
        case 'w': lua_pushnumber(L, r.w); return 1;
        case 'h': lua_pushnumber(L, r.h); return 1;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
};
static int rect_newindex(lua_State* L) {
    auto& r = bi::check<bi::rect_t>(L, 1);
    char key = *luaL_checkstring(L, 2);
    int v = luaL_checknumber(L, 3);
    switch (key) {
        case 'x': r.x = v; return 0;
        case 'y': r.y = v; return 0;
        case 'w': r.w = v; return 0;
        case 'h': r.h = v; return 0;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
}
static int recti_index(lua_State* L) {
    auto& r = bi::check<bi::recti_t>(L, 1);
    char key = *luaL_checkstring(L, 2);
    switch (key) {
        case 'x': lua_pushinteger(L, r.x); return 1;
        case 'y': lua_pushinteger(L, r.y); return 1;
        case 'w': lua_pushinteger(L, r.w); return 1;
        case 'h': lua_pushinteger(L, r.h); return 1;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
};
static int recti_newindex(lua_State* L) {
    auto& r = bi::check<bi::recti_t>(L, 1);
    char key = *luaL_checkstring(L, 2);
    int v = luaL_checknumber(L, 3);
    switch (key) {
        case 'x': r.x = v; return 0;
        case 'y': r.y = v; return 0;
        case 'w': r.w = v; return 0;
        case 'h': r.h = v; return 0;
    }
    luaL_error(L, "invalid index %s", luaL_checkstring(L, 2));
    return 0;
}
static void rect_init(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<bi::rect_t>());
    const luaL_Reg rect[] = {
        {lmm::index, rect_index},
        {lmm::newindex, rect_index},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, rect);
    lua_pushstring(L, rect_type_name);
    lua_setfield(L, -2, lmm::type);
    lua_pop(L, 1);
}
static void recti_init(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<bi::recti_t>());
    const luaL_Reg rect[] = {
        {lmm::index, recti_index},
        {lmm::newindex, recti_index},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, rect);
    lua_pushstring(L, rect_type_name);
    lua_setfield(L, -2, lmm::type);
    lua_pop(L, 1);
}
static int rect(lua_State* L) {
    double x = luaL_optnumber(L, 1, 0);
    double y = luaL_optnumber(L, 2, 0);
    double w = luaL_optnumber(L, 3, 0);
    double h = luaL_optnumber(L, 4, 0);
    bi::create<bi::rect_t>(L, x, y, w, h);
    return 1;
}
static int recti(lua_State* L) {
    int x = luaL_optinteger(L, 1, 0);
    int y = luaL_optinteger(L, 2, 0);
    int w = luaL_optinteger(L, 3, 0);
    int h = luaL_optinteger(L, 4, 0);
    bi::create<bi::recti_t>(L, x, y, w, h);
    return 1;
}
static void texture_init(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<bi::texture_t>());
    lua_pushstring(L, texture_type_name);
    lua_setfield(L, -2, lmm::type);
    lua_pop(L, 1);
}

static int set_draw_color(lua_State* L) {
    auto& color = bi::check<bi::coloru32_t>(L, 1);
    SDL_SetRenderDrawColor(renderer(), color.red(), color.green(), color.blue(), color.alpha());
    return 0;
}
static int render_copy(lua_State* L) {
    constexpr int texture_arg = 1;
    constexpr int dest_arg = 2;
    constexpr int src_arg = 3;
    const auto& texture = bi::check<bi::texture_t>(L, texture_arg);
    const bool has_dest = not lua_isnoneornil(L, dest_arg);
    const bool has_src = not lua_isnoneornil(L, src_arg);
    SDL_RenderCopy(
        renderer(),
        texture.get(),
        has_src? &bi::check<bi::recti_t>(L, src_arg):nullptr,
        has_dest ? &bi::check<bi::recti_t>(L, dest_arg):nullptr);
    return 0;
}
void bi::sdl_init_lib(lua_State *L) {
    font_init(L);
    texture_init(L);
    rect_init(L);
    recti_init(L);
    const luaL_Reg sdl[] = {
        {"open_font", open_font},
        {"load_image", load_image},
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, "sdl", sdl);
    lua_pop(L, 1);
    lua_getglobal(L, "sdl");
    lua_newtable(L);
    const luaL_Reg render[] = {
        {"set_draw_color", set_draw_color},
        {"render_copy", render_copy},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, render);
    lua_setfield(L, -2, "render");
    lua_pop(L, 1);
    lua_pushcfunction(L, recti, recti_type_name);
    lua_setglobal(L, recti_type_name);
    lua_pushcfunction(L, rect, rect_type_name);
    lua_setglobal(L, rect_type_name);

}

