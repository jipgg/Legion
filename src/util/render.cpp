#include "util.h"
#include "engine.h"
namespace util {
std::array<float, 8> get_quad_transform_raw(const Vec2f& dim, const Mat3f& transform) {
    Vec3f top_left = transform * Vec3f{0, 0, 1};
    Vec3f top_right = transform * Vec3f{dim[0], 0, 1};
    Vec3f bottom_right = transform * Vec3f{dim[0], dim[1], 1};
    Vec3f bottom_left = transform * Vec3f{0, dim[1], 1};
    return {
        top_left[0], top_left[1],
        top_right[0], top_right[1],
        bottom_right[0], bottom_right[1],
        bottom_left[0], bottom_left[1],
    };
}
std::array<float, 8> get_quad_transform_raw(const SDL_FRect& quad, const Mat3f& transform) {
    const Vec3f offset{quad.x, quad.y, 0};
    Vec3f top_left = transform * (offset + Vec3f{0, 0, 1});
    Vec3f top_right = transform * (offset + Vec3f{quad.w, 0, 1});
    Vec3f bottom_right = transform * (offset + Vec3f{quad.w, quad.h, 1});
    Vec3f bottom_left = transform * (offset + Vec3f{0, quad.h, 1});
    return {
        top_left[0], top_left[1],
        top_right[0], top_right[1],
        bottom_right[0], bottom_right[1],
        bottom_left[0], bottom_left[1],
    };
}
SDL_Renderer* renderer() {
    return SDL_GetRenderer(engine::window());
}
SDL_Color current_draw_color() {
    SDL_Color color;
    SDL_GetRenderDrawColor(renderer(), &color.r, &color.g, &color.b, &color.a);
    return color;
}
bool render_quad(const SDL_FRect& quad_dim, SDL_Texture* texture,
    const Mat3f& transform, SDL_Color color) {
    const auto vertices = get_quad_transform_raw(quad_dim, transform);
    if (SDL_RenderGeometryRaw(renderer(), texture,
        vertices.data(), vertex_stride, &color, 0,
        quad_uv.data(), vertex_stride, 4,
        quad_indices.data(), quad_indices.size(), indices_width)) {
        return false;
    }
    return true;
}
bool draw_rect_with_line_width(const SDL_Rect &rect, int line_width, ExpansionType expansion_type) {
    std::lock_guard<std::mutex> lock(global_rect_buffer.mtx);
    global_rect_buffer.lazy_clear();
    auto& vec = global_rect_buffer.vec;
    const int half_width{line_width / 2};
    for (int i{0}; i < line_width; ++i) {
        const int itwice{i * 2};
        switch (expansion_type) {
            case ExpansionType::outer:
                vec.emplace_back(SDL_Rect{
                    .x = rect.x - i,
                    .y = rect.y - i,
                    .w = rect.w + itwice,
                    .h = rect.h + itwice,
                });
                break;
            case ExpansionType::inner:
                vec.emplace_back(SDL_Rect{
                    .x = rect.x + i,
                    .y = rect.y + i,
                    .w = rect.w - itwice,
                    .h = rect.h - itwice,
                });
                break;
            case ExpansionType::centered:
                vec.emplace_back(SDL_Rect{
                    .x = rect.x - half_width + i,
                    .y = rect.y - half_width + i,
                    .w = rect.w + line_width - itwice,
                    .h = rect.h + line_width - itwice,
                });
                break;
        }
    }
    SDL_RenderDrawRects(renderer(), vec.data(), vec.size());
    return true;
}
}
