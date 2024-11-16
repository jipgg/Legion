#pragma once
#include <blaze/Blaze.h>
#include <SDL.h>
#include "common.h"
#include "builtin.h"
#include <filesystem>

namespace util {
constexpr mat3f default_transform = {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}};
std::array<float, 8> get_quad_transform_raw(const vec2f& dim, const mat3f& transform = default_transform);
std::array<float, 8> get_quad_transform_raw(const SDL_FRect& quad, const mat3f& transform = default_transform);
bool cache(const builtin::font& font, std::string_view to_cache);
void cache(const builtin::font& font, char to_cache);
void clear_font_cache_registry();
void clear_font_cache(const builtin::font& font);
struct font_traits {
    int newline_margin{0};
    int tab_width{4};
};
constexpr SDL_Color plain_white{0xff, 0xff, 0xff, 0xff};
SDL_Color current_draw_color();
bool render_quad(
    const SDL_FRect& quad_dim,
    SDL_Texture* texture = nullptr,
    const mat3f& transform = default_transform,
    SDL_Color color = plain_white
);
void draw_string(const builtin::font& font, std::string_view string, const mat3f& transform = default_transform, const font_traits& traits = {});
void fill_circle();
void fill_ellipse();
constexpr mat3f scale_matrix(const vec2f& s) {
    return mat3f{
        {s[0], 0, 0},
        {0, s[1], 0},
        {0, 0, 1},
    };
}
constexpr mat3f translation_matrix(const vec2f& t) {
    return mat3f{
        {1, 0, t[0]},
        {0, 1, t[1]},
        {0, 0, 1},
    };
}
constexpr mat3f rotation_matrix(float r) {
    return mat3f{
        {cosf(r), -sinf(r), 0},
        {sinf(r), cosf(r), 0},
        {0, 0, 1},
    };
}

std::filesystem::path get_executable_path();
SDL_Renderer* renderer();
constexpr std::array<float, 8> quad_uv{
    0, 0,
    1, 0,
    1, 1,
    0, 1
};
constexpr std::array<int, 6> quad_indices{
    0, 1, 2,
    2, 3, 0
};
constexpr int indices_width = sizeof(int); 
constexpr int vertex_stride = sizeof(float) * 2;
inline void clear_all_cache_registries() {
    clear_font_cache_registry();
}
}
