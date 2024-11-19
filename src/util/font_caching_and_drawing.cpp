#include "util.h"
#include <unordered_map>
#include "engine.h"
using builtin::Font;
using builtin::Texture;
using builtin::files::Path;
struct FontId {
    Path path;
    int pt_size;
    friend bool operator==(const FontId& lhs, const FontId& rhs) {
        return lhs.path == rhs.path and lhs.pt_size == rhs.pt_size;
    }
    FontId(const Font& font): path(font.path), pt_size(font.pt_size) {}
};
namespace std {
    template<>
    struct hash<FontId> {
        size_t operator()(const FontId& fid) const noexcept {
            size_t hash1 = std::hash<Path>{}(fid.path);
            size_t hash2 = std::hash<int>{}(fid.pt_size);
            return hash1 ^ (hash2 << 1);//relatively balanced
        }
    };
}
using FontCache = std::unordered_map<char, builtin::Texture>;
static std::unordered_map<FontId, FontCache> font_registry;
static std::string dummy{};
static constexpr SDL_Color plain_white{0xff, 0xff, 0xff, 0xff};

static FontCache& get_cache(const Font& font) {
    const FontId id{font};
    auto font_it = font_registry.find(id);
    if (font_it == font_registry.end()) {
        font_registry.insert(std::make_pair(id, FontCache{}));
    }
    return font_registry.at(id);
}

namespace util {
void clear_font_cache(const builtin::Font& font) {
    font_registry.erase(FontId{font});
}
void clear_font_cache_registry() {
    font_registry.clear();
}
bool cache(const Font& font, std::string_view to_cache) {
    bool has_new_entry;
    FontCache& fc = get_cache(font);
    for (char c : to_cache) {
        auto found_it = fc.find(c);
        if (found_it == fc.end()) {
            has_new_entry = true;
            util::cache(font, c);
        }
    }
    return has_new_entry;
}

void cache(const Font& font, char to_cache) {
    dummy.resize(1);
    dummy[0] = to_cache;
    SDL_Surface* surface = TTF_RenderText_Blended(font.ptr.get(),
    dummy.c_str(), plain_white);
    ScopeGuard d([&surface]{ SDL_FreeSurface(surface); });
    std::shared_ptr<SDL_Texture> txt{SDL_CreateTextureFromSurface(util::renderer(), surface), SDL_DestroyTexture};
    Texture loaded{.ptr = std::move(txt), .w = surface->w, .h = surface->h};
    get_cache(font).insert({to_cache, std::move(loaded)});
}

void draw_string(const Font& font, std::string_view string, const Mat3f& transform, const FontTraits& traits) {
    float x_off{};
    float y_off{};
    SDL_Color curr_draw_color{};
    SDL_GetRenderDrawColor(renderer(),
        &curr_draw_color.r, &curr_draw_color.g,
        &curr_draw_color.b, &curr_draw_color.a
    );
    const FontId id{font};
    FontCache& fc = get_cache(font);
    for (char c : string) {
        if (auto it = fc.find(c); it == fc.end()) {
            util::cache(font, c);
        }
        const auto& txt = fc.at(c);
        switch (c) {
            case '\n':
                x_off = 0;
                y_off += txt.h + traits.newline_margin;
                continue;
            case '\t':
                if (auto space = fc.find(' '); space == fc.end()) {
                    util::cache(font, ' ');
                }
                x_off += fc.at(' ').w * traits.tab_width;
                continue;
        }
        const SDL_FRect rect{x_off, y_off, static_cast<float>(txt.w), static_cast<float>(txt.h)};
        x_off += txt.w;
        engine::expect(util::render_quad(
            rect, 
            txt.ptr.get(), 
            transform, 
            curr_draw_color), SDL_GetError());
    }
}
}
