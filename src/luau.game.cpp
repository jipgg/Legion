#include "luau.h"
#include "luau.defs.h"
using Update_connection = event::Async<double>::Connection;
using Void_connection = event::Async<>::Connection;

namespace luau {
void game::init_lib(lua_State *L) {
    const luaL_Reg lib[] = {
        {"on_update", on_update},
        {"on_render", on_render},
        {"on_start", on_start},
        {"on_quit", on_quit},
        {nullptr, nullptr}
    };
    luaL_register(L, lib_name, lib);
    if (luaL_newmetatable(L, metatable_name<Update_connection>())) {
        const luaL_Reg metadata[] = {
            {"__namecall", [](lua_State* L) {
                auto& connection = ref<Update_connection>(L, 1);
                std::string_view key = luaL_checkstring(L, 2);
                if (key == "disconnect") {
                    connection.disconnect();
                }
                return 0;
            }},
            {nullptr, nullptr}
        };
    }
    if (luaL_newmetatable(L, metatable_name<Void_connection>())) {
        const luaL_Reg metadata[] = {
            {"__namecall", [](lua_State* L) {
                auto& connection = ref<Void_connection>(L, 1);
                std::string_view key = luaL_checkstring(L, 2);
                if (key == "disconnect") {
                    connection.disconnect();
                }
                return 0;
            }},
            {nullptr, nullptr}
        };
    }
}
struct Update_handler: public Function<double> {
    Update_handler(Fn_ref&& r): Function<double>(std::move(r)) {}
    void operator()(double delta_s) override {
        lua_getref(state(), *fn_ref);
        lua_pushnumber(state(), delta_s);
        lua_pcall(state(), 1, 0, 0);
    }
};
struct Void_handler: public Function<> {
    lua_State* L;
    Void_handler(lua_State* L, Fn_ref&& r): L(L), Function<>(std::move(r)) {}
    void operator()() override {
        if (*fn_ref == LUA_NOREF) {
            common::printerr("no ref");
            return;
        }
        lua_rawgeti(L, LUA_REGISTRYINDEX, *fn_ref);
        if (not lua_isLfunction(L, -1)) {
            lua_pop(L, 1);
            common::printerr("not a function");
            return;
        }
        try {
            lua_call(L, 0, 0);
        } catch (std::exception& e) {
            common::printerr(e.what());
        }
    }
};
int game::on_update(lua_State *L) {
    if (not lua_isfunction(L, 1)) {
        common::printerr("argument is not a function");
        return 0;
    }
    init<Update_connection>(L, update.connect(Update_handler{make_fn_ref(lua_ref(L, 1))}, true));
    return 1;
}
int game::on_start(lua_State *L) {
    if (not lua_isfunction(L, 1)) {
        common::printerr("argument is not a function");
        return 0;
    }
    init<Void_connection>(L, start.connect(Void_handler{L, make_fn_ref(lua_ref(L, 1))}, true));
    return 1;
}
int game::on_quit(lua_State *L) {
    if (not lua_isfunction(L, 1)) {
        common::printerr("argument is not a function");
        return 0;
    }
    init<Void_connection>(L, quit.connect(Void_handler{L, make_fn_ref(lua_ref(L, 1))}, true));
    return 1;
}
int game::on_render(lua_State *L) {
    if (not lua_isfunction(L, 1)) {
        common::printerr("argument is not a function");
        lua_error(L);
        return 0;
    }
    auto ref = std::make_shared<int>(lua_ref(L, 1));
    init<Void_connection>(L, render.connect(Void_handler{L, std::move(ref)}, true));
    return 1;
}
}
