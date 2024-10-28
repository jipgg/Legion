#pragma once
#include <lualib.h>
namespace luau {
namespace event_sockets {
constexpr auto update = "__builtin_update_socket";
constexpr auto shutdown = "__builtin_shutdown_socket";
constexpr auto render = "__builtin_render_socket";
constexpr auto mouse_up = "__builtin_mouseup_socket";
constexpr auto mouse_down = "__builtin_mousedown_socket";
constexpr auto mouse_motion = "__builtin_mousemoton_socket";
constexpr auto mouse_scroll = "__builtin_mousescroll_socket";
constexpr auto key_down = "__builtin_keydown_socket";
constexpr auto key_up = "__builtin_keyup_socket";
}
namespace metamethod {
constexpr auto type = "__type";
constexpr auto namecall = "__namecall";
constexpr auto index = "__index";
constexpr auto newindex = "__newindex";
constexpr auto add = "__add";
constexpr auto sub = "__sub";
constexpr auto unm = "__unm";
constexpr auto mul = "__mul";
constexpr auto div = "__div";
constexpr auto tostring = "__tostring";
constexpr auto iter = "__iter";
constexpr auto len = "__len";
}
enum class Method_atom {
    /*Vec2*/ dot, unit, abs, magnitude,
    /*fs_path*/ stem, empty, filename,
    has_stem, root_path, parent_path, is_relative,
    is_absolute, has_extension, extension, replace_extension,
    relative_path, has_relative_path, compare, root_name,
    root_directory, has_root_path, has_root_name, has_root_directory,
    /*fs_directory_entry*/ is_directory, is_fifo, path, is_socket,
    is_other, is_regular_file, is_character_file, is_symlink,
    is_block_file,
    /*Rect*/ bounds /*last one always*/
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
    static constexpr auto type_name{"vec2d"};
    enum class Field {x, y};
};
struct Rect {
    static void init_type(lua_State* L);
    static int ctor(lua_State* L);
    static int index(lua_State* L);
    static int tostring(lua_State* L);
    static constexpr auto type_name{"recti64"};
    enum class Field {x, y, width, height};
};
struct Color {
    static void init_type(lua_State* L);
    static int ctor(lua_State* L);
    static int index(lua_State* L);
    static int tostring(lua_State* L);
    enum class Field{red, green, blue, alpha};
    static constexpr auto type_name{"coloru32"};
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
    static constexpr auto type_name{"physical_component"};
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
namespace fs {
    void init_lib(lua_State* L);
    static constexpr auto lib_name{"fs"};
}
}
