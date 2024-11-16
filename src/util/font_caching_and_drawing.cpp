#include "util.h"
#include <unordered_map>
#include "engine.h"
namespace bi = builtin;
//using font_id = std::pair<bi::path, uint16_t>;
struct font_id {
    bi::path file_path;
    int pt_size;
    friend bool operator==(const font_id& lhs, const font_id& rhs) {
        return lhs.file_path == rhs.file_path and lhs.pt_size == rhs.pt_size;
    }
    font_id(const bi::font& font): file_path(font.file_path), pt_size(font.pt_size) {}
};
namespace std {
    template<>
    struct hash<font_id> {
        size_t operator()(const font_id& fid) const noexcept {
            size_t hash1 = std::hash<bi::path>{}(fid.file_path);
            size_t hash2 = std::hash<int>{}(fid.pt_size);
            return hash1 ^ (hash2 << 1);//relatively balanced
        }
    };
}
using font_cache = std::unordered_map<char, bi::texture>;
static std::unordered_map<font_id, font_cache> font_registry;
static std::string dummy{};
static constexpr SDL_Color plain_white{0xff, 0xff, 0xff, 0xff};

static font_cache& get_cache(const bi::font& font) {
    const font_id id{font};
    auto font_it = font_registry.find(id);
    if (font_it == font_registry.end()) {
        font_registry.insert(std::make_pair(id, font_cache{}));
    }
    return font_registry.at(id);
}

namespace util {
void clear_font_cache(const bi::font& font) {
    font_registry.erase(font_id{font});
}
void clear_font_cache_registry() {
    font_registry.clear();
}
bool cache(const bi::font& font, std::string_view to_cache) {
    bool has_new_entry;
    font_cache& fc = get_cache(font);
    for (char c : to_cache) {
        auto found_it = fc.find(c);
        if (found_it == fc.end()) {
            has_new_entry = true;
            util::cache(font, c);
        }
    }
    return has_new_entry;
}

void cache(const bi::font& font, char to_cache) {
    dummy.resize(1);
    dummy[0] = to_cache;
    SDL_Surface* surface = TTF_RenderText_Blended(font.ptr.get(),
    dummy.c_str(), plain_white);
    scope_guard d([&surface]{ SDL_FreeSurface(surface); });
    bi::texture_ptr txt{SDL_CreateTextureFromSurface(util::renderer(), surface), SDL_DestroyTexture};
    bi::texture loaded{.ptr = std::move(txt), .w = surface->w, .h = surface->h};
    get_cache(font).insert({to_cache, std::move(loaded)});
}

void draw_string(const bi::font& font, std::string_view string, const mat3f& transform, const font_traits& traits) {
    float x_off{};
    float y_off{};
    SDL_Color curr_draw_color{};
    SDL_GetRenderDrawColor(renderer(),
        &curr_draw_color.r, &curr_draw_color.g,
        &curr_draw_color.b, &curr_draw_color.a
    );
    const font_id id{font};
    font_cache& fc = get_cache(font);
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
