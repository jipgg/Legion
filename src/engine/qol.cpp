#include "engine.h"
#include <SDL.h>

namespace engine {
Vec2i window_size() {
    Vec2i size{};
    SDL_GetWindowSize(window(), &size[0], &size[1]);
    return size;
}
}
