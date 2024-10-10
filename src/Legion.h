#include <SDL.h>
#include <SDL_ttf.h>
#include "common.h"
#include <iostream>
template <class ...Ts>
void print(Ts&&...args) {
    ((std::cout << args << ' '), ...) << '\n';
}
namespace engine {
struct Start_options {
    std::string window_name{"Legion"};
    V2i window_size{1240, 720};
    bool window_resizable{false};
    bool hardware_accelerated{true};
    bool vsync_enabled{true};
};
int bootstrap(Start_options opts = {});
namespace core {
void start(Start_options opts = {});
void run();
void shutdown();}
}
