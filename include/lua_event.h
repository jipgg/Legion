#pragma once
#include "common.h"
#include <lualib.h>
#include <lua.h>
#include <vector>

struct lua_event {
    std::vector<std::pair<int, int>> refs;
    lua_State* L;
    static constexpr int nullid = 0;
    int curr_id{nullid};
    lua_event(lua_State* L) : L(L), refs() {}
    ~lua_event() {
        for (auto& [id, ref] : refs) {
            lua_unref(L, ref);
        }
    }
    int connect(int idx) {
        if (!lua_isfunction(L, idx)) {
            printerr("connect() error: Value is not a function\n");
            lua_pop(L, 1); // Pop table from stack
            return nullid;
        }
        refs.push_back(std::make_pair(++curr_id, lua_ref(L, idx)));
        return curr_id;
    }
    void disconnect(int id) {
        using pair_t = std::pair<int, int>;
        const pair_t dummy{id, 0};
        auto it = std::lower_bound(refs.begin(), refs.end(), dummy, [](const pair_t& e, const pair_t& v) {
            return e.first < v.first;
        });
        if (it != refs.end() and it->first == id) {
            refs.erase(it);
        } else luaL_error(L, "invalid id");
    }
    void fire(int arg_count) {
        for (auto& [id, ref] : refs) {
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
