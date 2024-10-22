#pragma once
struct lua_State;
namespace legion {
struct Lu_vec2d {
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
    static constexpr auto type_name{"Vec2d"};
    enum class Method {dot, unit, abs, magnitude};
    enum class Field {x, y};
};
struct Lu_vec2i {
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
    static constexpr auto type_name{"Vec2i"};
    enum class Method {dot, unit, abs, magnitude};
    enum class Field {x, y};
};
struct Lu_vec2f {
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
    static constexpr auto type_name{"Vec2f"};
    enum class Method {dot, unit, abs, magnitude};
    enum class Field {x, y};
};
struct Lu_recti64 {
    static void init_metadata(lua_State* L);
    static int ctor(lua_State* L);
    static int index(lua_State* L);
    static int tostring(lua_State* L);
    static constexpr auto type_name{"Recti64"};
    enum class Field {x, y, width, height};
};
struct Lu_sizei32 {
    static void init_metadata(lua_State* L);
    static int ctor(lua_State* L);
    static int index(lua_State* L);
    static int newindex(lua_State* L);
    static int tostring(lua_State* L);
    enum class Field{width, height};
    static constexpr auto type_name{"Sizei32"};
};
}
