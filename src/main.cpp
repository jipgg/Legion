#include <SDL.h>
#include <SDL_ttf.h>
#include <cassert>
#include <iostream>
#include "Legion.h"
#include "common.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
void attach_console();
#endif

SDL_Window* window{nullptr};
SDL_Renderer* renderer{nullptr};
bool quit{false};

int main(int, char**) {
    #ifdef _WIN32
    attach_console();
    #endif
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    constexpr uint32_t wnd_flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    window = SDL_CreateWindow("Legion", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1280, 720, wnd_flags);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    PhysicsSystem process_physics;
    SDL_Event e;
    while (not quit) {
        {// event handling
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT) {
                    quit = true;
                }
            }
        } {//update
            process_physics();
        } {//rendering
            SDL_RenderClear(renderer);
            SDL_RenderPresent(renderer);
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
