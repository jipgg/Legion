#include "lua_util.h"
#include "builtin.h"
#include <lualib.h>
#include <lua.h>
#include <luaconf.h>
#include <SDL.h>
#include <SDL_image.h>
#include <cstddef>
#include "engine.h"
static std::vector<SDL_Point> point_buffer;
static std::vector<SDL_Rect> rect_buffer;
static std::vector<SDL_Vertex> vertex_buffer;
namespace bi = builtin;
using bi::color;
using bi::rectangle;
using bi::vector2;
using bi::texture;
static int err_sdl(lua_State* L) {
    luaL_error(L, "SDL Error: %s", SDL_GetError());
}

static SDL_Renderer* renderer() {
    return SDL_GetRenderer(engine::window());
}
static int set_draw_color(lua_State* L) {
    auto& c = check<color>(L, 1);
    SDL_SetRenderDrawColor(renderer(), c.r, c.g, c.b, c.a);
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
static int render_geometry_raw(lua_State* L) {
    SDL_Texture* texture{nullptr};
    if (is_type<bi::texture_ptr>(L, 1)) {
        texture = check<bi::texture_ptr>(L, 1).get();
    }
    size_t len{};
    void* buffer = luaL_checkbuffer(L, 2, &len);
    int xy_offset = luaL_checkinteger(L, 3);
    int xy_stride = luaL_checkinteger(L, 4);
    void* xy = static_cast<byte*>(buffer) + xy_offset;
    int color_offset = luaL_checkinteger(L, 5);
    int color_stride = luaL_checkinteger(L, 6);
    void* color = static_cast<byte*>(buffer) + color_offset;
    int uv_offset = luaL_checkinteger(L, 7);
    int uv_stride = luaL_checkinteger(L, 8);
    void* uv = static_cast<byte*>(buffer) + uv_offset;
    int num_vertices = luaL_checkinteger(L, 9);
    int indices_offset = luaL_checkinteger(L, 10);
    int num_indices = luaL_checkinteger(L, 11);
    int size_indices = luaL_checkinteger(L, 12);
    void* indices = static_cast<byte*>(buffer) + indices_offset;
    
    if (SDL_RenderGeometryRaw(renderer(),
        texture,
        static_cast<float*>(xy),
        xy_stride,
        static_cast<SDL_Color*>(color),
        color_stride,
        static_cast<float*>(uv),
        uv_stride, num_vertices,
        indices,
        num_indices,
        size_indices
    ) == -1) return err_sdl(L);
    return 0;
}
static int render_geometry(lua_State* L) {
    int vertex_count = lua_gettop(L);
    int arg_offset = 1;
    SDL_Texture* texture = nullptr;
    if (is_type<bi::texture_ptr>(L, 1)) {
        texture = check<bi::texture_ptr>(L, 1).get();
        --vertex_count;
        ++arg_offset;
    }
    vertex_buffer.resize(vertex_count);
    for (int i{}; i < vertex_count; ++i) {
        vertex_buffer[i] = check<SDL_Vertex>(L, i + arg_offset);
    }
    if (SDL_RenderGeometry(renderer(), texture, vertex_buffer.data(), vertex_buffer.size(), nullptr, 0)) {
        luaL_error(L, "SDL Error: %s", SDL_GetError());
    }
    return 0;
}
static int render_quad(lua_State* L) {
    int vertex_count = lua_gettop(L);
    int arg_offset = 1;
    SDL_Texture* texture = nullptr;
    if (is_type<bi::texture_ptr>(L, 1)) {
        texture = check<bi::texture_ptr>(L, 1).get();
        --vertex_count;
        ++arg_offset;
    }
    vertex_buffer.resize(vertex_count);
    for (int i{}; i < vertex_count; ++i) {
        vertex_buffer[i] = check<SDL_Vertex>(L, i + arg_offset);
    }
    int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    if (SDL_RenderGeometry(renderer(), texture, vertex_buffer.data(), 4, indices, 6)) {
        luaL_error(L, "SDL Error: %s", SDL_GetError());
    }
    return 0;
}
static int clear(lua_State* L) {
    if (is_type<color>(L, 1)) {
        auto& c = check<color>(L, 1);
        SDL_SetRenderDrawColor(renderer(), c.r, c.g, c.b, c.a);
    }
    SDL_RenderClear(renderer());
    return 0;
}
static int flush(lua_State* L) {
    SDL_RenderFlush(renderer());
    return 0;
}
static int set_draw_blendmode(lua_State* L) {
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
static int render_copy(lua_State* L) {
    const texture& r = check<texture>(L, 1);
    SDL_Rect src{0, 0, r.w, r.h};
    SDL_Rect dst = src;
    if (is_type<bi::matrix3>(L, 2)) {
        auto& transform = check<bi::matrix3>(L, 2);
        blaze::StaticVector<float, 3> top_left = transform * blaze::StaticVector{0, 0, 1};
        blaze::StaticVector<float, 3> top_right = transform * blaze::StaticVector{r.w, 0, 1};
        blaze::StaticVector<float, 3> bottom_right = transform * blaze::StaticVector{r.w, r.h, 1};
        blaze::StaticVector<float, 3> bottom_left = transform * blaze::StaticVector{0, r.h, 1};
        float vertices[] = {
            top_left[0], top_left[1],
            top_right[0], top_right[1],
            bottom_right[0], bottom_right[1],
            bottom_left[0], bottom_left[1],
        };
        float uv[] = {
            0, 0,
            1, 0,
            1, 1,
            0, 1
        }; 
        int indices[] = {
            0, 1, 2,
            2, 3, 0
        };
        SDL_Color c{0xff, 0xff, 0xff, 0xff};
        const int stride = sizeof(float) * 2;
        if (SDL_RenderGeometryRaw(renderer(), r.ptr.get(), vertices, stride, &c, 0, uv, stride, 4, indices, 6, 4)) {
            return err_sdl(L);
        }
        return 0;
    } else if (is_type<vector2>(L, 2)) {
        const auto& pos = check<vector2>(L, 2);
        dst.x = pos[0];
        dst.y = pos[1];
    } else if (is_type<rectangle>(L, 2)) {
        const auto& dim = check<rectangle>(L, 2);
        dst.x = dim.x;
        dst.y = dim.y;
        dst.w = dim.w;
        dst.h = dim.h;
    }
    SDL_RenderCopy(renderer(), r.ptr.get(), &src, &dst);
    return 0;
}
int builtin::lib_rendering(lua_State *L) {
    const luaL_Reg lib[] = {
        {"render_geometry_raw", render_geometry_raw},
        {"clear", clear},
        {"flush", flush},
        {"render_texture", render_copy},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}

