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
void clear_cache(const builtin::font& font);
void draw(const builtin::font& font, std::string_view text, const mat3f& transform = default_transform);
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
constexpr int vertex_stride = 8;
inline void clear_all_caches() {
    clear_font_cache_registry();
}
}
