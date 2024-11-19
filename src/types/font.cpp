#include "builtin.h"
#include "lua_util.h"
using builtin::font;
using builtin::path;
static constexpr auto type = "Font";

static int index(lua_State* L) {
    auto& r = check<font>(L, 1);
    size_t len;
    const char initial = *luaL_checklstring(L, 2, &len);
    switch (initial) {
        case 'F':
            create<path>(L, r.file_path);
            return 1;
        case 'P': {
            lua_pushinteger(L, r.pt_size);
            return 1;
        }
    } 
    return lua_err::invalid_member(L, type);
}
static int newindex(lua_State* L) {
    luaL_error(L, "member access is read-only");
    return 0;
}
static int ctor(lua_State* L) {
    const auto& font_path = check<path>(L, 1);
    const int pt_size = luaL_checkinteger(L, 2);
    TTF_Font* font_resource = TTF_OpenFont(font_path.string().c_str(), pt_size);
    engine::expect(font_resource != nullptr, SDL_GetError());

    create<builtin::font>(L, builtin::font{
        .ptr{font_resource, TTF_CloseFont},
        .pt_size = pt_size,
        .file_path = font_path
    });
    return 1;
}
namespace builtin {
void register_font_type(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<font>())) {
        const luaL_Reg meta[] = {
            {metamethod::index, index},
            {metamethod::newindex, newindex},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
    }
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, "font_ctor");
    lua_setglobal(L, type);
}
}