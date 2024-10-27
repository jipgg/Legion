#include "luau.defs.h"
#include "luau.h"
#include "engine.h"
#include <lualib.h>

void luau::renderer::init_lib(lua_State *L) {
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
int luau::renderer::draw(lua_State *L) {
    if (luau::is_type<common::Recti64>(L, 1)) {
        engine::renderer::draw(luau::ref<common::Recti64>(L, 1));
    }
    return 0;
}
int luau::renderer::fill(lua_State *L) {
    engine::renderer::set_color({255, 255, 255, 255});
    if (is_type<common::Recti64>(L, 1)) {
        auto& rect = luau::ref<common::Recti64>(L, 1);
        //common::print(rect.data);
        engine::renderer::fill(rect);
    } else {
        common::print("bruh");
    }
    return 0;
}
int luau::renderer::render(lua_State *L) {
    return 0;
}
