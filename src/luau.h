#pragma once
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <typeinfo>
namespace luau {
namespace intern {
inline int unique_tag_incr{0};
}
template <class T>
int type_tag() {
    static const int tag = intern::unique_tag_incr++; 
    return tag;
}
template <class T>
bool is_type(lua_State* L, int idx) {
    return lua_userdatatag(L, idx) == type_tag<T>();
}
template<class T>
const char* metatable_name() {
    const std::type_info& ti = typeid(T);
    return ti.raw_name();
}
template <class T, class ...Init_arguments_t>
T& init(lua_State* L, Init_arguments_t&&...args) {
    void* ud = lua_newuserdatatagged(L, sizeof(T), type_tag<T>());
    new (ud) T{std::forward<Init_arguments_t>(args)...};
    lua_setuserdatadtor(L, type_tag<T>(), [](lua_State* L, void* data) {
        static_cast<T*>(data)->~T();//cause using placement new, no implicit destruction
    });
    if (luaL_getmetatable(L, metatable_name<T>())) {
        lua_setmetatable(L, -2);
    }
    return *static_cast<T*>(ud);
}
template <class T>
T& init(lua_State* L) {
    void* ud = lua_newuserdatatagged(L, sizeof(T), type_tag<T>());
    lua_setuserdatadtor(L, type_tag<T>(), [](lua_State* L, void* data) {
        static_cast<T*>(data)->~T();//cause using placement new, no implicit destruction
    });
    if (luaL_getmetatable(L, metatable_name<T>())) {
        lua_setmetatable(L, -2);
    }
    return *static_cast<T*>(ud);
}
template <class T>
T& ref(lua_State* L, int objindex) {
    void* ud = lua_touserdatatagged(L, objindex, type_tag<T>());
    return *static_cast<T*>(ud);
}
}
