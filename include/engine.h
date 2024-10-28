#pragma once
#include <string>
#include "common/common.h"
#include "types.h"
struct SDL_Window;
struct lua_State;
namespace engine {
struct engine_start_options {
    std::string window_name{"Legion"};
    common::vec2i window_size{1240, 720};
    bool window_resizable{false};
    bool hardware_accelerated{true};
    bool vsync_enabled{true};
};
int bootstrap(engine_start_options opts = {});
void quit();
namespace renderer {
void clear_frame();
void draw(const common::recti64& rect);
void fill(const common::recti64& rect);
void set_color(common::coloru32 color);
}//renderer end
namespace core {
void start(engine_start_options opts = {});
void run();
void shutdown();
SDL_Window* window();// not preferrable, but need it for textures
lua_State* lua_state();
}//core end
}
