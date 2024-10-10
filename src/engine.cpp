#include "legion.h"
#include <SDL.h>
#include <SDL_ttf.h>
using namespace std::chrono_literals;

static SDL_Window* game_window{nullptr};
static SDL_Renderer* game_renderer{nullptr};
static bool quit{false};
static bool paused{false};
static SDL_Event e{};

namespace engine {
int bootstrap(Start_options opts) {
    core::start(opts);
    core::run();
    core::shutdown();
    return 0;
}
void core::start(Start_options opts) {
    #ifdef _WIN32
    attach_console();
    #endif
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    constexpr int undefined = SDL_WINDOWPOS_UNDEFINED;
    const int width = opts.window_size.at(0);
    const int height = opts.window_size.at(1);
    u32 window_flags = SDL_WINDOW_SHOWN;
    if (opts.window_resizable) window_flags |= SDL_WINDOW_RESIZABLE;
    game_window = SDL_CreateWindow(opts.window_name.c_str(), undefined, undefined, width, height, window_flags);
    u32 renderer_flags = 0;
    if (opts.hardware_accelerated) renderer_flags |= SDL_RENDERER_ACCELERATED;
    if (opts.vsync_enabled) renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
    game_renderer = SDL_CreateRenderer(game_window, -1, renderer_flags);
}
void core::run() {
    while (not quit) {
        {// event handling
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
            }
        } {//update
        } {//rendering
            SDL_RenderClear(game_renderer);
            SDL_RenderPresent(game_renderer);
        }
    }
}
void core::shutdown() {
    SDL_DestroyRenderer(game_renderer);
    SDL_DestroyWindow(game_window);
    TTF_Quit();
    SDL_Quit();
}
}
