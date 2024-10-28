#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "builtin/method_atom.h"
#include "types.h"

static constexpr auto texture_type_name = "texture";
using method = builtin::method_atom;
using texture_t = types::texture;
namespace lmm = builtin::metamethod;
namespace bi = builtin;
namespace cm = common;
static int texture_namecall(lua_State* L) {
    int atom;
    lua_namecallatom(L, &atom);
    switch (static_cast<method>(atom)) {
        case method::size:
            bi::push<cm::vec2i>(L) = bi::get<texture_t>(L, 1).src_size();
            return 1;
        default:
        return 0;
    }
}
static int texture_ctor(lua_State* L) {
    bi::push<texture_t>(L, luaL_checkstring(L, 1));
    return 1;
}
static void texture_init(lua_State* L) {
    if (luaL_newmetatable(L, bi::metatable_name<texture_t>())) {
        const luaL_Reg texture_meta[] = {
            {lmm::namecall, texture_namecall},
            {nullptr, nullptr}
        }; 
        luaL_register(L, nullptr, texture_meta);
        lua_pushstring(L, texture_type_name);
        lua_setfield(L, -2, lmm::type);
        lua_pop(L, 1);
        lua_pushvalue(L, LUA_GLOBALSINDEX);
        lua_pushcfunction(L, texture_ctor, texture_type_name);
        lua_setfield(L, -2, texture_type_name);
        lua_pop(L, 1);
    } else {
        common::printerr("type texture was already initialized");
        lua_pop(L, 1);
    }
}

void bi::init_types(lua_State *L) {
    texture_init(L);
}

