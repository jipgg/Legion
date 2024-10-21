#pragma once
struct lua_State;
namespace luau {
struct V2 {
    static void init_metadata(lua_State* L);
    static int ctor(lua_State* L);
    static int add(lua_State* L);
    static int mul(lua_State* L);
    static int div(lua_State* L);
    static int sub(lua_State* L);
    static int index(lua_State* L);
    static int unm(lua_State* L);
    static int newindex(lua_State* L);
    static int metatable(lua_State* L);
    static int namecall(lua_State* L);
    static int tostring(lua_State* L);
    static constexpr auto type_name{"V2"};
    enum class Method {dot, unit, abs, magnitude};
    enum class Field {x, y};
};
struct Rect {
    static void init_metadata(lua_State* L);
    static int ctor(lua_State* L);
    static int index(lua_State* L);
    static int tostring(lua_State* L);
    static constexpr auto type_name{"Rect"};
    enum class Field {x, y, width, height};
};
}/*luau end*/
