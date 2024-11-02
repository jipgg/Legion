#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "builtin/method_atom.h"
#include "builtin/typedefs.h"
#include <sstream>
#include "common/common.h"
namespace bi = builtin;
using method = bi::method_atom;
using bi::mat3x3_t;
using namespace std::string_literals;
namespace mm = bi::metamethod;
static constexpr auto mat3x3_tname{"M3x3"};

static int ctor(lua_State* L) {
    std::array<std::array<double, 3>, 3> arr;
    for (int i{0}; i < 9; ++i) {
        int row = i / 3;
        int col = i % 3;
        double e = luaL_optnumber(L, i + 1, 0);
        arr[row][col] = e;
    }
    bi::create<mat3x3_t>(L) = mat3x3_t{arr};
    return 1;
}
static int call(lua_State* L) {
    auto& r = bi::check<mat3x3_t>(L, 1);
    const int i = luaL_checkinteger(L, 2);
    const int j = luaL_checkinteger(L, 3);
    lua_pushnumber(L, r.at(i, j));
    return 1;
}
static int mul(lua_State* L) {
    auto& lhs = bi::check<mat3x3_t>(L, 1);
    if (bi::is_type<mat3x3_t>(L, 2)) {
        auto& rhs = bi::check<mat3x3_t>(L, 2);
        bi::create<mat3x3_t>(L) = lhs * rhs;
        return 1;
    } else if (bi::is_type<bi::vec2d_t>(L, 2)) {
        auto& rhs = bi::check<bi::vec2d_t>(L, 2);
        blaze::StaticVector<double, 3> rhs_c = {rhs.at(0), rhs.at(1), 1.0};
        blaze::StaticVector<double, 3> vec{lhs * rhs_c};
        for (int i{}; i < 3; ++i) {
            common::printerr(lhs.at(i, 0), lhs.at(i, 1), lhs.at(i, 2));
        }
        common::print("rhs: ", rhs_c.at(0), rhs_c.at(1), rhs_c.at(2));
        common::print("cpp: ", vec.at(0), vec.at(1), vec.at(2));
        bi::create<bi::vec2d_t>(L) = bi::vec2d_t{vec.at(0), vec.at(1)};
        return 1;
    }
    return 0;
}
static int add(lua_State* L) {
    auto& lhs = bi::check<mat3x3_t>(L, 1);
    auto& rhs = bi::check<mat3x3_t>(L, 2);
    bi::create<mat3x3_t>(L) = lhs + rhs;
    return 1;
}
static int sub(lua_State* L) {
    auto& lhs = bi::check<mat3x3_t>(L, 1);
    auto& rhs = bi::check<mat3x3_t>(L, 2);
    bi::create<mat3x3_t>(L) = lhs - rhs;
    return 1;
}
static int tostring(lua_State* L) {
    auto& r = bi::check<mat3x3_t>(L, 1);
    std::stringstream ss{};
    ss << mat3x3_tname << ": {[";
    ss << r.at(0, 0) << ", " << r.at(0, 1) << ", " << r.at(0, 2) << "][";
    ss << r.at(1, 0) << ", " << r.at(1, 1) << ", " << r.at(1, 2) << "][";
    ss << r.at(2, 0) << ", " << r.at(2, 1) << ", " << r.at(2, 2) << "]}";
    lua_pushlstring(L, ss.str().data(), ss.str().size());
    return 1;
}
static int namecall(lua_State* L) {
    auto& r = bi::check<mat3x3_t>(L, 1);
    int atom;
    lua_namecallatom(L, &atom);
    switch (static_cast<method>(atom)) {
        case method::transpose:
            bi::create<mat3x3_t>(L) = r.transpose();
        return 1;
        case method::inverse: {
            mat3x3_t inv = r;
            blaze::invert3x3<blaze::InversionFlag::asGeneral>(inv);
            bi::create<mat3x3_t>(L) = inv;
        } return 1;
        default:
        break;
    }
    return 0;
}

void bi::mat3x3_init_type(lua_State *L) {
    luaL_newmetatable(L, bi::metatable_name<mat3x3_t>());
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
    lua_pushstring(L, mat3x3_tname);
    lua_setfield(L, -2, mm::type);
    lua_pop(L, 1);
    lua_pushcfunction(L, ctor, mat3x3_tname);
    lua_setglobal(L, mat3x3_tname);
}
