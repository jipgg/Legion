#include "types.h"
#include <SDL_ttf.h>
namespace fs = std::filesystem;
namespace types {
font::font(const fs::path& ttfpath, int ptsize) noexcept:
    font_(TTF_OpenFont(ttfpath.string().c_str(), ptsize)) {
}
font::~font() noexcept {
    if (font_) TTF_CloseFont(font_);
}
font::font(font&& a) noexcept: font_(a.font_) {
    a.font_ = nullptr;
}
font& font::operator=(font&& a) noexcept {
    font_ = a.font_;
    a.font_ = nullptr;
    return *this;
}
}
