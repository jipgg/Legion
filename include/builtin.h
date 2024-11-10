#pragma once
#include "common.h"
#include <memory>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <blaze/Blaze.h>
#include <lualib.h>
//#include "lua_event.h"
namespace builtin {
void init_global_types(lua_State* L);
void physical_init_type(lua_State* L);
void init_filesystem_lib(lua_State* L);

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
int class_matrix33(lua_State* L);
int class_vector2(lua_State* L);
int class_vector2i(lua_State* L);
int class_vector3(lua_State* L);
int class_vector(lua_State* L);
int class_path(lua_State* L);
int class_event(lua_State* L);

int fn_read_file(lua_State* L);
int fn_get_mouse_position(lua_State* L);
int fn_is_key_down(lua_State* L);
int fn_is_point_in_rect(lua_State* L);

int lib_sdl(lua_State* L);
int lib_filesystem(lua_State* L);
int lib_window(lua_State* L);
int lib_rendering(lua_State* L);
int lib_drawing(lua_State* L);

namespace tname {
constexpr auto opaque_texture = "Texture_ptr";
constexpr auto color = "Color";
constexpr auto opaque_font = "Font_ptr";
constexpr auto rectangle = "Rectangle";
constexpr auto vertex = "Vertex";
constexpr auto matrix33 = "Matrix33";
constexpr auto vector = "Vector";
constexpr auto vector2 = "Vector2";
constexpr auto vector3 = "Vector3";
static constexpr auto path = "Path";
static constexpr auto directory_entry = "Directory_entry";
static constexpr auto event = "Event";
static constexpr auto connection = "Connection";
static constexpr auto signal = "Signal";
}
using opaque_texture = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
using opaque_font = std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>;
struct rectangle {double x, y, w, h;};
using color = SDL_Color;
using vector2 = vec2d;
using vector3 = blaze::StaticVector<double, 3, blaze::defaultTransposeFlag, blaze::aligned,
    blaze::unpadded /*not 100% sure why but this causes issues when turning ud back to blaze type if set padded*/>; 
using vector = blaze::DynamicVector<double>;
using matrix33 = mat3x3;
using path = std::filesystem::path;
using directory_entry = std::filesystem::directory_entry;
//using event = lua_event;
//using connection = lua_event::connection;
//using signal = lua_event::signal;
}
