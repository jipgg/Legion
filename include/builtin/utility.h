#pragma once
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <typeinfo>
#include <memory>
#include "engine.h"
#include "common/common.h"
#include <SDL_scancode.h>
namespace builtin {
const char* scancode_to_string(SDL_Scancode scancode);
SDL_Scancode string_to_scancode(std::string_view string);
namespace intern {
inline int unique_tag_incr{0};
}
template <class my_type>
int type_tag() {
    static const int tag = intern::unique_tag_incr++; 
    return tag;
}
template <class my_type>
bool is_type(lua_State* L, int idx) {
    return lua_userdatatag(L, idx) == type_tag<my_type>();
}
template<class my_type>
const char* metatable_name() {
    const std::type_info& ti = typeid(my_type);
    return ti.raw_name();
}
template <class my_type, class ...init_arguments>
my_type& push(lua_State* L, init_arguments&&...args) {
    void* ud = lua_newuserdatatagged(L, sizeof(my_type), type_tag<my_type>());
    new (ud) my_type{std::forward<init_arguments>(args)...};
    lua_setuserdatadtor(L, type_tag<my_type>(), [](lua_State* L, void* data) {
        static_cast<my_type*>(data)->~my_type();//cause using placement new, no implicit destruction
    });
    if (luaL_getmetatable(L, metatable_name<my_type>())) {
        lua_setmetatable(L, -2);
    }
    return *static_cast<my_type*>(ud);
}
template <class my_type>
my_type& push(lua_State* L) {
    void* ud = lua_newuserdatatagged(L, sizeof(my_type), type_tag<my_type>());
    lua_setuserdatadtor(L, type_tag<my_type>(), [](lua_State* L, void* data) {
        static_cast<my_type*>(data)->~my_type();//cause using placement new, no implicit destruction
    });
    if (luaL_getmetatable(L, metatable_name<my_type>())) {
        lua_setmetatable(L, -2);
    }
    return *static_cast<my_type*>(ud);
}
template <class my_type>
my_type& get(lua_State* L, int objindex) {
    void* ud = lua_touserdatatagged(L, objindex, type_tag<my_type>());
    return *static_cast<my_type*>(ud);
}
}
