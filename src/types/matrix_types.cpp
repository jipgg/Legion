#include "builtin.h"
#include "lua_util.h"
#include "lua_atom.h"
#include <sstream>
namespace bi = builtin;
using namespace std::string_literals;
namespace mm = bi::metamethod;
using bi::matrix3;
namespace tn = bi::tname;

static int err_invalid_vector_size(lua_State* L, int size, int expected) {
    luaL_error(L, "invalid vector size '%d', expected %d", size, expected);
    return 0;
}

static int ctor(lua_State* L) {
    std::array<std::array<double, 3>, 3> arr;
    for (int i{0}; i < 9; ++i) {
        int row = i / 3;
        int col = i % 3;
        double e = luaL_optnumber(L, i + 1, 0);
        arr[row][col] = e;
    }
    create<matrix3>(L) = matrix3{arr};
    return 1;
}
static int call(lua_State* L) {
    auto& r = check<matrix3>(L, 1);
    const int i = luaL_checkinteger(L, 2);
    const int j = luaL_checkinteger(L, 3);
    lua_pushnumber(L, r.at(i, j));
    return 1;
}
static int mul(lua_State* L) {
    auto& self = check<matrix3>(L, 1);
    if (is_type<matrix3>(L, 2)) {
        auto& rhs = check<matrix3>(L, 2);
        create<matrix3>(L) = self * rhs;
        return 1;
    } else if (is_type<bi::vector3>(L, 2)) {
        create<bi::vector3>(L) = self * check<bi::vector3>(L, 2);
        return 1;
    } else if (is_type<bi::vector>(L, 2)) {
        auto& v = check<bi::vector>(L, 2);
        if (v.size() != 3) return err_invalid_vector_size(L, v.size(), 3);
        create<bi::vector>(L, self * v);
        return 1;
    }
    return 0;
}
static int add(lua_State* L) {
    auto& lhs = check<matrix3>(L, 1);
    auto& rhs = check<matrix3>(L, 2);
    create<matrix3>(L) = lhs + rhs;
    return 1;
}
static int sub(lua_State* L) {
    auto& lhs = check<matrix3>(L, 1);
    auto& rhs = check<matrix3>(L, 2);
    create<matrix3>(L) = lhs - rhs;
    return 1;
}
static int tostring(lua_State* L) {
    auto& r = check<matrix3>(L, 1);
    std::stringstream ss{};
    ss << tn::matrix3 << ": {[";
    ss << r.at(0, 0) << ", " << r.at(0, 1) << ", " << r.at(0, 2) << "][";
    ss << r.at(1, 0) << ", " << r.at(1, 1) << ", " << r.at(1, 2) << "][";
    ss << r.at(2, 0) << ", " << r.at(2, 1) << ", " << r.at(2, 2) << "]}";
    lua_pushlstring(L, ss.str().data(), ss.str().size());
    return 1;
}
static int namecall(lua_State* L) {
    auto& r = check<matrix3>(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    using la = lua_atom;
    switch (static_cast<la>(atom)) {
        case la::transpose:
            create<matrix3>(L) = r.transpose();
        return 1;
        case la::inverse: {
            matrix3 inv = r;
            blaze::invert3x3<blaze::InversionFlag::asGeneral>(inv);
            create<matrix3>(L) = inv;
        } return 1;
        default:
        break;
    }
    return 0;
}

int builtin::class_matrix3(lua_State *L) {
    if (luaL_newmetatable(L, metatable_name<matrix3>())) {
        const luaL_Reg meta[] = {
            {mm::call, call},
            {mm::namecall, namecall},
            {mm::add, add},
            {mm::sub, sub},
            {mm::mul, mul},
            {mm::tostring, tostring},
            {nullptr, nullptr}
        };
        luaL_register(L, nullptr, meta);
        lua_pushstring(L, tn::matrix3);
        lua_setfield(L, -2, mm::type);
    }
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, tn::matrix3);
    return 1;
}