#include "builtin.h"
#include "builtin_types.h"
#include "engine.h"
#include <SDL.h>
#include <SDL_image.h>
#include "lua_util.h"
using engine::window;
static constexpr size_t size_length = std::string("size").length();
static constexpr size_t position_length = std::string("position").length();
static constexpr size_t title_length = std::string("title").length();
static constexpr size_t opacity_length = std::string("opacity").length();
static constexpr size_t borderless_length = std::string("borderless").length();
static constexpr size_t always_on_top_length = std::string("always_on_top").length();
static constexpr size_t fullscreen_length = std::string("fullscreen").length();
static constexpr size_t icon_length = std::string("icon").length();
using engine::expect;
using engine::window;
using builtin::Vec2;
using builtin::Event;
static UniqueEvent shown;
static UniqueEvent hidden;
static UniqueEvent mouse_entered;
static UniqueEvent mouse_left;
static UniqueEvent size_changed;
static UniqueEvent resized;
static UniqueEvent exposed;
static UniqueEvent position_changed;
static UniqueEvent minimized;
static UniqueEvent maximized;
static UniqueEvent restored;
static UniqueEvent focus_gained;
static UniqueEvent focus_lost;
static UniqueEvent closing;
static bool is_always_on_top = false;
static bool is_borderless = false;

static int index(lua_State* L) {
    size_t length;
    const char key = *luaL_checklstring(L, 2, &length);
    switch (key) {
        case 's': {
            expect(length == size_length);
            int w, h;
            SDL_GetWindowSize(engine::window(), &w, &h);
            create<Vec2>(L) = Vec2i{w, h};
            return 1;
        }
        case 'p': {
            expect(length == position_length);
            int x, y;
            SDL_GetWindowPosition(engine::window(), &x, &y);
            create<Vec2>(L) = Vec2i{x, y};
            return 1;
        }
        case 't': {
            expect(length == title_length);
            lua_pushstring(L, SDL_GetWindowTitle(engine::window()));
            return 1;
        }
        case 'o': {
            expect(length == opacity_length);
            float opacity;
            SDL_GetWindowOpacity(engine::window(), &opacity);
            return 1;
        }
        case 'a': {
            expect(length == always_on_top_length);
            lua_pushboolean(L, is_always_on_top);
            return 1;
        }
        case 'b': {
            expect(length == borderless_length);
            lua_pushboolean(L, is_borderless);
            return 1;
        }
    }
    return 0;
}
static int newindex(lua_State* L) {
    size_t length;
    const char key = *luaL_checklstring(L, 2, &length);
    switch (key) {
        case 's': {
            expect(length == size_length);
            auto& new_size = check<Vec2>(L, 3);
            SDL_SetWindowSize(window(), static_cast<int>(new_size[0]), static_cast<int>(new_size[1]));
            return 0;
        }
        case 'p': {
            expect(length == position_length);
            auto& pos = check<Vec2>(L, 3);
            SDL_SetWindowPosition(window(), static_cast<int>(pos[0]), static_cast<int>(pos[1]));
            return 0;
        }
        case 't': {
            expect(length == title_length);
            SDL_SetWindowTitle(window(), luaL_checkstring(L, 3));
            return 0;
        }
        case 'o': {
            expect(length == opacity_length);
            const float opacity = luaL_checknumber(L, 3);
            SDL_SetWindowOpacity(window(), opacity);
            return 1;
        }
        case 'a': {
            expect(length == always_on_top_length);
            const bool b = luaL_checkboolean(L, 3);
            is_always_on_top = b;
            SDL_SetWindowAlwaysOnTop(window(), static_cast<SDL_bool>(b));
            return 0;
        }
        case 'b': {
            expect(length == borderless_length);
            const bool b = luaL_checkboolean(L, 3);
            is_borderless = not b;
            SDL_SetWindowBordered(window(), static_cast<SDL_bool>(is_borderless));
            return 0;
        }
    }
    return 0;
}
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
void handle_window_event(lua_State* L, SDL_WindowEvent& e) {
    switch (e.event) {
        case SDL_WINDOWEVENT_SHOWN:
            shown->fire(0);
        break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            focus_gained->fire(0);
        break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            focus_lost->fire(0);
        break;
        case SDL_WINDOWEVENT_EXPOSED:
            exposed->fire(0);
        break;
        case SDL_WINDOWEVENT_RESIZED:
            create<Vec2>(L) = Vec2i{e.data1, e.data2};
            resized->fire(1);
        break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            create<Vec2>(L) = Vec2i{e.data1, e.data2};
            size_changed->fire(1);
        break;
        case SDL_WINDOWEVENT_ENTER:
            mouse_entered->fire(0);
        break;
        case SDL_WINDOWEVENT_LEAVE:
            mouse_left->fire(0);
        break;
        case SDL_WINDOWEVENT_MOVED:
            create<Vec2>(L) = Vec2i{e.data1, e.data2};
            position_changed->fire(1);
        break;
        case SDL_WINDOWEVENT_MAXIMIZED:
            maximized->fire(0);
        break;
        case SDL_WINDOWEVENT_MINIMIZED:
            minimized->fire(0);
        case SDL_WINDOWEVENT_RESTORED:
            restored->fire(0);
        break;
        case SDL_WINDOWEVENT_CLOSE:
            closing->fire(0);
        break;
    }
}
int window_module(lua_State *L) {
    const luaL_Reg lib[] = {
        {"maximize", maximize},
        {"minimize", minimize},
        {"set_icon", set_icon},
        {"Focus", focus},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, lib);
    register_event(L, hidden, "hidden");
    register_event(L, shown, "shown");
    register_event(L, resized, "resized");
    register_event(L, mouse_entered, "mouse_enter");
    register_event(L, mouse_left, "mouse_leave");
    register_event(L, size_changed, "size_changed");
    register_event(L, position_changed, "position_changed");
    register_event(L, focus_gained, "focus_gained");
    register_event(L, focus_lost, "focus_lost");
    register_event(L, restored, "restored");
    register_event(L, closing, "closing");
    register_event(L, exposed, "exposed");
    register_event(L, maximized, "maximized");
    register_event(L, minimized, "minimized");
    if (luaL_newmetatable(L, "builtin_window_module")) {
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
