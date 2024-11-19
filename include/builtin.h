#pragma once
#include "common.h"
#include <memory>
#include <SDL_render.h>
#include <SDL_ttf.h>
#include <blaze/Blaze.h>
#include <lualib.h>
namespace builtin {

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

int fn_read_file(lua_State* L);
int fn_get_mouse_position(lua_State* L);
int fn_is_key_down(lua_State* L);
int fn_is_point_in_rect(lua_State* L);

int files_module(lua_State* L);

int window_module(lua_State* L);
void handle_window_event(lua_State* L, SDL_WindowEvent& e);

int rendering_module(lua_State* L);
int graphics_module(lua_State* L);
int renderer_module(lua_State* L);
int userinput_module(lua_State* L);
void handle_userinput_event(lua_State* L, SDL_Event& e);
}
