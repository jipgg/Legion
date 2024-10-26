#pragma once
#include <lualib.h>
namespace luau {
namespace handler_keys {
constexpr auto update = "__legion_update_handler";
constexpr auto start = "__legion_start_handler";
constexpr auto shutdown = "__legion_shutdown_handler";
constexpr auto render = "__legion_render_handler";
constexpr auto mouse_button_up = "__legion_mouse_button_up_handler";
constexpr auto mouse_button_down = "__legion_mouse_button_down_handler";
}
enum class Method_atom {
    /*Vec2*/ dot, unit, abs, magnitude,
    /*Rect*/ bounds
};
struct Vec2 {
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
    static constexpr auto type_name{"Vec2"};
    enum class Field {x, y};
};
struct Rect {
    static void init_type(lua_State* L);
    static int ctor(lua_State* L);
    static int index(lua_State* L);
    static int tostring(lua_State* L);
    static constexpr auto type_name{"Rect"};
    enum class Field {x, y, width, height};
};
struct Color {
    static void init_type(lua_State* L);
    static int ctor(lua_State* L);
    static int index(lua_State* L);
    static int tostring(lua_State* L);
    enum class Field{red, green, blue, alpha};
    static constexpr auto type_name{"Color"};
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
    static constexpr auto type_name{"Physical"};
};
namespace renderer {
    void init_lib(lua_State* L);
    int fill(lua_State* L);
    int draw(lua_State* L);
    int render(lua_State* L);
    static constexpr auto lib_name{"renderer"};
}
namespace builtin {
    void init_lib(lua_State* L);
    static constexpr auto lib_name{"builtin"};
}
}
