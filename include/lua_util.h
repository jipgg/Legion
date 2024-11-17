#pragma once
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <typeinfo>
#include <memory>
#include "engine.h"
#include "common.h"
#include <SDL_scancode.h>
[[nodiscard]] const char* scancode_to_string(SDL_Scancode scancode);
[[nodiscard]] SDL_Scancode string_to_scancode(std::string_view string);
std::optional<std::string> resolve_path_type(lua_State* L, int i);
namespace {
constexpr const char* bm_add = "add";
constexpr const char* bm_mod = "mod";
constexpr const char* bm_mul = "mul";
constexpr const char* bm_blend = "blend";
constexpr const char* bm_none = "none";
constexpr const char* bm_invalid = "invalid";
}
[[nodiscard]] __forceinline constexpr const char* blendmode_to_string(SDL_BlendMode bm) {
    switch (bm) {
        case SDL_BLENDMODE_ADD: return "add";
        case SDL_BLENDMODE_MOD: return "mod";
        case SDL_BLENDMODE_MUL: return "mul";
        case SDL_BLENDMODE_BLEND: return "blend";
        case SDL_BLENDMODE_NONE: return "none";
        default: return "invalid";
    }
}
[[nodiscard]] __forceinline constexpr SDL_BlendMode string_to_blendmode(std::string_view string) {
    if (string == bm_none) return SDL_BLENDMODE_NONE;
    else if (string == bm_mul) return SDL_BLENDMODE_MUL;
    else if (string == bm_add) return SDL_BLENDMODE_ADD;
    else if (string == bm_mod) return SDL_BLENDMODE_MOD;
    else if (string == bm_blend) return SDL_BLENDMODE_BLEND;
    else return SDL_BLENDMODE_INVALID;
}
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
    const bool is_full_userdata = lua_isuserdata(L, idx) and lua_userdatatag(L, idx) == type_tag<my_type>();
    const bool is_light_userdata = lua_islightuserdata(L, idx) and lua_lightuserdatatag(L, idx) == type_tag<my_type>(); 
    return is_full_userdata or is_light_userdata;
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
    engine::expect(is_type<my_type>(L, objindex), std::string("builtin type ") + typeid(my_type).name() + " expected."); void* ud;
    if (lua_islightuserdata(L, objindex)) {
        ud = lua_tolightuserdatatagged(L, objindex, type_tag<my_type>());
    } else {
        ud = lua_touserdatatagged(L, objindex, type_tag<my_type>());
    }
    return *static_cast<my_type*>(ud);
}
template <class my_type>
void push(lua_State* L, my_type& userdata) {
    lua_pushlightuserdatatagged(L, &userdata, type_tag<my_type>());
    if (luaL_getmetatable(L, metatable_name<my_type>())) {
        lua_setmetatable(L, -2);
    }
}
namespace lua_err {
__forceinline int invalid_member(lua_State* L, const char* tname) {
    constexpr auto err_index_msg = "invalid %s member '%s'.";
    luaL_error(L, err_index_msg, tname, luaL_checkstring(L, 2));
    return 0;
}
__forceinline int invalid_method(lua_State* L, const char* tname) {
    luaL_error(L, "invalid %s method call '%s'.");
    return 0;
}
__forceinline int out_of_range(lua_State* L, const char* tname) {
    luaL_error(L, "index '%d' is out of range in %s", luaL_checkinteger(L, 2), tname);
    return 0;
}
__forceinline int invalid_type(lua_State* L) {
    luaL_error(L, "invalid type '%s'.", luaL_typename(L, 2));
    return 0;
}
__forceinline int invalid_argument(lua_State* L, int idx, std::optional<const char*> tname = std::nullopt) {
    if (tname) {
        luaL_error(L, "invalid argument %d with type %s, %s expected.", idx, luaL_typename(L, idx), *tname);
    } else {
        luaL_error(L, "invalid argument %d with type %s", idx, luaL_typename(L, idx));
    }
    return 0;
}
}
 __forceinline bool not_in_range(int index, int size, int min = 0) {
    return index >= size or index < 0;
}
