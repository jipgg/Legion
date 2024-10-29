#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "builtin/typedefs.h"
#include "engine.h"
#include <lualib.h>
namespace cm = common;
namespace bi = builtin;
namespace rnd = engine::renderer;
using engine::core::window;
static int draw(lua_State *L) {
    if (bi::is_type<bi::recti64_t>(L, 1)) {
        engine::renderer::draw(builtin::check<bi::recti64_t>(L, 1));
    }
    return 0;
}
static int fill(lua_State *L) {
    //engine::renderer::set_color({255, 255, 255, 255});
    if (bi::is_type<bi::recti64_t>(L, 1)) {
        auto& rect = bi::check<bi::recti64_t>(L, 1);
        //common::print(rect.data);
        engine::renderer::fill(rect);
    } else {
        common::print("bruh");
    }
    return 0;
}
static int render(lua_State *L) {
    auto& texture = bi::check<bi::texture_t>(L, 1);
    auto& src = bi::check<bi::recti64_t>(L, 2);
    SDL_Rect dest{src.x(), src.y(), src.width(), src.height()};
    SDL_RenderCopy(SDL_GetRenderer(window()), texture.get(), nullptr, &dest);
    return 0;
}
static int set_color(lua_State* L) {
    rnd::set_color(bi::check<bi::coloru32_t>(L, 1));
    return 0;
}
void bi::renderer_init_lib(lua_State *L) {
    const luaL_Reg lib[] = {
        {"draw", draw},
        {"fill", fill},
        {"set_color", set_color},
        {"render", render},
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, "renderer", lib);
    lua_pop(L, 1);
}
