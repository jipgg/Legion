#include "luau.defs.h"
#include "component.h"
#include "luau.h"
#include "types.h"
#include "comptime.h"
#include "engine.h"
#include <lualib.h>
using Self = component::Wrapper<types::Clickable>;
using Mouse_connection = types::Clickable::Mouse_event::Connection;

static int connection_namecall(lua_State* L) {
    auto& connection = luau::ref<Mouse_connection>(L, 1);
    std::string_view key = luaL_checkstring(L, 2);
    if (key == "disconnect") {
        connection.disconnect();
    }
    return 0;
};

void luau::Clickable::init_type(lua_State *L) {
    if (luaL_newmetatable(L, metatable_name<Self>())) {
        const luaL_Reg metadata[] = {
            {"__namecall", namecall},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, metadata);
        lua_pop(L, 1);
        lua_pushcfunction(L, ctor, type_name);
        lua_setglobal(L, type_name);
    }
    if (luaL_newmetatable(L, metatable_name<Mouse_connection>())) {
        const luaL_Reg metadata[] = {
            {"__namecall", connection_namecall},
            {nullptr, nullptr}
        };
    }
}
int luau::Clickable::ctor(lua_State *L) {
    if (not is_type<common::Recti64>(L, 1)) {common::printerr("invalid arg"); return 0;} 
    init<Self>(L, types::Clickable{.hit = ref<common::Recti64>(L, 1)});
    lua_callbacks(L)->useratom = [](const char* name, size_t idk) {
        static constexpr auto count = comptime::count<Method, Method::on_mouse_down>();
        return static_cast<int16_t>(comptime::enum_item<Method, count>(name).index);
    };
    return 1;
}
int luau::Clickable::namecall(lua_State *L) {
    int atom;
    auto& self = ref<Self>(L, 1);
    lua_namecallatom(L, &atom);
    using  Button = types::Clickable::Button;
    struct Luau_mouse_handler: public luau::Function<Button, common::Vec2i> {
        Luau_mouse_handler(std::shared_ptr<int>&& r): luau::Function<Button, common::Vec2i>(std::move(r)) {}
        void operator()(types::Clickable::Button button, common::Vec2i pos) override {
            lua_State* L = engine::core::get_lua_state();
            lua_getref(L, *fn_ref);
            lua_pushinteger(L, static_cast<int>(button));
            luau::init<common::Vec2i>(L) = pos;
            lua_pcall(L, 2, 0, 0);
        }
    };
    switch(static_cast<Method>(atom)) {
        case Method::on_mouse_down:
            init<Mouse_connection>(L, self.get().on_mouse_down.connect(Luau_mouse_handler{std::make_shared<int>(lua_ref(L, 2))}, true));
        return 1;
        case Method::on_mouse_click:
            init<Mouse_connection>(L, self.get().on_mouse_up.connect(Luau_mouse_handler{std::make_shared<int>(lua_ref(L, 2))}, true));
        return 1;
    }
    return 0;
}

