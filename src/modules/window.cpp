#include "builtin.h"
#include "engine.h"
#include <SDL.h>
#include <SDL_image.h>
using engine::window;

static int size(lua_State* L) {
    int width, height;
    SDL_GetWindowSize(window(), &width, &height);
    lua_pushinteger(L, width);
    lua_pushinteger(L, height);
    return 2;
}
static int set_size(lua_State* L) {
    SDL_SetWindowSize(window(), luaL_checkinteger(L, 1), luaL_checkinteger(L, 2));
    return 0;
}
static int maximize(lua_State* L) {
    SDL_MaximizeWindow(window());
    return 0;
}
static int minimize(lua_State* L) {
    SDL_MinimizeWindow(window());
    return 0;
}

static int set_title(lua_State* L) {
    SDL_SetWindowTitle(window(), luaL_checkstring(L, 1));
    return 0;
}
static int title(lua_State* L) {
    lua_pushstring(L, SDL_GetWindowTitle(window()));
    return 1;
}

static int set_always_on_top(lua_State* L) {
    SDL_SetWindowAlwaysOnTop(window(), SDL_bool(luaL_checkboolean(L, 1)));
    return 0;
}
static int set_borderless(lua_State* L) {
    SDL_SetWindowBordered(window(), SDL_bool(not luaL_checkboolean(L, 1)));
    return 0;
}
static int set_position(lua_State* L) {
    SDL_SetWindowPosition(window(), luaL_checkinteger(L, 1), luaL_checkinteger(L, 2));
    return 0;
}
static int position(lua_State* L) {
    int x, y;
    SDL_GetWindowPosition(window(), &x, &y);
    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    return 2;
}
static int set_opacity(lua_State* L) {
    SDL_SetWindowOpacity(window(), static_cast<float>(luaL_checknumber(L, 1)));
    return 0;
}

static int opacity(lua_State* L) {
    float opacity;
    SDL_GetWindowOpacity(window(), &opacity);
    lua_pushnumber(L, opacity);
    return 1;
}
static int set_icon(lua_State* L) {
    const char* file = lua_tostring(L, 1);
    SDL_Surface* icon = IMG_Load(file);
    if (not icon) {
        luaL_error(L, "failed to load icon '%s'", file);
        return 0;
    }
    SDL_SetWindowIcon(window(), icon);
    return 0;
}
static int focus(lua_State* L) {
    SDL_RaiseWindow(window());
    return 0;
}

namespace builtin {
int lib_window(lua_State *L) {
    const luaL_Reg lib[] = {
        {"size", size},
        {"set_size", set_size},
        {"maximize", maximize},
        {"minimize", minimize},
        {"title", title},
        {"set_title", set_title},
        {"set_always_on_top", set_always_on_top},
        {"set_borderless", set_borderless},
        {"position", position},
        {"set_position", set_position},
        {"set_opacity", set_opacity},
        {"opacity", opacity},
        {"set_icon", set_icon},
        {"focus", focus},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    return 1;
}
}
