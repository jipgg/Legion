#include "types.h"
#include "engine.h"
#include <SDL.h>
#include <SDL_image.h>
namespace fs = std::filesystem;
namespace cm = common;
namespace types {
texture::texture(const fs::path& img_path): path(img_path), src_size(0), data(nullptr, SDL_DestroyTexture) {
    common::print(path, path.string().c_str());
    SDL_Surface* surface = IMG_Load(path.string().c_str());
    if (surface == nullptr) {
        common::printerr("surface was nullptr");
    } else {
        cm::deferred _{ [&surface] {SDL_FreeSurface(surface);} };
        src_size = cm::vec2i{surface->w, surface->h};
        data.reset(SDL_CreateTextureFromSurface(SDL_GetRenderer(engine::core::window()), surface));
    }
}
}
