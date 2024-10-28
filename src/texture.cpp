#include "types.h"
#include "engine.h"
#include <SDL.h>
#include <SDL_image.h>
namespace fs = std::filesystem;
namespace cm = common;
namespace types {
texture::texture(const fs::path& img_path) noexcept: src_size_(0) {
    SDL_Surface* surface = IMG_Load(img_path.string().c_str());
    cm::deferred _{ [&surface] {SDL_FreeSurface(surface);} };
    src_size_ = cm::vec2i{surface->w, surface->h};
    texture_ = SDL_CreateTextureFromSurface(SDL_GetRenderer(engine::core::window()), surface);
}
texture::texture(texture&& a) noexcept: src_size_(a.src_size_), texture_(a.texture_) {
    a.texture_ = nullptr;
}
texture& texture::operator=(texture&& a) noexcept {
    texture_ = a.texture_;
    src_size_ = a.src_size_;
    a.texture_ = nullptr;
    return *this;
}
texture::~texture() noexcept {
    SDL_DestroyTexture(texture_);
}
cm::vec2i texture::src_size() const {
    return src_size_;
}
}
