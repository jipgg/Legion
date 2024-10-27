#include <cassert>
#include "engine.h"
#include "types.h"
#include <lua.h>
#include <lualib.h>
#include <blaze/Blaze.h>
#include <SDL_main.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
using namespace std::string_view_literals;
namespace rnd = engine::renderer;
namespace fs = std::filesystem;
namespace ty = types;
void on_render() {
    rnd::set_color(0x0);
    rnd::clear_frame();
    rnd::set_color(0xFFAA1FFF);
    rnd::fill(common::Recti64{100, 100, 200, 100});
}
ty::Script* script;
void on_start() {
    //script = new ty::Script{fs::path("test.luau")};
}
void on_update(double delta_s) {
}

int main(int, char**) {
    #ifdef _WIN32
    common::attach_console();
    common::enable_ansi_escape_sequences();
    #endif
    const engine::Start_options opts {
        .window_name{"Legion"},
        .window_size{800, 600},
        .window_resizable = true,
        .hardware_accelerated = true,
        .vsync_enabled = true,
        .update_function = on_update,
        .render_function = on_render,
        .start_function = on_start,
    };
    return engine::bootstrap(opts);
}
