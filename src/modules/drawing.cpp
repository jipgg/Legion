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

static SDL_Renderer* renderer() {
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
    auto& rect = check<rectangle>(L, 1);
    SDL_Rect dummy{
        static_cast<int>(rect.x),
        static_cast<int>(rect.y),
        static_cast<int>(rect.w),
        static_cast<int>(rect.h)
    };
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
    auto& rect = check<rectangle>(L, 1);
    SDL_Rect dummy{
        static_cast<int>(rect.x),
        static_cast<int>(rect.y),
        static_cast<int>(rect.w),
        static_cast<int>(rect.h)
    };
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
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}
