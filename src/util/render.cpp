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
}
