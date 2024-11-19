#include <cassert>
#include "engine.h"
#include <lua.h>
#include <lualib.h>
#include <blaze/Blaze.h>
#include <SDL_main.h>
#include <filesystem>
#include "util.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
using namespace std::string_view_literals;
namespace fs = std::filesystem;

using zstring = char*;
int main(int argc, zstring* argv) {
    #ifdef _WIN32
    attach_console();
    enable_ansi_escape_sequences();
    #endif
    engine::LaunchOptions opts{
        .window_name{"luwaw!"},
        .window_size{800, 600},
        .window_resizable = true,
        .hardware_accelerated = true,
        .vsync_enabled = true,
        .main_entry_point = "../tests/dummy_game.luau",
        .bin_path = util::get_executable_path(),
    };
    if (argc > 1) {
        opts.main_entry_point = argv[1];
    }
    return engine::bootstrap(opts);
}
