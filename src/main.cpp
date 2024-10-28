#include <cassert>
#include "engine.h"
#include <lua.h>
#include <lualib.h>
#include <blaze/Blaze.h>
#include <SDL_main.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
using namespace std::string_view_literals;

int main(int, char**) {
    #ifdef _WIN32
    common::attach_console();
    common::enable_ansi_escape_sequences();
    #endif
    const engine::engine_start_options opts {
        .window_name{"Legion"},
        .window_size{800, 600},
        .window_resizable = true,
        .hardware_accelerated = true,
        .vsync_enabled = true,
    };
    return engine::bootstrap(opts);
}
