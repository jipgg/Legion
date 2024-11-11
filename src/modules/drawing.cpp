#include "lua_util.h"
#include "builtin.h"
#include <lualib.h>
#include <lua.h>
#include <luaconf.h>
#include <SDL.h>
#include <SDL_image.h>
#include <cstddef>
#include "engine.h"
static std::vector<float> float_buffer;
static std::vector<SDL_Point> point_buffer;
static std::vector<SDL_Rect> rect_buffer;
namespace bi = builtin;
using bi::color;
using bi::rectangle;
using bi::vector2;
static int err_sdl(lua_State* L) {
    luaL_error(L, "SDL Error: %s", SDL_GetError());
}
__forceinline static void fill_ellipse_with_alpha_channel_impl(
    SDL_Renderer* renderer, int center_x, int center_y, int radius_x, int radius_y) {
    SDL_Color color;
    SDL_GetRenderDrawColor(renderer, &color.r, &color.g, &color.b, &color.a);
    int rx_squared = radius_x * radius_x;
    int ry_squared = radius_y * radius_y;
    for (int y = -radius_y; y <= radius_y; y++) {
        for (int x = -radius_x; x <= radius_x; x++) {
            float distance_squared = (x * x) / (float)rx_squared + (y * y) / (float)ry_squared;
            if (distance_squared <= 1.0f) {
                SDL_RenderDrawPoint(renderer, center_x + x, center_y + y);
            }
        }
    }
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}
__forceinline static void fill_circle_with_alpha_channel_impl(SDL_Renderer* renderer, int center_x, int center_y, int radius) {
    int inner = radius - 1;
    int inner_squared = inner * inner;
    int radius_squared = radius * radius;
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            int distance_squared = x * x + y * y;
            if (distance_squared <= radius_squared) {
                SDL_RenderDrawPoint(renderer, center_x + x, center_y + y);
            }
        }
    }
}
__forceinline static void fill_circle_impl(SDL_Renderer* renderer, int x, int y, int radius) {
    int dx = 0;
    int dy = radius;
    int d = 1 - radius;
    while (dy >= dx) {
        SDL_RenderDrawLine(renderer, x - dx, y + dy, x + dx, y + dy);
        SDL_RenderDrawLine(renderer, x - dx, y - dy, x + dx, y - dy);
        SDL_RenderDrawLine(renderer, x - dy, y + dx, x + dy, y + dx);
        SDL_RenderDrawLine(renderer, x - dy, y - dx, x + dy, y - dx);
        if (d < 0) d += 2 * dx + 3;
        else {
            d += 2 * (dx - dy) + 5;
            dy--;
        }
        dx++;
    }
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
}
__forceinline static SDL_Renderer* renderer() {
    return SDL_GetRenderer(engine::window());
}
static int set_color(lua_State* L) {
    auto& c = check<color>(L, 1);
    SDL_SetRenderDrawColor(renderer(), c.r, c.g, c.b, c.a);
    return 0;
}
static int set_blend_mode(lua_State* L) {
    std::string_view mode = luaL_checkstring(L, 1);
    SDL_BlendMode blend;
    if (mode == "none") blend = SDL_BLENDMODE_NONE;
    else if (mode == "mul") blend = SDL_BLENDMODE_MUL;
    else if (mode == "add") blend = SDL_BLENDMODE_ADD;
    else if (mode == "mod") blend = SDL_BLENDMODE_MOD;
    else if (mode == "blend") blend  = SDL_BLENDMODE_BLEND;
    else blend = SDL_BLENDMODE_INVALID;
    SDL_SetRenderDrawBlendMode(renderer(), blend);
    return 0;
}
static int draw_rectangle(lua_State* L) {
    SDL_Rect dummy{};
    if (is_type<rectangle>(L, 1)) {
        auto& rect = check<rectangle>(L, 1);
        dummy = {
            static_cast<int>(rect.x),
            static_cast<int>(rect.y),
            static_cast<int>(rect.w),
            static_cast<int>(rect.h)
        };
    } else if (is_type<vector2>(L, 1)) {
        auto& origin = check<vector2>(L, 1);
        auto& size = check<vector2>(L, 2);
        dummy = {
            static_cast<int>(origin[0]),
            static_cast<int>(origin[1]),
            static_cast<int>(size[0]),
            static_cast<int>(size[1])
        };
    
    } else return err_invalid_type(L);
    SDL_RenderDrawRect(renderer(), &dummy);
    return 0;
}
static int draw_rectangles(lua_State* L) {
    const int top = lua_gettop(L);
    rect_buffer.resize(0);
    rect_buffer.reserve(top);
    SDL_Rect dummy{};
    for (int i{1}; i <= top; ++i) {
        auto& r = check<rectangle>(L, i);
        dummy = {
            static_cast<int>(r.x),
            static_cast<int>(r.y),
            static_cast<int>(r.w),
            static_cast<int>(r.h)
        };
        rect_buffer.emplace_back(dummy);
    }
    SDL_RenderDrawRects(renderer(), rect_buffer.data(), rect_buffer.size());
    return 0;
}
static int draw_points(lua_State* L) {
    const int top = lua_gettop(L);
    point_buffer.resize(0);
    point_buffer.reserve(top);
    for (int i{1}; i <= top; ++i) {
        auto& p = check<vector2>(L, i);
        point_buffer.emplace_back(SDL_Point{static_cast<int>(p.at(0)), static_cast<int>(p.at(1))});
    }
    SDL_RenderDrawPoints(renderer(), point_buffer.data(), point_buffer.size());
    return 0;
}
static int fill_rectangle(lua_State* L) {
    SDL_Rect dummy{};
    if (is_type<rectangle>(L, 1)) {
        auto& rect = check<rectangle>(L, 1);
        dummy = {
            static_cast<int>(rect.x),
            static_cast<int>(rect.y),
            static_cast<int>(rect.w),
            static_cast<int>(rect.h)
        };
    } else if (is_type<vector2>(L, 1)) {
        auto& origin = check<vector2>(L, 1);
        auto& size = check<vector2>(L, 2);
        dummy = {
            static_cast<int>(origin[0]),
            static_cast<int>(origin[1]),
            static_cast<int>(size[0]),
            static_cast<int>(size[1])
        };
    
    } else return err_invalid_type(L);
    SDL_RenderFillRect(renderer(), &dummy);
    return 0;
}
static int fill_rectangles(lua_State* L) {
    const int top = lua_gettop(L);
    rect_buffer.resize(top);
    for (int i{1}; i <= top; ++i) {
        auto& r = check<rectangle>(L, i);
        rect_buffer[i - 1] = SDL_Rect{
            static_cast<int>(r.x),
            static_cast<int>(r.y),
            static_cast<int>(r.w),
            static_cast<int>(r.h),
        };
    }
    SDL_RenderFillRects(renderer(), rect_buffer.data(), rect_buffer.size());
    return 0;
}
static int draw_line(lua_State* L) {
    auto& t0 = check<vector2>(L, 1);
    auto& t1 = check<vector2>(L, 2);
    SDL_RenderDrawLine(renderer(), t0.at(0), t0.at(1), t1.at(0), t1.at(1));
    return 0;
}
static int draw_point(lua_State* L) {
    auto& p = check<bi::vector2>(L, 1);
    SDL_RenderDrawPoint(renderer(), p.at(0), p.at(1));
    return 0;
}
static int draw_lines(lua_State* L) {
    const int top = lua_gettop(L);
    point_buffer.resize(0);
    point_buffer.reserve(top);
    for (int i{1}; i <= top; ++i) {
        auto& p = check<bi::vector2>(L, i);
        point_buffer.emplace_back(SDL_Point{int(p.at(0)), int(p.at(1))});
    }
    SDL_RenderDrawLines(renderer(), point_buffer.data(), point_buffer.size());
    return 0;
}

static int draw_polygon(lua_State* L) {
    point_buffer.resize(0);
    for (int i{1}; i <= lua_objlen(L, 1); ++i) {
        lua_rawgeti(L, 1, i);
        auto& point = check<vector2>(L, -1);
        point_buffer.emplace_back(SDL_Point{
            static_cast<int>(point[0]),
            static_cast<int>(point[1])
        });
        lua_pop(L, 1);
    }
    point_buffer.emplace_back(point_buffer.front());
    SDL_RenderDrawLines(renderer(), point_buffer.data(), point_buffer.size());
    return 0;
}

static int fill_circle(lua_State* L) {
    auto& center = check<vector2>(L, 1);
    double radius = luaL_checknumber(L, 2);
    const bool has_alpha_channel = luaL_optboolean(L, 3, false);
    if (has_alpha_channel) {
        fill_circle_with_alpha_channel_impl(renderer(), int(center[0]), int(center[1]), int(radius));
        return 0;
    }
    fill_circle_impl(renderer(), int(center[0]), int(center[1]), int(radius));
    return 0;
}
static int fill_ellipse(lua_State* L) {
    auto& center = check<vector2>(L, 1);
    auto& radius = check<vector2>(L, 2);

    const int x = int(center[0]);
    const int y = int(center[1]);
    const int rx = int(radius[0]);
    const int ry = int(radius[1]);
    const bool has_alpha_channel = luaL_optboolean(L, 3, false);
    if (has_alpha_channel) {
        fill_ellipse_with_alpha_channel_impl(renderer(), x, y, rx, ry);
        return 0;
    }
    fill_ellipse_impl(renderer(), int(center[0]), int(center[1]), int(radius[0]), int(radius[1]));
    return 0;
}
static int fill_polygon(lua_State* L) {
    float_buffer.resize(0);
    const int len = lua_objlen(L, 1);
    for (int i{1}; i <= len; ++i) {
        lua_rawgeti(L, 1, i);
        auto& point = check<vector2>(L, -1);
        float_buffer.emplace_back(static_cast<float>(point[0]));
        float_buffer.emplace_back(static_cast<float>(point[1]));
        lua_pop(L, 1);
    }
    float_buffer.emplace_back(float_buffer.front());
    float_buffer.emplace_back(float_buffer.front() + 1);
    SDL_Color c{};
    SDL_GetRenderDrawColor(renderer(), &c.r, &c.g, &c.b, &c.a);
    if (SDL_RenderGeometryRaw(
        renderer(), nullptr,
        float_buffer.data(), 8, &c, 0, nullptr, 0, float_buffer.size() / 2, nullptr, 0, 0)) {
        printerr(SDL_GetError());
    }
    return 0;
}
int builtin::lib_drawing(lua_State *L) {
    const luaL_Reg lib[] = {
        {"set_color", set_color},
        {"set_blend_mode", set_blend_mode},
        {"draw_rectangle", draw_rectangle},
        {"draw_rectangles", draw_rectangles},
        {"fill_rectangle", fill_rectangle},
        {"fill_rectangles", fill_rectangles},
        {"draw_line", draw_line},
        {"draw_lines", draw_lines},
        {"draw_point", draw_point},
        {"draw_points", draw_points},
        {"draw_polygon", draw_polygon},
        {"fill_polygon", fill_polygon},
        {"fill_circle", fill_circle},
        {"fill_ellipse", fill_ellipse},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}
