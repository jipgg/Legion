#pragma once
#include <string>
#include "common.h"
#include <filesystem>
#include "builtin.h"
#include <source_location>
struct SDL_Window;
struct lua_State;
namespace engine {
struct start_options {
    std::string window_name{"Legion"};
    vec2i window_size{1240, 720};
    bool window_resizable{false};
    bool hardware_accelerated{true};
    bool vsync_enabled{true};
    std::filesystem::path main_entry_point{"main.luau"};
    std::filesystem::path bin_path;
};
int bootstrap(start_options opts = {});
void quit();
SDL_Window* window();// not preferrable, but need it for textures
lua_State* lua_state();
builtin::font& default_font();
void expect(bool expression, std::string_view reason = "not specified", const std::source_location& location = std::source_location::current());
}
