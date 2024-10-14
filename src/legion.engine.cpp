#include "Legion.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <chrono>
#include "ECS.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
namespace ch = std::chrono;
namespace ec = ecs::core;
enum class Procedure {
    Destruction,
    Physical_position_changed,
    count_
};
namespace engine {
struct Passed_functions {
    Update_fn update{nullptr};
    Render_fn render{nullptr};
    Shutdown_fn shutdown{nullptr};
};
//states
static SDL_Window* window_ptr{nullptr};
static SDL_Renderer* renderer_ptr{nullptr};
static bool quit{false};
static bool paused{false};
static SDL_Event sdl_event_dummy{};
static SDL_Rect sdl_rect_dummy{};
static Passed_functions fns{};
using Entity_data = ec::Entity_bundle<Procedure, size_t(Procedure::count_)>;
static std::vector<Entity_data> active_entities;
static lua_State* main_state;
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
void renderer::clear_frame() {
    SDL_RenderClear(renderer_ptr);
}
void renderer::set_color(Coloru32 color) {
    SDL_SetRenderDrawColor(renderer_ptr, color.red(), color.green(), color.blue(), color.alpha());
}
void renderer::draw(const Recti64& rect) {
    sdl_rect_dummy.x = rect.x();
    sdl_rect_dummy.y = rect.y();
    sdl_rect_dummy.w = rect.width();
    sdl_rect_dummy.h = rect.height();
    SDL_RenderDrawRect(renderer_ptr, &sdl_rect_dummy);
}
void renderer::fill(const Recti64& rect) {
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
    if (opts.start_function) opts.start_function();
}
void core::run() {
    auto cached_last_tp = ch::steady_clock::now();
    std::vector<std::pair<Procedure, std::reference_wrapper<Entity_data>>> flagged_entities;
    while (not quit) {
        {// event handling
            while (SDL_PollEvent(&sdl_event_dummy)) {
                if (sdl_event_dummy.type == SDL_QUIT) {
                    quit = true;
                }
            }
        } {//updating
            const auto curr_tp = ch::steady_clock::now();
            const double delta_s = ch::duration<double>(curr_tp - cached_last_tp).count();
            cached_last_tp = curr_tp;
            for (auto& data : active_entities) {
                for (int i{}; i < int(Procedure::count_); ++i) {
                    if (data.flagged_for(Procedure(i))) {
                        flagged_entities.emplace_back(std::make_pair(Procedure(i), std::ref(data)));
                    }
                }
                /*
                const Entity_t entity = data.entity;
                if (auto plr = ec::get_if<Playable>(entity); auto phys = ec::get_if<Physical>(entity)) {
                    player_input_system(plr->get(), phys->get(), delta_s);
                }
                */
            }
            physics_system(ec::component_storage<Physical>().span(), delta_s);
            update_system(ec::component_storage<Updatable>().span(), delta_s);
            if (fns.update) [[likely]] fns.update(delta_s);
        } {//rendering
            SDL_SetRenderDrawColor(renderer_ptr, 0x00, 0x00, 0x00, 0xff);
            SDL_RenderClear(renderer_ptr);
            render_system(ec::component_storage<Renderable>().span());
            if (fns.render) [[likely]] fns.render();
            SDL_RenderPresent(renderer_ptr);
        } {//flag processing
            for (auto& [proc, _data] : flagged_entities) {
                Entity_data& data = _data;
                Entity_t entity = data.entity;
                switch (proc) {
                    case Procedure::Destruction:
                        data.unflag(Procedure::Destruction);
                        ec::clear_entity_components(data.entity);
                        std::swap(data, active_entities.back());
                        active_entities.pop_back();
                    break;
                    case Procedure::Physical_position_changed:
                        data.unflag(Procedure::Physical_position_changed);
                        ec::get<Physical>(entity);
                    break;
                    case Procedure::count_:
                        assert(false);
                    break;
                }
            }
            flagged_entities.clear();
        }
    }
}
void core::shutdown() {
    if (fns.shutdown) [[unlikely]] fns.shutdown();
    ec::component_storage_registry.clear();
    lua_close(main_state);
    SDL_DestroyRenderer(renderer_ptr);
    SDL_DestroyWindow(window_ptr);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
}
