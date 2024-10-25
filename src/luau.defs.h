#pragma once
#include <lualib.h>
#include "event.h"
namespace luau {
struct Vec2d {
    static void init_type(lua_State* L);
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
struct Vec2i {
    static void init_type(lua_State* L);
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
struct Vec2f {
    static void init_type(lua_State* L);
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
struct Recti64 {
    static void init_type(lua_State* L);
    static int ctor(lua_State* L);
    static int index(lua_State* L);
    static int tostring(lua_State* L);
    static constexpr auto type_name{"Recti64"};
    enum class Field {x, y, width, height};
};
struct Sizei32 {
    static void init_type(lua_State* L);
    static int ctor(lua_State* L);
    static int index(lua_State* L);
    static int newindex(lua_State* L);
    static int tostring(lua_State* L);
    enum class Field{width, height};
    static constexpr auto type_name{"Sizei32"};
};
struct Coloru32 {
    static void init_type(lua_State* L);
    static int ctor(lua_State* L);
    static int index(lua_State* L);
    static int tostring(lua_State* L);
    enum class Field{red, green, blue, alpha};
    static constexpr auto type_name{"Coloru32"};
};
struct Physical {
    static void init_type(lua_State* L);
    static int ctor(lua_State* L);
    static void dtor(lua_State* L, void* ud);
    static int index(lua_State* L);
    static int newindex(lua_State* L);
    static int namecall(lua_State* L);
    enum class Field{position, velocity, acceleration,
        welded, falling, obstructed, elasticity_coeff,
        friction_coeff, mass, size};
    enum class Method{bounds};
    static constexpr auto type_name{"Physical"};
};
struct Clickable {
    static void init_type(lua_State* L);
    static int ctor(lua_State* L);
    static int namecall(lua_State* L);
    enum class Method{on_mouse_click, on_mouse_down};
    static constexpr auto type_name{"Clickable"};
};
namespace renderer {
    void init_lib(lua_State* L);
    int fill(lua_State* L);
    int draw(lua_State* L);
    int render(lua_State* L);
    static constexpr auto lib_name{"renderer"};
}
namespace game {
    void init_lib(lua_State* L);
    inline event::Async<double> update;
    inline event::Async<> render;
    inline event::Async<> start;
    inline event::Async<> quit;
    int on_update(lua_State* L);
    int on_render(lua_State* L);
    int on_start(lua_State* L);
    int on_quit(lua_State* L);
    static constexpr auto lib_name{"game"};
}
}
