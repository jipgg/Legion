#include "builtin.h"
#include "lua_util.h"
#include "lua_atom.h"
namespace bi = builtin;
namespace mm = bi::metamethod;
static constexpr auto tn = bi::tname::vector;
using ty =  bi::vector;

static int ctor(lua_State *L) {
    const int top = lua_gettop(L);
    auto& r = create<ty>(L, ty{});
    r.resize(top);
    for (int i{}; i < top; ++i) {
        r[i] = luaL_checknumber(L, i + 1);
    }
    return 1;
}
static int add(lua_State* L) {
    create<ty>(L, check<ty>(L, 1) + check<ty>(L, 2));
    return 1;
}
static int mul(lua_State *L) {
    double scalar = luaL_checknumber(L, 2);
    create<ty>(L, check<ty>(L, 1) * scalar);
    return 1;
}
static int tostring(lua_State *L) {
    const auto& self = check<ty>(L, 1);
    std::stringstream ss{};
    ss << tn << ": {";
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
    create<ty>(L, check<ty>(L, 1) - check<ty>(L, 2));
    return 1;
}
static int div(lua_State *L) {
    double scalar = luaL_checknumber(L, 2);
    create<ty>(L, check<ty>(L, 1) / scalar);
    return 1;
}
static int unm(lua_State* L) {
    create<ty>(L, -check<ty>(L, 1));
    return 1;
}
static int namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    ty& r = check<ty>(L, 1);
    using la = lua_atom;
    switch(static_cast<lua_atom>(atom)) {
        case la::DotProduct: {
            const double dot = blaze::dot(check<ty>(L, 1), check<ty>(L, 2));
            lua_pushnumber(L, dot);
            return 1;
        }
        case la::ToUnitVector: {
            create<ty>(L, blaze::normalize(check<ty>(L, 1)));
            return 1;
        }
        case la::Abs: {
            create<ty>(L, blaze::abs(check<ty>(L, 1)));
            return 0;
        }
        case la::Length: {

            auto& r = check<ty>(L, 1);
            lua_pushnumber(L, blaze::length(r));
            return 1;
        }
        case la::Set: {
            const int index = luaL_checkinteger(L, 2);
            const double value = luaL_checknumber(L, 3);
            if (not_in_range(index, r.size())) return lua_err::out_of_range(L, tn);
            r[index] = value;
            return 0;
        }
        case la::At: {
            const int index = luaL_checkinteger(L, 2);
            if (not_in_range(index, r.size())) return lua_err::out_of_range(L, tn);
            lua_pushnumber(L, r[index]);
            return 1;
        }
        case la::Extend: {
            const int size = luaL_checkinteger(L, 2);
            r.extend(size, luaL_optboolean(L, 3, true));
            return 0;
        }
        case la::Reset: {
            r.reset();
            return 0;
        }
        case la::Size: {
            lua_pushinteger(L, r.size());
            return 1;
        }
        case la::Capacity: {
            lua_pushinteger(L, r.capacity());
            return 1;
        }
        case la::Resize: {
            r.resize(luaL_checkinteger(L, 2), luaL_optboolean(L, 3, true));
            return 0;
        }
        case la::Reserve: {
            r.reserve(luaL_checkinteger(L, 2));
            return 0;
        }
        default: return lua_err::invalid_method(L, tn);
    }
}
static int call(lua_State* L) {
    ty& r = check<ty>(L, 1);
    const int index = luaL_checkinteger(L, 2);
    if (not_in_range(index, r.size())) return lua_err::out_of_range(L, tn);
    lua_pushnumber(L, r[index]);
    return 1;
}
int builtin::class_vector(lua_State* L) {
    if (luaL_newmetatable(L, metatable_name<ty>())) {
        const luaL_Reg meta [] = {
            {mm::add, add},
            {mm::mul, mul},
            {mm::unm, unm},
            {mm::div, div},
            {mm::sub, sub},
            {mm::namecall, namecall},
            {mm::tostring, tostring},
            {mm::call, call},
            {nullptr, nullptr}
        };
        lua_pushstring(L, tn);
        lua_setfield(L, -2, mm::type);
        luaL_register(L, nullptr, meta);
    }
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, tn);
    return 1;
}
