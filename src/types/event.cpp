#include "builtin.h"
#include "lua_util.h"
#include "lua_atom.h"
#include "lua_event.h"
namespace bi = builtin;
namespace tn = bi::tname;
using event = lua_event;

static int ctor(lua_State* L) {
    create<event>(L, L);
    return 1;
}


static int namecall(lua_State* L) {
    int atom;
    auto& r = check<lua_event>(L, 1); 
    lua_namecallatom(L, &atom);
    using la = lua_atom;
    switch (static_cast<la>(atom)) {
        case la::connect: {
            if (not lua_isfunction(L, 2)) return err_invalid_type(L);
            int id = r.connect(2);
            lua_pushinteger(L, id);
            return 1;
        }
        case la::disconnect: {
            r.disconnect(luaL_checkinteger(L, 2));
            return 0;
        }
        case la::fire: {
            lua_remove(L, -lua_gettop(L));//removes event from stack
            //print("top is", lua_gettop(L));
            r.fire(lua_gettop(L));
            return 0;
        }
        default:
        return err_invalid_method(L, tn::event);
    }
}

int builtin::class_event(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<event>())) {
        const luaL_Reg lib[] = {
            {"__namecall", namecall},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, lib);
        lua_pushstring(L, tn::event);
        lua_setfield(L, -2, bi::metamethod::type);
        lua_pop(L, 1);
    }
    lua_pushcfunction(L, ctor, tn::event);
    return 1;
}
