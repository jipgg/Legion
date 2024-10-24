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
}
int luau::renderer::draw(lua_State *L) {
    if (luau::is_type<common::Recti64>(L, 1)) {
        engine::renderer::draw(luau::ref<common::Recti64>(L, 1));
    }
    return 0;
}
int luau::renderer::fill(lua_State *L) {
    if (is_type<Recti64>(L, 1)) {
        engine::renderer::fill(ref<common::Recti64>(L, 1));
    }
    return 0;
}
int luau::renderer::render(lua_State *L) {
    return 0;
}
