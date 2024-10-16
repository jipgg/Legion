#include "legion/components.h"
#include <SDL_ttf.h>
namespace fs = std::filesystem;
Font::Font(const fs::path& ttfpath, int ptsize) noexcept:
    font_(TTF_OpenFont(ttfpath.string().c_str(), ptsize)) {
}
Font::~Font() noexcept {
    if (font_) TTF_CloseFont(font_);
}
Font::Font(Font&& a) noexcept: font_(a.font_) {
    a.font_ = nullptr;
}
Font& Font::operator=(Font&& a) noexcept {
    font_ = a.font_;
    a.font_ = nullptr;
    return *this;
}
