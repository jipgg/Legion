#pragma once
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <typeinfo>
#include <memory>
#include "engine.h"
#include "common.h"
#include <SDL_scancode.h>
const char* scancode_to_string(SDL_Scancode scancode);
SDL_Scancode string_to_scancode(std::string_view string);
std::optional<std::string> resolve_path_type(lua_State* L, int i);
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
    return lua_isuserdata(L, idx) and lua_userdatatag(L, idx) == type_tag<my_type>();
}
template<class my_type>
const char* metatable_name() {
    const std::type_info& ti = typeid(my_type);
    return ti.raw_name();
}
template <class my_type, class ...init_arguments>
my_type& create(lua_State* L, init_arguments&&...args) {
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
my_type& create(lua_State* L) {
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
my_type& check(lua_State* L, int objindex) {
    engine::expect(is_type<my_type>(L, objindex), std::string("builtin type ") + typeid(my_type).name() + " expected.");
    void* ud = lua_touserdatatagged(L, objindex, type_tag<my_type>());
    return *static_cast<my_type*>(ud);
}
[[nodiscard]] inline int err_invalid_member(lua_State* L, const char* tname) {
    constexpr auto err_index_msg = "invalid %s member '%s'.";
    luaL_error(L, err_index_msg, tname, luaL_checkstring(L, 2));
    return 0;
}
[[nodiscard]] inline int err_invalid_method(lua_State* L, const char* tname) {
    luaL_error(L, "invalid %s method '%s'.");
    return 0;
}
[[nodiscard]] inline int err_out_of_range(lua_State* L, const char* tname) {
    luaL_error(L, "index '%d' is out of range in %s", luaL_checkinteger(L, 2), tname);
    return 0;
}
[[nodiscard]] inline int err_invalid_type(lua_State* L) {
    luaL_error(L, "invalid type '%s'.", luaL_typename(L, 2));
    return 0;
}
[[nodiscard]] inline bool not_in_range(int index, int size, int min = 0) {
    return index >= size or index < 0;
}
