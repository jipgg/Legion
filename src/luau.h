#pragma once
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <typeinfo>
#include <memory>
#include "engine.h"
#include "common.h"
#include <SDL_scancode.h>
namespace luau {
const char* scancode_to_string(SDL_Scancode scancode);
SDL_Scancode string_to_scancode(std::string_view string);
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
using Fn_ref = std::shared_ptr<int>;
template <class ...Init_arguments_t>
Fn_ref make_fn_ref(Init_arguments_t...args) {return std::make_shared<int>(std::forward<Init_arguments_t>(args)...);}
template<class ...Params>
struct Function {
    Fn_ref fn_ref;
    lua_State* state() {return engine::core::get_lua_state();}
    virtual void operator()(Params...args) = 0;
    Function(Fn_ref&& fn): fn_ref(fn) {}
    ~Function() {
        if (fn_ref.use_count() == 1) {
            common::print("Destroyed");
            lua_unref(state(), *fn_ref);
        }
    }
};
}
