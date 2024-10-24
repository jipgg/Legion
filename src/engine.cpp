#include "engine.h"
#include "systems.h"
#include "types.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <chrono>
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include "event.h"
namespace ch = std::chrono;
namespace ty = types;
namespace comp = component;
struct Passed_functions {
    engine::Update_fn update{nullptr};
    engine::Render_fn render{nullptr};
    engine::Shutdown_fn shutdown{nullptr};
};
//states
static SDL_Window* window_ptr{nullptr};
static SDL_Renderer* renderer_ptr{nullptr};
static bool quitting{false};
static bool paused{false};
static SDL_Event sdl_event_dummy{};
static SDL_Rect sdl_rect_dummy{};
static Passed_functions fns{};
static lua_State* main_state;
namespace engine {
//helpers
SDL_Window* core::get_window() {
    return window_ptr;
}
lua_State* core::get_lua_state() {
    return main_state;
}
//
int bootstrap(Start_options opts) {
    core::start(opts);
    core::run();
    core::shutdown();
    return 0;
}
void quit() {
    quitting = true;
}
void renderer::clear_frame() {
    SDL_RenderClear(renderer_ptr);
}
void renderer::set_color(common::Coloru32 color) {
    SDL_SetRenderDrawColor(renderer_ptr, color.red(), color.green(), color.blue(), color.alpha());
}
void renderer::draw(const common::Recti64& rect) {
    sdl_rect_dummy.x = rect.x();
    sdl_rect_dummy.y = rect.y();
    sdl_rect_dummy.w = rect.width();
    sdl_rect_dummy.h = rect.height();
    SDL_RenderDrawRect(renderer_ptr, &sdl_rect_dummy);
}
void renderer::fill(const common::Recti64& rect) {
    sdl_rect_dummy.x = rect.x();
    sdl_rect_dummy.y = rect.y();
    sdl_rect_dummy.w = rect.width();
    sdl_rect_dummy.h = rect.height();
    SDL_RenderFillRect(renderer_ptr, &sdl_rect_dummy);
}
void core::start(Start_options opts) {
    SDL_Init(SDL_INIT_VIDEO);// should do proper error handling here
    TTF_Init();
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    main_state = luaL_newstate();
    luaL_openlibs(main_state);
    constexpr int undefined = SDL_WINDOWPOS_UNDEFINED;
    const int width = opts.window_size.at(0);
    const int height = opts.window_size.at(1);
    uint32_t window_flags = SDL_WINDOW_SHOWN;
    if (opts.window_resizable) window_flags |= SDL_WINDOW_RESIZABLE;
    window_ptr = SDL_CreateWindow(opts.window_name.c_str(), undefined, undefined, width, height, window_flags);
    uint32_t renderer_flags = 0;
    if (opts.hardware_accelerated) renderer_flags |= SDL_RENDERER_ACCELERATED;
    if (opts.vsync_enabled) renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
    renderer_ptr = SDL_CreateRenderer(window_ptr, -1, renderer_flags);
    fns.render = opts.render_function ? opts.render_function : nullptr;
    fns.update = opts.update_function ? opts.update_function : nullptr;
    fns.shutdown = opts.shutdown_function ? opts.shutdown_function : nullptr;
    if (opts.start_function) [[likely]] opts.start_function();
}
void core::run() {
    auto cached_last_tp = ch::steady_clock::now();
    while (not quitting) {
        {// event handling
            while (SDL_PollEvent(&sdl_event_dummy)) {
                switch (sdl_event_dummy.type) {
                    case SDL_QUIT:
                        quitting = true;
                    break;
                    case SDL_MOUSEBUTTONUP:
                        systems::process_mouse_up(comp::view<ty::Clickable>(), sdl_event_dummy.button);
                    break;
                    case SDL_MOUSEBUTTONDOWN:
                        systems::process_mouse_down(comp::view<ty::Clickable>(), sdl_event_dummy.button);
                    break;
                }
            }
            event::process_event_stack_entries(5);
        } {//updating
            const auto curr_tp = ch::steady_clock::now();
            const double delta_s = ch::duration<double>(curr_tp - cached_last_tp).count();
            cached_last_tp = curr_tp;
            systems::physics(comp::view<ty::Physical>(), delta_s);
            systems::update(comp::view<ty::Updatable>(), delta_s);
            if (fns.update) [[likely]] fns.update(delta_s);
        } {//rendering
            SDL_SetRenderDrawColor(renderer_ptr, 0x00, 0x00, 0x00, 0xff);
            SDL_RenderClear(renderer_ptr);
            if (fns.render) [[likely]] fns.render();
            systems::render(comp::view<ty::Renderable>());
            SDL_RenderPresent(renderer_ptr);
        }
        event::process_event_stack_entries(5);
    }
}
void core::shutdown() {
    if (fns.shutdown) [[unlikely]] fns.shutdown();
    //lua_close(main_state); //this slows down shutdown time significantly
    SDL_DestroyRenderer(renderer_ptr);
    SDL_DestroyWindow(window_ptr);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
}
