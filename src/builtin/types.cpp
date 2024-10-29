#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "builtin/method_atom.h"
#include "types.h"
#include <SDL.h>
#include <SDL_image.h>

static constexpr auto texture_type_name = "texture";
using method = builtin::method_atom;
using texture_dtor = decltype(&SDL_DestroyTexture);
using texture_t = std::unique_ptr<SDL_Texture, texture_dtor>;
namespace lmm = builtin::metamethod;
namespace bi = builtin;
namespace cm = common;
static int texture_ctor(lua_State* L) {
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
        bi::create<texture_t>(L, text, SDL_DestroyTexture);
        common::print("created texture");
        lua_pushinteger(L, surface->w);
        lua_pushinteger(L, surface->h);
        return 3;
    }
}
static void texture_init(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<texture_t>());
    lua_pushstring(L, texture_type_name);
    lua_setfield(L, -2, lmm::type);
    lua_pop(L, 1);
}
static void sdl_init(lua_State* L) {
    const luaL_Reg lib[] = {
        {"texture", texture_ctor},
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, "sdl", lib);
    lua_pop(L, 1);
}

void bi::init_types(lua_State *L) {
    texture_init(L);
    sdl_init(L);
}

