#include "engine.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <chrono>
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
#include "lua_util.h"
#include "builtin.h"
#include <Luau/Compiler.h>
#include "lua_atom.h"
#include "comptime_enum.h"
#include "lua_base.h"
#include "builtin_module.h"
#include "util.h"
namespace ch = std::chrono;
namespace fs = std::filesystem;
using czstring = const char*;
namespace bi = builtin;
static SDL_Window* window_ptr{nullptr};
static SDL_Renderer* renderer_ptr{nullptr};
static std::unique_ptr<builtin::font> default_font_ptr{nullptr};
static bool quitting{false};
static bool paused{false};
static SDL_Event sdl_event_dummy{};
static SDL_Rect sdl_rect_dummy{};
static lua_State* main_state;
static fs::path bin_path;
static constexpr auto builtin_name = "engine";
namespace events {
static bi::event* updating;
static bi::event* rendering;
static bi::event* key_pressed;
static bi::event* key_released;
static bi::event* mouse_pressed;
static bi::event* mouse_released;
static bi::event* shutting_down;
}

static fs::path res_path() {
    return bin_path / "resources/luau_library";
}
static czstring mouse_button_to_string(Uint8 button) {
    switch (button) {
        case SDL_BUTTON_RIGHT: return "right";
        case SDL_BUTTON_MIDDLE: return "middle";
        default: return "left";
    }
}

namespace module {
static builtin_module filesystem{"filesystem", builtin::lib_filesystem};
static builtin_module window{"window", builtin::lib_window};
static builtin_module drawing{"drawing", builtin::lib_drawing};
static builtin_module rendering{"rendering", builtin::lib_rendering};
}
namespace callback {
static constexpr auto keyup = "on_keyup";
static constexpr auto keydown = "on_keydown";
static constexpr auto mouseup = "on_mouseup";
static constexpr auto mousedown = "on_mousedown";
static constexpr auto mousescroll = "on_mousescroll";
static constexpr auto mousemotion = "on_mousemotion";
static constexpr auto update = "on_update";
static constexpr auto render = "on_render";
static constexpr auto shutdown = "at_shutdown";
static constexpr auto resize = "at_resize";
static constexpr auto move = "at_move";
static constexpr auto sizechange = "at_sizechange";
}

static bool push_callback(lua_State* L, const char* name) {
    lua_getglobal(L, builtin_name);
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        luaL_error(L, "unexpected error");
        return false;
    }
    lua_getfield(L, -1, name);
    if (not lua_isfunction(L, -1)) {
        lua_pop(L, 2);
        return false;
    }
    lua_remove(L, -2);
    return true;
}
static int lua_load_image(lua_State* L) {
    std::string file{};
    if (is_type<bi::path>(L, 1)) {
        file = check<bi::path>(L, 1).string();
    } else file = luaL_checkstring(L, 1);
    SDL_Surface* loaded = IMG_Load(file.c_str());
    if (not loaded) {
        luaL_error(L, SDL_GetError());
        return 0;
    }
    deferred d{[&loaded]{ SDL_FreeSurface(loaded); }};
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_ptr, loaded);
    if (not texture) {
        luaL_error(L, SDL_GetError());
        return 0;
    }
    create<bi::texture>(L, bi::texture{
        bi::texture_ptr(texture, SDL_DestroyTexture),
        loaded->w,
        loaded->h});
    return 1;
}
static int load_builtin_module(lua_State* L) {
    std::string key = luaL_checkstring(L, 1);
     auto cache_import_module = [&L, &key](builtin_module& module, int(*fn)(lua_State* L)) {
        lua_getglobal(L, module_cache_name);
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            lua_newtable(L);
            lua_setglobal(L, module_cache_name);
            lua_getglobal(L, module_cache_name);
        }
        lua_getfield(L, -1, module.name);
        if (lua_isnil(L, -1)) {
            lua_pop(L, 1);
            if (fn(L) != 1) luaL_error(L, "Library function did not return exactly one value");
            lua_setfield(L, -2, module.name);
            lua_getfield(L, -1, module.name);
        }
        lua_remove(L, -2);
        return 1;
    };
    //if (key == module::filesystem.name) return module::filesystem.load(L);
    //if (key == module::window.name) return module::window.load(L);
    if (key == module::rendering.name) return module::rendering.load(L);
    if (key ==  module::drawing.name) return module::drawing.load(L);
    if (key == "iterator") {
        push_luau_module(L, res_path() / "iterator.luau");
        return 1;
    }
    luaL_error(L, "invalid module name '%s'.", key.c_str());
    return 0;
}
static void init_luau_state(lua_State* L, const fs::path& main_entry_point) {
    luaL_openlibs(main_state);
    lua_callbacks(L)->useratom = [](const char* raw_name, size_t s) {
        std::string_view name{raw_name, s};
        static constexpr auto count = static_cast<size_t>(lua_atom::_last);
        auto e = comptime_enum::item<lua_atom, count>(name);
        return static_cast<int16_t>(e.index);
    };
    //lua_newtable(L);
    //lua_setglobal(L, callbacks_name);
    bi::init_global_types(L);
    lua_register_globals(L);
    const luaL_Reg engine_functions[] = {
        {"get_module", load_builtin_module},
        {"load_image", lua_load_image},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    builtin::lib_window(L);
    lua_setfield(L, -2, "window");
    luaL_register(L, nullptr, engine_functions);
    lua_setglobal(L, builtin_name);
    builtin::lib_filesystem(L);
    lua_setglobal(L, "fs");
    builtin::class_vector2(L);
    lua_setglobal(L, "Vec2");
    builtin::class_vector3(L);
    lua_setglobal(L, "Vec3");
    builtin::class_vector(L);
    lua_setglobal(L, "Vec");
    builtin::class_matrix3(L);
    lua_setglobal(L, "Mat3");
    builtin::class_path(L);
    lua_setglobal(L, "Path");
    push_luau_module(L, res_path() / "task.luau");
    lua_setglobal(L, "task");
    push_luau_module(L, res_path() / "fixed_width_buffer.luau");
    lua_setglobal(L, "fixed_buffer");
    builtin::class_event(L);
    lua_setglobal(L, "Event");
    lua_getglobal(L, builtin_name);
    events::updating = &create<bi::event>(L, L);
    lua_setfield(L, -2, "updating");
    events::rendering = &create<bi::event>(L, L);
    lua_setfield(L, -2, "rendering");
    events::key_pressed = &create<bi::event>(L, L);
    lua_setfield(L, -2, "key_pressed");
    events::key_released = &create<bi::event>(L, L);
    lua_setfield(L, -2, "key_released");
    events::mouse_pressed = &create<bi::event>(L, L);
    lua_setfield(L, -2, "mouse_pressed");
    events::mouse_released = &create<bi::event>(L, L);
    lua_setfield(L, -2, "mouse_released");
    events::shutting_down = &create<bi::event>(L, L);
    lua_setfield(L, -2, "shutting_down");
    lua_pop(L, 1);
    std::optional<std::string> source = read_file(main_entry_point);
    if (not source) {
        using namespace std::string_literals;
        printerr("failed to read the file '"s + main_entry_point.string() + "'");
    } else {
        std::string bytecode = Luau::compile(*source, compile_options());
        if (luau_load(L, "=main_entry_point", bytecode.data(), bytecode.size(), 0)) {
            printerr(luaL_checkstring(L, -1));
        } else {
            if (lua_pcall(L, 0, 0, 0)) {
                printerr(luaL_checkstring(L, -1));
            }
        }
    }
    
    luaL_sandbox(main_state);
}
static void init(engine::start_options opts) {
    SDL_Init(SDL_INIT_VIDEO);// should do proper error handling here
    TTF_Init();
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_WEBP | IMG_INIT_TIF);
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
    const fs::path font_path = fs::absolute(bin_path) / "resources" / "main_font.ttf";
    const int pt_size = 25;
    TTF_Font* font_resource = TTF_OpenFont(font_path.string().c_str(), pt_size);
    if (not font_resource) {
        printerr(SDL_GetError());
        std::abort();
    }
    default_font_ptr = std::make_unique<builtin::font>(builtin::font{
        .ptr{font_resource, TTF_CloseFont},
        .pt_size = pt_size,
        .file_path = font_path
    });
    assert(default_font_ptr->ptr.get());
    main_state = luaL_newstate();
    bin_path = std::move(opts.bin_path);
    init_luau_state(main_state, opts.main_entry_point);
}
static void run() {
    auto cached_last_tp = ch::steady_clock::now();
    auto& e = sdl_event_dummy;
    while (not quitting) {
        {// event handling
            while (SDL_PollEvent(&e)) {
                switch (e.type) {
                    case SDL_QUIT:
                        quitting = true;
                    break;
                    case SDL_KEYDOWN:
                        lua_pushstring(main_state, scancode_to_string(e.key.keysym.scancode));
                        events::key_pressed->fire(1);
                    break;
                    case SDL_KEYUP:
                        lua_pushstring(main_state, scancode_to_string(e.key.keysym.scancode));
                        events::key_released->fire(1);
                    break;
                    case SDL_MOUSEBUTTONUP:
                        lua_pushstring(main_state, mouse_button_to_string(e.button.button));
                        create<bi::vector2>(main_state) = {
                            static_cast<double>(sdl_event_dummy.button.x),
                            static_cast<double>(sdl_event_dummy.button.y)
                        };
                        events::mouse_released->fire(2);
                    break;
                    case SDL_MOUSEBUTTONDOWN:
                        lua_pushstring(main_state, mouse_button_to_string(e.button.button));
                        create<bi::vector2>(main_state) = {
                            static_cast<double>(sdl_event_dummy.button.x),
                            static_cast<double>(sdl_event_dummy.button.y)
                        };
                        events::mouse_pressed->fire(2);
                    break;
                }
            }
        } {//updating
            const auto curr_tp = ch::steady_clock::now();
            const double delta_s = ch::duration<double>(curr_tp - cached_last_tp).count();
            cached_last_tp = curr_tp;
            lua_pushnumber(main_state, delta_s);
            events::updating->fire(1);
        } {//rendering
            events::rendering->fire(0);
            SDL_RenderPresent(renderer_ptr);
        }
    }
}
static void shutdown() {
    events::shutting_down->fire(0);
    lua_close(main_state);
    util::clear_all_caches();
    default_font_ptr.reset();
    SDL_DestroyRenderer(renderer_ptr);
    SDL_DestroyWindow(window_ptr);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
namespace engine {
int bootstrap(start_options opts) {
    init(opts);
    run();
    shutdown();
    return 0;
}
SDL_Window* window() {return window_ptr;}
lua_State* lua_state() {return main_state;}
void quit() {quitting = true;}
builtin::font& default_font() {return *default_font_ptr;}
}