#include "lua_util.h"
#include "builtin.h"
#include <lualib.h>
#include <lua.h>
#include <luaconf.h>
#include <SDL.h>
#include <SDL_image.h>
#include "engine.h"
#include "util.h"
#include "common.h"
#include "builtin_types.h"
using builtin::Color;
using builtin::Rect;
using builtin::Vec2;
using builtin::Mat3;
using builtin::Texture;
namespace std {//vec2i hash
template<>
struct hash<Vec2i> {
    size_t operator()(const Vec2i& id) const noexcept {
        size_t x = std::hash<int>{}(id[0]);
        size_t y = std::hash<int>{}(id[1]);
        return x ^ (y << 1);
    }
};
}
static std::vector<float> float_buffer;
static std::vector<SDL_Point> point_buffer;
static std::vector<SDL_Rect> rect_buffer;
using CircleId = int;
using RadiusId = Vec2i;

static std::unordered_map<CircleId, std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>> circle_cache;
static std::unordered_map<RadiusId, std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>> ellipse_cache;
static SDL_RendererInfo info;
static int err_sdl(lua_State* L) {
    luaL_error(L, "SDL Error: %s", SDL_GetError());
}
__forceinline static void fill_circle_impl(SDL_Renderer* renderer, int x, int y, int radius) {
    int dx = 0;
    int dy = radius;
    int d = 1 - radius;
    point_buffer.resize(0);
    const int area = static_cast<int>(std::ceil(M_PI * static_cast<double>(radius * radius)));
    point_buffer.reserve(area);
    while (dy >= dx) {
        point_buffer.emplace_back(SDL_Point{x-dx, y + dy});
        point_buffer.emplace_back(SDL_Point{x + dx, y + dy});

        point_buffer.emplace_back(SDL_Point{x - dx, y - dy});
        point_buffer.emplace_back(SDL_Point{x + dx, y - dy});

        point_buffer.emplace_back(SDL_Point{x - dy, y + dx});
        point_buffer.emplace_back(SDL_Point{x + dy, y + dx});

        point_buffer.emplace_back(SDL_Point{x - dy, y - dx});
        point_buffer.emplace_back(SDL_Point{x + dy, y - dx});
        if (d < 0) d += 2 * dx + 3;
        else {
            d += 2 * (dx - dy) + 5;
            dy--;
        }
        dx++;
    }
    SDL_RenderDrawLines(renderer, point_buffer.data(), point_buffer.size());
}
__forceinline static void fill_ellipse_impl(SDL_Renderer* renderer, int center_x, int center_y, int radius_x, int radius_y) {
    int x = 0;
    int y = radius_y;
    int rx_squared = radius_x * radius_x;
    int ry_squared = radius_y * radius_y;
    int rx_squared_x2 = 2 * rx_squared;
    int ry_squared_x2 = 2 * ry_squared;
    int px = 0;
    int py = rx_squared_x2 * y;
    int p1 = ry_squared - (rx_squared * radius_y) + (0.25 * rx_squared);
    point_buffer.resize(0);
    const int area = static_cast<int>(std::ceil(M_PI * static_cast<double>(radius_x * radius_y)));
    point_buffer.reserve(area);
    while (px < py) {
        SDL_RenderDrawLine(renderer, center_x - x, center_y + y, center_x + x, center_y + y);
        SDL_RenderDrawLine(renderer, center_x - x, center_y - y, center_x + x, center_y - y);
        x++;
        px += ry_squared_x2;
        if (p1 < 0) p1 += ry_squared + px;
        else {
            y--;
            py -= rx_squared_x2;
            p1 += ry_squared + px - py;
        }
    }
    int p2 = (ry_squared * (x + 0.5) * (x + 0.5)) + (rx_squared * (y - 1) * (y - 1)) - (rx_squared * ry_squared);
    while (y >= 0) {
        SDL_RenderDrawLine(renderer, center_x - x, center_y + y, center_x + x, center_y + y);
        SDL_RenderDrawLine(renderer, center_x - x, center_y - y, center_x + x, center_y - y);
        y--;
        py -= rx_squared_x2;
        if (p2 > 0) {
            p2 += rx_squared - py;
        } else {
            x++;
            px += ry_squared_x2;
            p2 += rx_squared - py + px;
        }
    }
    //SDL_RenderDrawLines(renderer, point_buffer.data(), point_buffer.size());
}
__forceinline static bool is_cached(int radius) {
    return circle_cache.find(radius) != circle_cache.end();
}
enum class cache_status {
    not_cached,
    cached,
    cached_flipped_r,
};
__forceinline static cache_status is_cached(const Vec2i& radii) {
    if (ellipse_cache.find(radii) != ellipse_cache.end()) return cache_status::cached;
    if (ellipse_cache.find(Vec2i{radii[1], radii[0]}) != ellipse_cache.end()) return cache_status::cached_flipped_r;
    return cache_status::not_cached;
}
__forceinline static void cache_circle(SDL_Renderer* renderer, int radius, SDL_Color curr_draw_color = util::current_draw_color()) {
    const int diameter = radius * 2;
    SDL_Texture* cache = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, diameter, diameter);
    SDL_SetTextureBlendMode(cache, SDL_BLENDMODE_BLEND);
        SDL_SetRenderTarget(renderer, cache);
        ScopeGuard a([&renderer] {
            SDL_SetRenderTarget(renderer, nullptr);
        });
        SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
        ScopeGuard b([&renderer, &curr_draw_color]{
            SDL_SetRenderDrawColor(renderer, curr_draw_color.r, curr_draw_color.g, curr_draw_color.b, curr_draw_color.a);
        });
        fill_circle_impl(renderer, radius, radius, radius);
        circle_cache.insert({radius, std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>{cache, SDL_DestroyTexture}});
}
__forceinline static void cache_ellipse(SDL_Renderer* renderer, const Vec2i& radii, SDL_Color cdc = util::current_draw_color()) {
    const int diameter_x = radii[0] * 2;
    const int diameter_y = radii[1] * 2;
    SDL_Texture* to_cache = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, diameter_x, diameter_y);
    SDL_SetTextureBlendMode(to_cache, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, to_cache);
    ScopeGuard a([&renderer] {SDL_SetRenderTarget(renderer, nullptr);});
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    ScopeGuard b([&renderer, &cdc] {SDL_SetRenderDrawColor(renderer, cdc.r, cdc.g, cdc.b, cdc.a);});
    fill_ellipse_impl(renderer, radii[0], radii[1], radii[0], radii[1]);
    ellipse_cache.insert({radii, std::unique_ptr<SDL_Texture, decltype(&SDL_DestroyTexture)>(to_cache, SDL_DestroyTexture)});
    print("cached ellipse", radii[0], radii[1]);
}
__forceinline static void fill_cached_ellipse(
    SDL_Renderer* renderer, const Vec2i& radii, Mat3f transform = util::default_transform) {
    SDL_Color color = util::current_draw_color();
    SDL_Texture* ellipse = nullptr;
    SDL_FRect dst{
        .x = static_cast<float>(-radii[0]),
        .y = static_cast<float>(-radii[1]),
        .w = static_cast<float>(radii[0] * 2),
        .h = static_cast<float>(radii[1] * 2),
    };
    switch (is_cached(radii)) {
        case cache_status::not_cached:
            cache_ellipse(renderer, radii, color);
            ellipse = ellipse_cache.at(radii).get();
            break;
        case cache_status::cached:
            ellipse = ellipse_cache.at(radii).get();
            break;
        case cache_status::cached_flipped_r:
            transform *= util::rotation_matrix(M_PI / 2.f);
            dst = {.x = float(-radii[1]), .y = float(-radii[0]), .w = float(radii[1] * 2), .h = float(radii[0] * 2)};
            ellipse = ellipse_cache.at(Vec2i{radii[1], radii[0]}).get();
            break;
    }
    engine::expect(
        util::render_quad(dst, ellipse, transform, color),
        SDL_GetError()
    );
}
__forceinline static void fill_cached_circle(SDL_Renderer* renderer, int radius, const Mat3f& transform = util::default_transform) {
    SDL_Color curr_draw_color = util::current_draw_color();
    const int diameter = radius * 2;
    if (not is_cached(radius)) {
        cache_circle(renderer, radius, curr_draw_color);
    }
    SDL_Texture* circle = circle_cache.at(radius).get();
    const SDL_FRect dst{
        .x = static_cast<float>(-radius),
        .y = static_cast<float>(-radius),
        .w = static_cast<float>(diameter),
        .h = static_cast<float>(diameter),
    };
    engine::expect(
        util::render_quad(dst, circle, transform, curr_draw_color),
        SDL_GetError()
    );
}
static int set_color(lua_State* L) {
    auto& c = check<Color>(L, 1);
    SDL_SetRenderDrawColor(util::renderer(), c.r, c.g, c.b, c.a);
    return 0;
}
static int set_blend_mode(lua_State* L) {
    std::string_view mode = luaL_checkstring(L, 1);
    SDL_BlendMode blend = string_to_blendmode(mode);
    SDL_SetRenderDrawBlendMode(util::renderer(), blend);
    return 0;
}
static int draw_rectangle(lua_State* L) {
    SDL_Rect dummy{};
    if (is_type<Rect>(L, 1)) {
        auto& rect = check<Rect>(L, 1);
        dummy = {
            static_cast<int>(rect.x),
            static_cast<int>(rect.y),
            static_cast<int>(rect.w),
            static_cast<int>(rect.h)
        };
    } else if (is_type<Vec2>(L, 1)) {
        auto& origin = check<Vec2>(L, 1);
        auto& size = check<Vec2>(L, 2);
        dummy = {
            static_cast<int>(origin[0]),
            static_cast<int>(origin[1]),
            static_cast<int>(size[0]),
            static_cast<int>(size[1])
        };
    
    } else return lua_err::invalid_type(L);
    SDL_RenderDrawRect(util::renderer(), &dummy);
    return 0;
}
static int draw_rectangles(lua_State* L) {
    const int top = lua_gettop(L);
    rect_buffer.resize(0);
    rect_buffer.reserve(top);
    SDL_Rect dummy{};
    for (int i{1}; i <= top; ++i) {
        auto& r = check<Rect>(L, i);
        dummy = {
            static_cast<int>(r.x),
            static_cast<int>(r.y),
            static_cast<int>(r.w),
            static_cast<int>(r.h)
        };
        rect_buffer.emplace_back(dummy);
    }
    SDL_RenderDrawRects(util::renderer(), rect_buffer.data(), rect_buffer.size());
    return 0;
}
static int draw_points(lua_State* L) {
    const int top = lua_gettop(L);
    point_buffer.resize(0);
    point_buffer.reserve(top);
    for (int i{1}; i <= top; ++i) {
        auto& p = check<Vec2>(L, i);
        point_buffer.emplace_back(SDL_Point{static_cast<int>(p.at(0)), static_cast<int>(p.at(1))});
    }
    SDL_RenderDrawPoints(util::renderer(), point_buffer.data(), point_buffer.size());
    return 0;
}
static int fill_rectangle(lua_State* L) {
    SDL_Rect dummy{};
    if (is_type<Rect>(L, 1)) {
        auto& rect = check<Rect>(L, 1);
        dummy = {
            static_cast<int>(rect.x),
            static_cast<int>(rect.y),
            static_cast<int>(rect.w),
            static_cast<int>(rect.h)
        };
    } else if (is_type<Vec2>(L, 1)) {
        auto& origin = check<Vec2>(L, 1);
        auto& size = check<Vec2>(L, 2);
        dummy = {
            static_cast<int>(origin[0]),
            static_cast<int>(origin[1]),
            static_cast<int>(size[0]),
            static_cast<int>(size[1])
        };
    
    } else return lua_err::invalid_argument(L, 1);
    SDL_RenderFillRect(util::renderer(), &dummy);
    return 0;
}
static int fill_rectangles(lua_State* L) {
    const int top = lua_gettop(L);
    rect_buffer.resize(top);
    for (int i{1}; i <= top; ++i) {
        auto& r = check<Rect>(L, i);
        rect_buffer[i - 1] = SDL_Rect{
            static_cast<int>(r.x),
            static_cast<int>(r.y),
            static_cast<int>(r.w),
            static_cast<int>(r.h),
        };
    }
    SDL_RenderFillRects(util::renderer(), rect_buffer.data(), rect_buffer.size());
    return 0;
}
static int draw_line(lua_State* L) {
    auto& t0 = check<Vec2>(L, 1);
    auto& t1 = check<Vec2>(L, 2);
    SDL_RenderDrawLine(util::renderer(), t0.at(0), t0.at(1), t1.at(0), t1.at(1));
    return 0;
}
static int draw_point(lua_State* L) {
    auto& p = check<Vec2>(L, 1);
    SDL_RenderDrawPoint(util::renderer(), p.at(0), p.at(1));
    return 0;
}
static int draw_lines(lua_State* L) {
    const int top = lua_gettop(L);
    point_buffer.resize(0);
    point_buffer.reserve(top);
    for (int i{1}; i <= top; ++i) {
        auto& p = check<Vec2>(L, i);
        point_buffer.emplace_back(SDL_Point{int(p.at(0)), int(p.at(1))});
    }
    SDL_RenderDrawLines(util::renderer(), point_buffer.data(), point_buffer.size());
    return 0;
}

static int draw_polygon(lua_State* L) {
    point_buffer.resize(0);
    for (int i{1}; i <= lua_objlen(L, 1); ++i) {
        lua_rawgeti(L, 1, i);
        auto& point = check<Vec2>(L, -1);
        point_buffer.emplace_back(SDL_Point{
            static_cast<int>(point[0]),
            static_cast<int>(point[1])
        });
        lua_pop(L, 1);
    }
    point_buffer.emplace_back(point_buffer.front());
    SDL_RenderDrawLines(util::renderer(), point_buffer.data(), point_buffer.size());
    return 0;
}

static int fill_circle(lua_State* L) {
    Mat3f transform{};
    if (is_type<Vec2>(L, 1)) {
        auto& center = check<Vec2>(L, 1);
        transform = util::translation_matrix(center);
    } else if (is_type<Mat3>(L, 1)) {
        transform = check<Mat3>(L, 1);
    } else return lua_err::invalid_argument(L, 1);
    double radius = luaL_checknumber(L, 2);
    fill_cached_circle(util::renderer(),int(radius), transform);
    return 0;
}
static int fill_ellipse(lua_State* L) {
    //auto& center = check<vector2>(L, 1);
    Mat3f transform{};
    if (is_type<Vec2>(L, 1)) {
        auto& center = check<Vec2>(L, 1);
        transform = util::translation_matrix(center);
    } else if (is_type<Mat3>(L, 1)) {
        transform = check<Mat3>(L, 1);
    } else return lua_err::invalid_argument(L, 1);
    auto& radius = check<Vec2>(L, 2);
    fill_cached_ellipse(util::renderer(), radius, transform);
    return 0;
}
static int fill_polygon(lua_State* L) {
    float_buffer.resize(0);
    const int len = lua_objlen(L, 1);
    for (int i{1}; i <= len; ++i) {
        lua_rawgeti(L, 1, i);
        auto& point = check<Vec2>(L, -1);
        float_buffer.emplace_back(static_cast<float>(point[0]));
        float_buffer.emplace_back(static_cast<float>(point[1]));
        lua_pop(L, 1);
    }
    float_buffer.emplace_back(float_buffer.front());
    float_buffer.emplace_back(float_buffer.front() + 1);
    SDL_Color c{};
    SDL_GetRenderDrawColor(util::renderer(), &c.r, &c.g, &c.b, &c.a);
    if (SDL_RenderGeometryRaw(
        util::renderer(), nullptr,
        float_buffer.data(), 8, &c, 0, nullptr, 0, float_buffer.size() / 2, nullptr, 0, 0)) {
        printerr(SDL_GetError());
    }
    return 0;
}
static int draw_string(lua_State* L) {
    if (is_type<builtin::Font>(L, 1)) {
        const Mat3f& tr = is_type<Mat3>(L, 3) ? Mat3f(check<Mat3>(L, 3)) : util::default_transform;
        util::draw_string(check<builtin::Font>(L, 1), luaL_checkstring(L, 2), tr);
        return 0;

    }
    std::string_view text = luaL_checkstring(L, 1);
    if (is_type<Mat3>(L, 2)) {
        Mat3f tr = check<Mat3>(L, 2);
        util::draw_string(engine::default_font(), text, tr);
        return 0;
    }
    return lua_err::invalid_type(L);
}
static int clear(lua_State* L) {
    if (is_type<Color>(L, 1)) {
        auto& c = check<Color>(L, 1);
        SDL_SetRenderDrawColor(util::renderer(), c.r, c.g, c.b, c.a);
    }
    SDL_RenderClear(util::renderer());
    return 0;
}
static int flush(lua_State* L) {
    SDL_RenderFlush(util::renderer());
    return 0;
}
static int draw_texture(lua_State* L) {
    const builtin::Texture& r = check<builtin::Texture>(L, 1);
    SDL_Rect src{0, 0, r.w, r.h};
    SDL_Rect dst = src;
    if (is_type<Mat3>(L, 2)) {
        auto& transform = check<Mat3>(L, 2);
        auto xy = util::get_quad_transform_raw(Vec2i{r.w, r.h}, transform);
        SDL_Color c{0xff, 0xff, 0xff, 0xff};
        SDL_GetTextureColorMod(r.ptr.get(), &c.r, &c.g, &c.b);
        SDL_GetTextureAlphaMod(r.ptr.get(), &c.a);
        if (SDL_RenderGeometryRaw(
            util::renderer(),
            r.ptr.get(),
            xy.data(), util::vertex_stride,
            &c, 0,
            util::quad_uv.data(),
            util::vertex_stride, 4,
            util::quad_indices.data(),
            util::quad_indices.size(), sizeof(int))) {
            return err_sdl(L);
        }
        return 0;
    }
    if (is_type<Vec2>(L, 2)) {
        const auto& pos = check<Vec2>(L, 2);
        dst.x = pos[0];
        dst.y = pos[1];
    } else if (is_type<Rect>(L, 2)) {
        const auto& dim = check<Rect>(L, 2);
        dst.x = dim.x;
        dst.y = dim.y;
        dst.w = dim.w;
        dst.h = dim.h;
    }
    SDL_RenderCopy(util::renderer(), r.ptr.get(), &src, &dst);
    return 0;
}
static constexpr std::string_view draw_color = "color";
static constexpr std::string_view blend_mode = "blend_mode";
static constexpr std::string_view viewport = "viewport";
static constexpr std::string_view vsync_enabled = "vsync_enabled";
static constexpr std::string_view clip_rect = "clip_rect";
static constexpr std::string_view scale = "scale";
static consteval char char_v(const std::string_view v) {
    return *v.data();
}
static int index(lua_State* L) {
    const std::string_view key = luaL_checkstring(L, 2);
    switch (*key.data()) {
        case char_v(blend_mode): {
            engine::expect (key == blend_mode);
            SDL_BlendMode bm;
            SDL_GetRenderDrawBlendMode(util::renderer(), &bm);
            lua_pushstring(L, blendmode_to_string(bm));
            return 1;
        }
        case char_v(viewport): {
            if (key == viewport) {
                SDL_Rect vp;
                SDL_RenderGetViewport(util::renderer(), &vp);
                create<Rect>(L, 
                    static_cast<double>(vp.x),
                    static_cast<double>(vp.y),
                    static_cast<double>(vp.w),
                    static_cast<double>(vp.h)
                );
                return 1;
            } else if (key == vsync_enabled) {
                if (SDL_GetRendererInfo(util::renderer(), &info) != 0) {
                    luaL_error(L, SDL_GetError());
                }
                lua_pushboolean(L, info.flags & SDL_RENDERER_PRESENTVSYNC);
                return 1;
            }
        }
        case char_v(clip_rect): {
            if (key == draw_color) {
                create<Color>(L, util::current_draw_color());
                return 1;
            } else if (key == clip_rect) {
                SDL_Rect cr;
                SDL_RenderGetClipRect(util::renderer(), &cr);
                create<Rect>(L,
                    static_cast<double>(cr.x),
                    static_cast<double>(cr.y),
                    static_cast<double>(cr.w),
                    static_cast<double>(cr.h)
                );
                return 1;
            }
        }
        case char_v(scale): {
            engine::expect(key == scale);
            Vec2f scale{};
            SDL_RenderGetScale(util::renderer(), &scale[0], &scale[1]);
            create<Vec2>(L) = scale;
            return 1;
        }
    }
    return 0;
}
static int newindex(lua_State* L) {
    const std::string_view key = luaL_checkstring(L, 2);
    constexpr int value_idx = 3;
    switch (*key.data()) {
        case char_v(blend_mode): {
            engine::expect(key == blend_mode);
            const char* bm = luaL_checkstring(L, value_idx);
            SDL_SetRenderDrawBlendMode(engine::renderer(), string_to_blendmode(bm));
            return 0;
        }
        case char_v(viewport): {
            if (key == vsync_enabled) {
                const bool enabled = luaL_checkboolean(L, value_idx);
                SDL_RenderSetVSync(engine::renderer(), enabled);
                return 0;
            } else if (key == viewport) {
                auto& r = check<Rect>(L, value_idx);
                SDL_Rect vp{
                    static_cast<int>(r.x),
                    static_cast<int>(r.y),
                    static_cast<int>(r.w),
                    static_cast<int>(r.h),
                };
                SDL_RenderSetViewport(engine::renderer(), &vp);
                return 0;
            }
        }
        case char_v(clip_rect): {
            if (key == draw_color) {
                auto& color = check<Color>(L, value_idx);
                SDL_SetRenderDrawColor(engine::renderer(), color.r, color.g, color.b, color.a);
                return 0;
            } else if (key == clip_rect) {
                if (is_type<Rect>(L, value_idx)) {
                    const Rect& r = check<Rect>(L, value_idx);
                    SDL_Rect cr{
                        static_cast<int>(r.x),
                        static_cast<int>(r.y),
                        static_cast<int>(r.w),
                        static_cast<int>(r.h),
                    };
                    SDL_RenderSetClipRect(engine::renderer(), &cr);
                } else if (lua_isnil(L, value_idx)) {
                    SDL_RenderSetClipRect(engine::renderer(), nullptr);
                }
                return 0;
            }
        }
        case char_v(scale): {
            engine::expect(key == scale);
            const auto& s = check<Vec2>(L, value_idx);
            SDL_RenderSetScale(engine::renderer(), static_cast<float>(s[0]), static_cast<float>(s[1]));
            return 0;
        }
    }
    luaL_error(L, "invalid index");
    return 0;
}
static int is_clip_enabled(lua_State* L) {
    SDL_bool enabled = SDL_RenderIsClipEnabled(engine::renderer());
    lua_pushboolean(L, static_cast<bool>(enabled));
    return 1;
}
namespace builtin {
int graphics_module(lua_State *L) {
    const luaL_Reg lib[] = {
        {"draw_rect", draw_rectangle},
        {"draw_rects", draw_rectangles},
        {"fill_rect", fill_rectangle},
        {"fill_rects", fill_rectangles},
        {"draw_line", draw_line},
        {"draw_lines", draw_lines},
        {"draw_pixel", draw_point},
        {"draw_pixels", draw_points},
        {"draw_polygon", draw_polygon},
        {"fill_polygon", fill_polygon},
        {"fill_circle", fill_circle},
        {"fill_ellipse", fill_ellipse},
        {"draw_string", draw_string},
        {"draw_texture", draw_texture},
        {"clear", clear},
        {"is_clip_enabled", is_clip_enabled},
        {nullptr, nullptr}
    };
    const luaL_Reg draw[] = {
        {"rect", draw_rectangle},
        {"rects", draw_rectangles},
        {"line", draw_line},
        {"lines", draw_lines},
        {"pixel", draw_point},
        {"pixels", draw_points},
        {"string", draw_string},
        {"texture", draw_texture},
        {nullptr, nullptr},
    };
    const luaL_Reg fill[] = {
        {"rect", fill_rectangle},
        {"rects", fill_rectangles},
        {"circle", fill_circle},
        {"ellipse", fill_ellipse},
        {nullptr, nullptr}
    };
    const luaL_Reg render[] = {
        {"text", draw_string},
        {"texture", draw_texture},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    lua_newtable(L);
    luaL_register(L, nullptr, draw);
    lua_setfield(L, -2, "draw");
    lua_newtable(L);
    luaL_register(L, nullptr, fill);
    lua_setfield(L, -2, "fill");
    lua_newtable(L);
    luaL_register(L, nullptr, render);
    lua_setfield(L, -2, "render");
    if (luaL_newmetatable(L, "builtin_graphics_module")) {
        const luaL_Reg meta[] = {
            {metamethod::index, index},
            {metamethod::newindex, newindex},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
    }
    lua_setmetatable(L, -2);
    return 1;
}
}
