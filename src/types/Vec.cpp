#include "builtin.h"
#include "builtin_types.h"
#include "lua_util.h"
#include "lua_atom.h"
static constexpr auto type = "Vec";
using builtin::Vec;

static int ctor(lua_State *L) {
    const int top = lua_gettop(L);
    auto& r = create<Vec>(L, Vec{});
    r.resize(top);
    for (int i{}; i < top; ++i) {
        r[i] = luaL_checknumber(L, i + 1);
    }
    return 1;
}
static int add(lua_State* L) {
    create<Vec>(L, check<Vec>(L, 1) + check<Vec>(L, 2));
    return 1;
}
static int mul(lua_State *L) {
    double scalar = luaL_checknumber(L, 2);
    create<Vec>(L, check<Vec>(L, 1) * scalar);
    return 1;
}
static int tostring(lua_State *L) {
    const auto& self = check<Vec>(L, 1);
    std::stringstream ss{};
    ss << type << ": {";
    for (int i{}; i < self.size(); ++i) {
        double v = self[i];
        std::string s = std::to_string(v);
        if (std::floor(v) == v) s.erase(s.find('.'));
        ss << s;
        if (i != self.size() - 1) ss << ", ";
        else ss << "}";
    }
    const std::string str = ss.str();
    lua_pushlstring(L, str.data(), str.size());
    return 1;
}
static int sub(lua_State* L) {
    create<Vec>(L, check<Vec>(L, 1) - check<Vec>(L, 2));
    return 1;
}
static int div(lua_State *L) {
    double scalar = luaL_checknumber(L, 2);
    create<Vec>(L, check<Vec>(L, 1) / scalar);
    return 1;
}
static int unm(lua_State* L) {
    create<Vec>(L, -check<Vec>(L, 1));
    return 1;
}
static int namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    Vec& r = check<Vec>(L, 1);
    using la = lua_atom;
    switch(static_cast<lua_atom>(atom)) {
        case la::dot: {
            const double dot = blaze::dot(check<Vec>(L, 1), check<Vec>(L, 2));
            lua_pushnumber(L, dot);
            return 1;
        }
        case la::normalized: {
            create<Vec>(L, blaze::normalize(check<Vec>(L, 1)));
            return 1;
        }
        case la::abs: {
            create<Vec>(L, blaze::abs(check<Vec>(L, 1)));
            return 0;
        }
        case la::norm: {

            auto& r = check<Vec>(L, 1);
            lua_pushnumber(L, blaze::length(r));
            return 1;
        }
        case la::set: {
            const int index = luaL_checkinteger(L, 2);
            const double value = luaL_checknumber(L, 3);
            if (not_in_range(index, r.size())) return lua_err::out_of_range(L, type);
            r[index] = value;
            return 0;
        }
        case la::at: {
            const int index = luaL_checkinteger(L, 2);
            if (not_in_range(index, r.size())) return lua_err::out_of_range(L, type);
            lua_pushnumber(L, r[index]);
            return 1;
        }
        case la::extend: {
            const int size = luaL_checkinteger(L, 2);
            r.extend(size, luaL_optboolean(L, 3, true));
            return 0;
        }
        case la::reset: {
            r.reset();
            return 0;
        }
        case la::size: {
            lua_pushinteger(L, r.size());
            return 1;
        }
        case la::capacity: {
            lua_pushinteger(L, r.capacity());
            return 1;
        }
        case la::resize: {
            r.resize(luaL_checkinteger(L, 2), luaL_optboolean(L, 3, true));
            return 0;
        }
        case la::reserve: {
            r.reserve(luaL_checkinteger(L, 2));
            return 0;
        }
        default: return lua_err::invalid_method(L, type);
    }
}
static int call(lua_State* L) {
    Vec& r = check<Vec>(L, 1);
    const int index = luaL_checkinteger(L, 2);
    if (not_in_range(index, r.size())) return lua_err::out_of_range(L, type);
    lua_pushnumber(L, r[index]);
    return 1;
}
namespace builtin {
void register_vec_type(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<Vec>())) {
        const luaL_Reg meta [] = {
            {metamethod::add, add},
            {metamethod::mul, mul},
            {metamethod::unm, unm},
            {metamethod::div, div},
            {metamethod::sub, sub},
            {metamethod::namecall, namecall},
            {metamethod::tostring, tostring},
            {metamethod::call, call},
            {nullptr, nullptr}
        };
        lua_pushstring(L, type);
        lua_setfield(L, -2, metamethod::type);
        luaL_register(L, nullptr, meta);
    }
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, type);
    lua_setglobal(L, type);
}

}
