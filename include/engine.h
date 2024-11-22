#pragma once
#include <string>
#include "common.h"
#include "builtin_types.h"
#include <filesystem>
#include <source_location>
struct SDL_Window;
struct lua_State;
namespace engine {
struct LaunchOptions {
    std::string window_name{"luwaw"};
    Vec2i window_size{1920, 1080};
    bool window_resizable{false};
    bool hardware_accelerated{true};
    bool vsync_enabled{true};
    std::filesystem::path main_entry_point{"main.luau"};
    std::filesystem::path bin_path;
};
int bootstrap(LaunchOptions opts = {});
void quit();
SDL_Window* window();
SDL_Renderer* renderer();
lua_State* lua_state();
builtin::Font& default_font();
builtin::Font& debug_font();
//QoL
Vec2i window_size();
void expect(bool expression, std::string_view reason = "not specified", const std::source_location& location = std::source_location::current());
enum class debug_level {
    trace, debug, info, warn, error, critical
};
struct OutputEntry {
    debug_level level;
    std::string data;
};
inline std::vector<OutputEntry> output_history;
template <class ...Ts>
void output(debug_level level, Ts&&...args) {
    OutputEntry entry{.level = level};
    constexpr std::string_view separator = ", ";
    ((entry.data += args += separator), ...);
    for (int i{}; i < separator.length(); ++i) entry.data.pop_back();
    output_history.emplace_back(entry);
}
}
