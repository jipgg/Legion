#include <cassert>
#include "Legion.h"
#include <lua.h>
#include <lualib.h>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
using namespace std::string_view_literals;
namespace rnd = engine::renderer;
void on_render() {
    rnd::set_color(0x0);
    rnd::clear_frame();
    rnd::set_color(0xff'aa'1f'ff);
    rnd::fill(Recti64{100, 100, 200, 100});
}
void on_start() {
    Script script{fs::path("test.luau")};
}

int main(int, char**) {
    #ifdef _WIN32
    attach_console();
    enable_ansi_escape_sequences();
    #endif
    const engine::Start_options opts {
        .window_name{"Legion"},
        .window_size{800, 600},
        .window_resizable = true,
        .hardware_accelerated = true,
        .vsync_enabled = true,
        .render_function = on_render,
        .start_function = on_start,
    };
    return engine::bootstrap(opts);
}
