#include <Luau/Common.h>
#include <Luau/Compiler.h>
#include <lua.h>
#include <lualib.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "core.h"
#include "util.h"
#include <string_view>
#include <iostream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
void attach_parent_process() {
    if (AttachConsole(ATTACH_PARENT_PROCESS)) {
        FILE* fp;
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
        freopen_s(&fp, "CONIN$", "r", stdin);
    }
}
#endif//_WIN32

void run_game_loop(luau::State& L) {
    sdl::Window window = SDL_CreateWindow("Legion",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    sdl::Renderer renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    bool quit_game{};
    SDL_Event e{};
    while (not quit_game) {
        {// event handling
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit_game = true;
                }
            }
        } {//update

        } {//rendering
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
        }
    }
}
inline std::string_view  get_luau_error(lua_State* L) {
    return luaL_checkstring(L, -1);
}
luau::State init_luau() {
    luau::State L{};
    luaL_openlibs(L);
    luaL_sandbox(L);
    const std::string source = util::read_text_file("scripts/main.luau");
    const std::string bytecode = Luau::compile(source);
    if (luau_load(L, "main", bytecode.data(), bytecode.size(), 0) != LUA_OK) {
        std::cerr << get_luau_error(L);
    }
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
        std::cerr << get_luau_error(L);
    }
    return L;
}
int main(int, char**) {
    #ifdef _WIN32
    attach_parent_process();
    #endif
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    luau::State L = init_luau();
    run_game_loop(L);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
