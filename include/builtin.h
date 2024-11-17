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
int class_matrix3(lua_State* L);
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
int lib_graphics(lua_State* L);

namespace tname {
constexpr auto opaque_texture = "TexturePtr";
constexpr auto color = "Color";
constexpr auto opaque_font = "FontPtr";
constexpr auto rectangle = "Rectangle";
constexpr auto vertex = "Vertex";
constexpr auto matrix3 = "Matrix3";
constexpr auto vector = "Vector";
constexpr auto vector2 = "Vector2";
constexpr auto vector3 = "Vector3";
static constexpr auto path = "FilePath";
static constexpr auto directory_entry = "DirectoryEntry";
static constexpr auto event = "Event";
static constexpr auto texture = "Texture";
}
using texture_ptr = std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>;
using font_ptr = std::unique_ptr<TTF_Font, decltype(&TTF_CloseFont)>;
struct rectangle {double x, y, w, h;};
using color = SDL_Color;
using vector2 = vec2d;
using vector3 = blaze::StaticVector<double, 3, blaze::defaultTransposeFlag, blaze::aligned,
    blaze::unpadded /*not 100% sure why but this causes issues when turning ud back to blaze type if set padded*/>; 
using vector = blaze::DynamicVector<double>;
using matrix3 = mat3x3;
using path = std::filesystem::path;
using directory_entry = std::filesystem::directory_entry;
struct event {
    struct connection {
        size_t id;
    };
    std::vector<std::pair<size_t, int>> refs;
    lua_State* L;
    static constexpr size_t nullid = 0;
    size_t curr_id{nullid};
    event(lua_State* L);
    ~event();
    connection connect(int idx);
    void disconnect(connection id);
    void fire(int arg_count);
};
struct texture {
    texture_ptr ptr;
    int w, h;
};
struct font {
    font_ptr ptr;
    int pt_size;
    path file_path;
};
}
