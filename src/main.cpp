#include <cassert>
#include "legion/engine.h"
#include "legion/components.h"
#include "legion/event.h"
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
struct Event_data {
    int i{0};
    std::string hello{"hello world"};
};
static event::Event<Event_data> my_event{};
static event::Event<void> my_void_event{};
static event::Connection<Event_data>* my_connection;
void on_render() {
    rnd::set_color(0x0);
    rnd::clear_frame();
    rnd::set_color(0xFFAA1FFF);
    rnd::fill(Recti64{100, 100, 200, 100});
}
void on_event(const Event_data& data) {
    print("yoooooo", data.hello);
}
void on_start() {
    Script script{fs::path("test.luau")};
}
void on_update(double delta_s) {
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
        .update_function = on_update,
        .render_function = on_render,
        .start_function = on_start,
    };
    return engine::bootstrap(opts);
}
