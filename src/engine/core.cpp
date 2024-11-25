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
#include "builtin_types.h"
#include <Luau/Compiler.h>
#include "lua_atom.h"
#include "comptime_enum.h"
#include "lua_base.h"
#include "BuiltinModule.h"
#include "util.h"
#include <stdexcept>
namespace ch = std::chrono;
namespace fs = std::filesystem;
using czstring = const char*;
static constexpr auto builtin_name = "waw";
static SDL_Window* window_ptr{nullptr};
static SDL_Renderer* renderer_ptr{nullptr};
static std::unique_ptr<builtin::Font> default_font_ptr{nullptr};
static bool quitting{false};
static bool paused{false};
static SDL_Event sdl_event_dummy{};
static SDL_Rect sdl_rect_dummy{};
static lua_State* main_state;
static fs::path bin_path;

static fs::path res_path() {
    return bin_path / "resources/luau_library";
}
namespace module {
static BuiltinModule filesystem{"Files", builtin::files_module};
static BuiltinModule window{"window", builtin::window_module};
static BuiltinModule graphics{"Graphics", builtin::graphics_module};
static BuiltinModule userinput("userinput", builtin::userinput_module);
static BuiltinModule rendering{"rendering", builtin::rendering_module};
}
namespace events {
static UniqueEvent shutting_down;
static UniqueEvent updating;
static UniqueEvent rendering;
static UniqueEvent run_begin;
static UniqueEvent run_done;
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
    if (is_type<builtin::files::Path>(L, 1)) {
        file = check<builtin::files::Path>(L, 1).string();
    } else file = luaL_checkstring(L, 1);
    SDL_Surface* loaded = IMG_Load(file.c_str());
    if (not loaded) {
        luaL_error(L, SDL_GetError());
        return 0;
    }
    ScopeGuard d{[&loaded]{ SDL_FreeSurface(loaded); }};
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_ptr, loaded);
    if (not texture) {
        luaL_error(L, SDL_GetError());
        return 0;
    }
    create<builtin::Texture>(L, builtin::Texture{
        std::shared_ptr<SDL_Texture>(texture, SDL_DestroyTexture),
        loaded->w,
        loaded->h});
    return 1;
}
static int load_builtin_module(lua_State* L) {
    std::string key = luaL_checkstring(L, 1);
     auto cache_import_module = [&L, &key](BuiltinModule& module, int(*fn)(lua_State* L)) {
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
    if (key == module::filesystem.name) return module::filesystem.load(L);
    //if (key == module::window.name) return module::window.load(L);
    if (key == module::rendering.name) return module::rendering.load(L);
    if (key ==  module::graphics.name) return module::graphics.load(L);
    if (key == module::userinput.name) return module::userinput.load(L);
    luaL_error(L, "invalid module name '%s'.", key.c_str());
    return 0;
}
static void register_builtin_module(lua_State* L, BuiltinModule& module) {
    module.load(L);
    lua_setfield(L, -2, module.name);
}
static void init_luau_state(const fs::path& main_entry_point) {
    luaL_openlibs(main_state);
    lua_callbacks(main_state)->useratom = [](const char* raw_name, size_t s) {
        std::string_view name{raw_name, s};
        static constexpr auto count = static_cast<size_t>(lua_atom::_last);
        try {
            auto e = comptime_enum::item<lua_atom, count>(name);
            return static_cast<int16_t>(e.index);
        } catch (std::out_of_range& e) {
            luaL_error(main_state, e.what());
            return 0i16;
        }
    };
    lua_register_globals(main_state);
    const luaL_Reg engine_functions[] = {
        {"get_builtin_module", load_builtin_module},
        {nullptr, nullptr}
    };
    lua_newtable(main_state);
    luaL_register(main_state, nullptr, engine_functions);
    lua_setglobal(main_state, builtin_name);
    {
        using namespace builtin;
        register_mat3_type(main_state);
        register_vec2_type(main_state);
        register_vec3_type(main_state);
        register_vec_type(main_state);
        //register_file_path_type(main_state);
        register_event_type(main_state);
        register_font_type(main_state);
        register_texture_type(main_state);
        register_color_type(main_state);
        register_rect_type(main_state);
        register_recti_type(main_state);
    }
    lua_getglobal(main_state, builtin_name);
    //register_builtin_module(main_state, module::filesystem);
    //register_builtin_module(main_state, module::graphics);
    register_builtin_module(main_state, module::userinput);
    register_builtin_module(main_state, module::window);
    register_event(main_state, events::run_begin, "beforeRun");
    register_event(main_state, events::run_done, "afterRun");
    register_event(main_state, events::updating, "onUpdate");
    register_event(main_state, events::rendering, "onRender");
    register_event(main_state, events::shutting_down, "onQuit");
    lua_pop(main_state, 1);
    std::optional<std::string> source = read_file(main_entry_point);
    if (not source) {
        using namespace std::string_literals;
        printerr("failed to read the file '"s + main_entry_point.string() + "'");
    } else {
        auto identifier = main_entry_point.filename().string();
        identifier = "=" + identifier;
        std::string bytecode = Luau::compile(*source, compile_options());
        if (luau_load(main_state, identifier.c_str(), bytecode.data(), bytecode.size(), 0)) {
            printerr(luaL_checkstring(main_state, -1));
        } else {
            if (lua_pcall(main_state, 0, 0, 0)) {
                printerr(luaL_checkstring(main_state, -1));
            }
        }
    }
    
    luaL_sandbox(main_state);
}
static void init(engine::LaunchOptions opts) {
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
    const int pt_size = 16;
    TTF_Font* font_resource = TTF_OpenFont(font_path.string().c_str(), pt_size);
    if (not font_resource) {
        printerr(SDL_GetError());
        assert(false);
    }
    default_font_ptr = std::make_unique<builtin::Font>(builtin::Font{
        .ptr{font_resource, TTF_CloseFont},
        .pt_size = pt_size,
        .path = font_path
    });
    engine::expect(default_font_ptr->ptr.get());
    main_state = luaL_newstate();
    bin_path = std::move(opts.bin_path);
    init_luau_state(opts.main_entry_point);
}
static void run() {
    auto cached_last_tp = ch::steady_clock::now();
    auto& e = sdl_event_dummy;
    while (not quitting) {
        {// event handling
            events::run_begin->fire(0);
            while (SDL_PollEvent(&e)) {
                if (module::userinput.loaded) {
                    builtin::handle_userinput_event(main_state, e);
                }
                switch (e.type) {
                    case SDL_QUIT:
                        quitting = true;
                    break;
                    case SDL_WINDOWEVENT:
                        if (module::window.loaded) {
                            builtin::handle_window_event(main_state, e.window);
                        }
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
        events::run_done->fire(0);//maybe time point as parameter
    }
}
static void shutdown() {
    events::shutting_down->fire(0);
    lua_close(main_state);
    util::clear_all_cache_registries();
    default_font_ptr.reset();
    SDL_DestroyRenderer(renderer_ptr);
    SDL_DestroyWindow(window_ptr);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
namespace engine {
int bootstrap(LaunchOptions opts) {
    init(opts);
    run();
    shutdown();
    return 0;
}
SDL_Window* window() {return window_ptr;}
SDL_Renderer* renderer() {return renderer_ptr;}
lua_State* lua_state() {return main_state;}
void quit() {quitting = true;}
builtin::Font& default_font() {return *default_font_ptr;}
builtin::Font& debug_font() {return *default_font_ptr;}
void expect(bool expression, std::string_view reason, const std::source_location& location) {
    if (expression) return;
    luaL_error(main_state, "Failed expected precondition. Luau state terminated.\n-> Reason: %s\n-> In file: %s\n-> At line: %d (column: %d) \n-> In function: %s",
        std::string(reason).c_str(), location.file_name(), location.line(), location.column(), location.function_name());
}
}
