#include "lua_util.h"
#include "builtin.h"
#include <SDL.h>
#include "engine.h"
#include "common.h"
using namespace std::string_literals;
using namespace std::string_view_literals;
static constexpr auto lib_name = "builtin";
using engine::window;
namespace bi = builtin;

static int maximize_window(lua_State* L) {
    SDL_MaximizeWindow(window());
    return 0;
}
int builtin::fn_get_mouse_position(lua_State* L) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    create<builtin::vector2>(L) = {static_cast<double>(x), static_cast<double>(y)};
    return 1;
}
int builtin::fn_is_key_down(lua_State* L) {
    std::string_view key = luaL_checkstring(L, 1);
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    lua_pushboolean(L, state[string_to_scancode(key)]);
    return 1;
}
int builtin::fn_is_point_in_rect(lua_State* L) {
    SDL_FPoint point{};
    SDL_FRect rect{};
    SDL_PointInFRect(&point, &rect);
    constexpr auto pt_arg = 1;
    constexpr auto rct_arg = 2;
    if (is_type<bi::vector2>(L, pt_arg)) {
        auto& p = check<bi::vector2>(L, pt_arg);
        point.x = p[0];
        point.y = p[1];
    } else if (is_type<bi::vector>(L, pt_arg)) {
        auto& p = check<bi::vector>(L, pt_arg);
        if (p.size() > 2) luaL_error(L, "invalid vector size");
        point.x = p[0];
        point.y = p[1];
    }
    if (is_type<rectangle>(L, rct_arg)) {
        auto& r = check<rectangle>(L, rct_arg);
        rect = {float(r.x), float(r.y), float(r.w), float(r.h)};
    } else if (is_type<rectangle>(L, rct_arg)) {
        auto& r = check<rectangle>(L, rct_arg);
        rect = {float(r.x), float(r.y), float(r.w), float(r.h)};
    }
    lua_pushboolean(L, SDL_PointInFRect(&point, &rect));
    return 1;
}
int builtin::fn_read_file(lua_State* L) {
    std::filesystem::path path = luaL_checkstring(L, 1);
    if (auto contents = read_file(path)) {
        lua_pushstring(L, contents->c_str());
        return 1;
    }
    return 0;
}
