#include "builtin.h"
#include "lua_util.h"
using builtin::event;
using builtin::vector2;
static unique_event key_pressed;
static unique_event key_released;
static unique_event mouse_button_pressed;
static unique_event mouse_button_released;
static unique_event mouse_moved;
static unique_event mouse_wheeled;

static int is_key_down(lua_State* L) {
    std::string_view key = luaL_checkstring(L, 1);
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    lua_pushboolean(L, state[string_to_scancode(key)]);
    return 1;
}
static constexpr size_t mouse_position_length = std::string("MousePosition").length();
static int index(lua_State* L) {
    size_t length;
    const char* key = luaL_checklstring(L, 2, &length);
    switch (*key) {
        case 'M':
            engine::expect(length == mouse_position_length);
            vec2i pos{};
            SDL_GetMouseState(&pos[0], &pos[1]);
            create<vector2>(L) = pos;
            return 1;
    }
    luaL_error(L, "invalid index");
    return 0;
}
static int newindex(lua_State* L) {
    size_t length;
    const char* key = luaL_checklstring(L, 2, &length);
    switch (*key) {
        case 'M':
            engine::expect(length == mouse_position_length);
            luaL_error(L, "property 'MousePosition' is readonly");
            return 0;
    }
    luaL_error(L, "invalid index");
    return 0;
}

namespace builtin {
int userinput_module(lua_State *L) {
    const luaL_Reg functions[] = {
        {"IsKeyDown", is_key_down},
        {nullptr, nullptr}
    };
    lua_newtable(L);
    luaL_register(L, nullptr, functions);
    register_event(L, key_pressed, "KeyPressed");
    register_event(L, key_released, "KeyReleased");
    register_event(L, mouse_button_pressed, "MouseButtonPressed");
    register_event(L, mouse_button_released, "MouseButtonReleased");
    register_event(L, mouse_moved, "MouseMoved");
    register_event(L, mouse_wheeled, "MouseWheeled");
    if (luaL_newmetatable(L, "builtin_userinput_module")) {
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
void handle_userinput_event(lua_State* L, SDL_Event& e) {
    switch (e.type) {
        case SDL_KEYDOWN:
            lua_pushstring(L, scancode_to_string(e.key.keysym.scancode));
            key_pressed->fire(1);
            return;
        case SDL_KEYUP:
            lua_pushstring(L, scancode_to_string(e.key.keysym.scancode));
            key_released->fire(1);
        return;
        case SDL_MOUSEBUTTONUP:
            lua_pushstring(L, mouse_button_to_string(e.button.button));
            create<vector2>(L) = vec2i{e.button.x, e.button.y};
            mouse_button_released->fire(2);
        return;
        case SDL_MOUSEBUTTONDOWN:
            lua_pushstring(L, mouse_button_to_string(e.button.button));
            create<vector2>(L) = vec2i{e.button.x, e.button.y};
            mouse_button_pressed->fire(2);
        return;
        case SDL_MOUSEMOTION:
            create<vector2>(L) = vec2i{e.motion.x, e.motion.y};
            create<vector2>(L) = vec2i{e.motion.xrel, e.motion.yrel};
            mouse_moved->fire(2);
        return;
        case SDL_MOUSEWHEEL:
        break;
    }
}
}
