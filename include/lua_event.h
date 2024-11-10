#pragma once
#include "common.h"
#include <lualib.h>
#include <lua.h>
#include <vector>

class lua_event {
public:
    std::vector<int> refs;
    lua_State* L;
    lua_event(lua_State* L) : L(L), refs() {}
    ~lua_event() {
        for (int i : refs) {
            lua_unref(L, i);
        }
    }
    void connect(int idx) {
        if (!lua_isfunction(L, idx)) {
            printerr("connect() error: Value is not a function\n");
            lua_pop(L, 1); // Pop table from stack
            return;
        }
    refs.push_back(lua_ref(L, idx));
}
    void fire(int arg_count) {
        for (auto& ref : refs) {
            lua_getref(L, ref);
            if (lua_isfunction(L, -1)) {
                lua_pushvalue(L, -1); // Push a copy of the function
                for (int i = 0; i < arg_count; ++i) {
                    lua_pushvalue(L, -(arg_count + 2));
                }
                if (lua_pcall(L, arg_count, 0, 0) != LUA_OK) {
                    std::cerr << "Error calling function: " << lua_tostring(L, -1) << "\n";
                    lua_pop(L, 1);
                }
            }
            lua_pop(L, 1);
        }
        lua_pop(L, arg_count);
    }
};
