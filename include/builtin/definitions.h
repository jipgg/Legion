#pragma once
#include <lualib.h>
namespace builtin {
void init_global_types(lua_State* L);
void vec2i_init_type(lua_State* L);
void vec2d_init_type(lua_State* L);
void mat3x3_init_type(lua_State* L);
void physical_init_type(lua_State* L);
void fs_init_lib(lua_State* L);

int sdl_import_lib(lua_State* L);
int fs_import_lib(lua_State* L);

int read_file(lua_State* L);
int get_mouse_position(lua_State* L);
int is_key_down(lua_State* L);
enum class vec2_field {x, y};
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
constexpr auto call = "__call";
constexpr auto pow = "__pow";
}
}
