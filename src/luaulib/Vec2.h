#pragma once
struct lua_State;
namespace luaulib {
namespace vec2 {
void init_metadata(lua_State* L);
int ctor(lua_State* L);
int add(lua_State* L);
int mul(lua_State* L);
int index(lua_State* L);
int newindex(lua_State* L);
int metatable(lua_State* L);
int namecall(lua_State* L);
int tostring(lua_State* L);
}
}
