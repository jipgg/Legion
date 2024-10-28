#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "engine.h"
#include <lualib.h>
namespace cm = common;
namespace bi = builtin;
static int draw(lua_State *L) {
    if (bi::is_type<cm::recti64>(L, 1)) {
        engine::renderer::draw(builtin::get<cm::recti64>(L, 1));
    }
    return 0;
}
static int fill(lua_State *L) {
    engine::renderer::set_color({255, 255, 255, 255});
    if (bi::is_type<cm::recti64>(L, 1)) {
        auto& rect = bi::get<cm::recti64>(L, 1);
        //common::print(rect.data);
        engine::renderer::fill(rect);
    } else {
        common::print("bruh");
    }
    return 0;
}
static int render(lua_State *L) {
    return 0;
}
void bi::renderer_init_lib(lua_State *L) {
    const luaL_Reg lib[] = {
        {"draw", draw},
        {"fill", fill},
        {"render", render},
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, "renderer", lib);
    lua_pop(L, 1);
}
