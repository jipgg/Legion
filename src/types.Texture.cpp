#include "types.h"
#include "engine.h"
#include <SDL.h>
#include <SDL_image.h>
namespace fs = std::filesystem;
namespace types {
Texture::Texture(const fs::path& img_path) noexcept: src_size_(0) {
    SDL_Surface* surface = IMG_Load(img_path.string().c_str());
    common::Deferred _{ [&surface] {SDL_FreeSurface(surface);} };
    src_size_ = common::Sizei32{static_cast<int16_t>(surface->w), static_cast<int16_t>(surface->h)};
    texture_ = SDL_CreateTextureFromSurface(SDL_GetRenderer(engine::core::get_window()), surface);
}
Texture::Texture(Texture&& a) noexcept: src_size_(a.src_size_), texture_(a.texture_) {
    a.texture_ = nullptr;
}
Texture& Texture::operator=(Texture&& a) noexcept {
    texture_ = a.texture_;
    src_size_ = a.src_size_;
    a.texture_ = nullptr;
    return *this;
}
Texture::~Texture() noexcept {
    SDL_DestroyTexture(texture_);
}
common::Sizei32 Texture::src_size() const {
    return src_size_;
}
}
