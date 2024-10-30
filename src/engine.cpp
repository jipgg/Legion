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
#include "builtin/definitions.h"
#include "builtin/utility.h"
#include <Luau/Common.h>
#include <Luau/Compiler.h>
#include <Luau/CodeGen.h>
#include "builtin/method_atom.h"
#include "Require.h"
#include "common/comptime.h"
namespace ch = std::chrono;
namespace ty = types;

namespace comp = component;
namespace fs = std::filesystem;
namespace bi = builtin;
namespace cm = common;
static bool codegen = false;
//states
static SDL_Window* window_ptr{nullptr};
static SDL_Renderer* renderer_ptr{nullptr};
static bool quitting{false};
static bool paused{false};
static SDL_Event sdl_event_dummy{};
static SDL_Rect sdl_rect_dummy{};
static lua_State* main_state;
struct GlobalOptions {
    int optimizationLevel = 1;
    int debugLevel = 1;
} globalOptions;
static Luau::CompileOptions copts(){
    Luau::CompileOptions result = {};
    result.optimizationLevel = globalOptions.optimizationLevel;
    result.debugLevel = globalOptions.debugLevel;
    result.typeInfoLevel = 1;
    return result;
}
static int lua_loadstring(lua_State* L) {
    size_t l = 0;
    const char* s = luaL_checklstring(L, 1, &l);
    const char* chunkname = luaL_optstring(L, 2, s);

    lua_setsafeenv(L, LUA_ENVIRONINDEX, false);

    std::string bytecode = Luau::compile(std::string(s, l), copts());
    if (luau_load(L, chunkname, bytecode.data(), bytecode.size(), 0) == 0)
        return 1;

    lua_pushnil(L);
    lua_insert(L, -2); // put before error message
    return 2;          // return nil plus error message
}
static int finishrequire(lua_State* L) {
    if (lua_isstring(L, -1))
        lua_error(L);
    return 1;
}
static int lua_require(lua_State* L) {
    std::string name = luaL_checkstring(L, 1);

    RequireResolver::ResolvedRequire resolvedRequire = RequireResolver::resolveRequire(L, std::move(name));

    if (resolvedRequire.status == RequireResolver::ModuleStatus::Cached)
        return finishrequire(L);
    else if (resolvedRequire.status == RequireResolver::ModuleStatus::NotFound)
        luaL_errorL(L, "error requiring module");

    // module needs to run in a new thread, isolated from the rest
    // note: we create ML on main thread so that it doesn't inherit environment of L
    lua_State* GL = lua_mainthread(L);
    lua_State* ML = lua_newthread(GL);
    lua_xmove(GL, L, 1);

    // new thread needs to have the globals sandboxed
    luaL_sandboxthread(ML);

    // now we can compile & run module on the new thread
    std::string bytecode = Luau::compile(resolvedRequire.sourceCode, copts());
    if (luau_load(ML, resolvedRequire.chunkName.c_str(), bytecode.data(), bytecode.size(), 0) == 0)
    {
        if (codegen)
        {
            Luau::CodeGen::CompilationOptions nativeOptions;
            Luau::CodeGen::compile(ML, -1, nativeOptions);
        }
        int status = lua_resume(ML, L, 0);

        if (status == 0)
        {
            if (lua_gettop(ML) == 0)
                lua_pushstring(ML, "module must return a value");
            else if (!lua_istable(ML, -1) && !lua_isfunction(ML, -1))
                lua_pushstring(ML, "module must return a table or function");
        }
        else if (status == LUA_YIELD)
        {
            lua_pushstring(ML, "module can not yield");
        }
        else if (!lua_isstring(ML, -1))
        {
            lua_pushstring(ML, "unknown error while running module");
        }
    }

    // there's now a return value on top of ML; L stack: _MODULES ML
    lua_xmove(ML, L, 1);
    lua_pushvalue(L, -1);
    lua_setfield(L, -4, resolvedRequire.absolutePath.c_str());

    // L stack: _MODULES ML result
    return finishrequire(L);
}

static int lua_collectgarbage(lua_State* L) {
    const char* option = luaL_optstring(L, 1, "collect");
    if (strcmp(option, "collect") == 0) {
        lua_gc(L, LUA_GCCOLLECT, 0);
        return 0;
    }
    if (strcmp(option, "count") == 0) {
        int c = lua_gc(L, LUA_GCCOUNT, 0);
        lua_pushnumber(L, c);
        return 1;
    }
    luaL_error(L, "collectgarbage must be called with 'count' or 'collect'");
}

namespace engine {
//helpers
SDL_Window* core::window() {
    return window_ptr;
}
lua_State* core::lua_state() {
    return main_state;
}
//
int bootstrap(engine_start_options opts) {
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
void renderer::set_color(cm::coloru32 color) {
    SDL_SetRenderDrawColor(renderer_ptr, color.red(), color.green(), color.blue(), color.alpha());
}
void renderer::draw(const cm::recti64& rect) {
    sdl_rect_dummy.x = rect.x();
    sdl_rect_dummy.y = rect.y();
    sdl_rect_dummy.w = rect.width();
    sdl_rect_dummy.h = rect.height();
    SDL_RenderDrawRect(renderer_ptr, &sdl_rect_dummy);
}
void renderer::fill(const cm::recti64& rect) {
    sdl_rect_dummy.x = rect.x();
    sdl_rect_dummy.y = rect.y();
    sdl_rect_dummy.w = rect.width();
    sdl_rect_dummy.h = rect.height();
    SDL_RenderFillRect(renderer_ptr, &sdl_rect_dummy);
}
static void init_state(lua_State* L) {
    luaL_openlibs(main_state);
    lua_callbacks(L)->useratom = [](const char* raw_name, size_t s) {
        std::string_view name{raw_name, s};
        static constexpr auto count = comptime::count<bi::method_atom, bi::method_atom::bounds>();
        auto e = comptime::item<bi::method_atom, count>(name);
        return static_cast<int16_t>(e.index);
    };
    {using namespace builtin;
        init_global_types(L);
        fs_init_lib(L);
        vec2d_init_type(L);
        vec2i_init_type(L);
        sdl_init_lib(L);
    }
    static const luaL_Reg funcs[] = {
        {"loadstring", lua_loadstring},
        {"require", lua_require},
        {"collectgarbage", lua_collectgarbage},
        {"__builtin_import_filesystem", builtin::fs_import_lib},
        {NULL, NULL}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, NULL, funcs);
    lua_pop(L, 1);
    fs::path filepath = "main.luau";
    std::optional<std::string> source = common::read_file(filepath);
    if (not source) {
        using namespace std::string_literals;
        common::printerr("failed to read the file '"s + filepath.string() + "'");
    } else {
        std::string bytecode = Luau::compile(*source, copts());
        if (luau_load(L, "=main", bytecode.data(), bytecode.size(), 0)) {
            common::printerr(luaL_checkstring(L, -1));
        } else {
            if (lua_pcall(L, 0, 0, 0)) {
                common::printerr(luaL_checkstring(L, -1));
            }
        }
    }
}
void core::start(engine_start_options opts) {
    SDL_Init(SDL_INIT_VIDEO);// should do proper error handling here
    TTF_Init();
    IMG_Init(IMG_INIT_JPG);
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
    main_state = luaL_newstate();
    init_state(main_state);
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
                    case SDL_KEYDOWN:
                        lua_getglobal(main_state, bi::event_sockets::key_down);
                        if (not lua_isnil(main_state, -1) and lua_isfunction(main_state, -1)) {
                            lua_pushstring(main_state, bi::scancode_to_string(sdl_event_dummy.key.keysym.scancode));
                            lua_pcall(main_state, 1, 0, 0);
                        } else lua_pop(main_state, 1);
                    break;
                    case SDL_KEYUP:
                        lua_getglobal(main_state, bi::event_sockets::key_up);
                        if (not lua_isnil(main_state, -1) and lua_isfunction(main_state, -1)) {
                            lua_pushstring(main_state, bi::scancode_to_string(sdl_event_dummy.key.keysym.scancode));
                            lua_pcall(main_state, 1, 0, 0);
                        } else lua_pop(main_state, 1);
                    break;
                    case SDL_MOUSEBUTTONUP:
                        lua_getglobal(main_state, bi::event_sockets::mouse_up);
                        if (not lua_isnil(main_state, -1) and lua_isfunction(main_state, -1)) {
                            switch (sdl_event_dummy.button.button) {
                                case SDL_BUTTON_LEFT:
                                    lua_pushstring(main_state, "left");
                                break;
                                case SDL_BUTTON_RIGHT:
                                    lua_pushstring(main_state, "right");
                                break;
                                case SDL_BUTTON_MIDDLE:
                                    lua_pushstring(main_state, "middle");
                                break;

                            }
                            bi::create<cm::vec2d>(main_state) = {
                                double(sdl_event_dummy.button.x),
                                double(sdl_event_dummy.button.y)
                            };
                            lua_pcall(main_state, 2, 0, 0);
                        } else lua_pop(main_state, 1);
                    break;
                    case SDL_MOUSEBUTTONDOWN:
                        lua_getglobal(main_state, bi::event_sockets::mouse_down);
                        if (not lua_isnil(main_state, -1) and lua_isfunction(main_state, -1)) {
                            switch (sdl_event_dummy.button.button) {
                                case SDL_BUTTON_LEFT:
                                    lua_pushstring(main_state, "left");
                                break;
                                case SDL_BUTTON_RIGHT:
                                    lua_pushstring(main_state, "right");
                                break;
                                case SDL_BUTTON_MIDDLE:
                                    lua_pushstring(main_state, "middle");
                                break;
                            }
                            bi::create<cm::vec2d>(main_state) = {
                                double(sdl_event_dummy.button.x),
                                double(sdl_event_dummy.button.y)
                            };
                            lua_pcall(main_state, 2, 0, 0);
                        } else lua_pop(main_state, 1);
                    break;
                }
            }
            event::process_event_stack_entries(5);
        } {//updating
            const auto curr_tp = ch::steady_clock::now();
            const double delta_s = ch::duration<double>(curr_tp - cached_last_tp).count();
            cached_last_tp = curr_tp;
            systems::physics(comp::view<ty::physical_component>(), delta_s);
            lua_getglobal(main_state, bi::event_sockets::update);
            if (not lua_isnil(main_state, -1) and lua_isfunction(main_state, -1)) {
                lua_pushnumber(main_state, delta_s);
                lua_pcall(main_state, 1, 0, 0);
            } else lua_pop(main_state, 1);
        } {//rendering
            SDL_SetRenderDrawColor(renderer_ptr, 0x00, 0x00, 0x00, 0xff);
            SDL_RenderClear(renderer_ptr);
            lua_getglobal(main_state, bi::event_sockets::render);
            if (not lua_isnil(main_state, -1) and lua_isfunction(main_state, -1)) {
                lua_pcall(main_state, 0, 0, 0);
            } else lua_pop(main_state, 1);
            SDL_RenderPresent(renderer_ptr);
        }
        event::process_event_stack_entries(5);
    }
}
void core::shutdown() {
    lua_getglobal(main_state, bi::event_sockets::shutdown);
    if (not lua_isnil(main_state, -1) and lua_isfunction(main_state, -1)) {
        lua_pcall(main_state, 0, 0, 0);
    } else lua_pop(main_state, 1);
    //lua_close(main_state); //this slows down shutdown time significantly
    SDL_DestroyRenderer(renderer_ptr);
    SDL_DestroyWindow(window_ptr);
    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}
}
