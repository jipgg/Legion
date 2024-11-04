#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "builtin/method_atom.h"
#include "builtin/typedefs.h"
#include <sstream>
namespace bi = builtin;
using method = bi::method_atom;
using bi::mat3x3_t;
using namespace std::string_literals;
namespace mm = bi::metamethod;
static constexpr auto m3x3_tname{"Mat3x3"};
static constexpr auto matrix_tname{"Matrix"};
using v2d = bi::vec2d_t;
using v3d = bi::vec3d_t;
using vec = bi::vec_t;
using m3x3 = bi::mat3x3_t;
using m3x2 = bi::mat3x2_t;
using mat = blaze::DynamicMatrix<double>;
using bi::check;
using bi::create;
using bi::is_type;

static int err_invalid_vector_size(lua_State* L, int size, int expected) {
    luaL_error(L, "invalid vector size '%d', expected %d", size, expected);
    return 0;
}

static int m3x3_ctor(lua_State* L) {
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
static int m3x3_call(lua_State* L) {
    auto& r = bi::check<mat3x3_t>(L, 1);
    const int i = luaL_checkinteger(L, 2);
    const int j = luaL_checkinteger(L, 3);
    lua_pushnumber(L, r.at(i, j));
    return 1;
}
static int m3x3_mul(lua_State* L) {
    auto& lhs = bi::check<mat3x3_t>(L, 1);
    if (bi::is_type<mat3x3_t>(L, 2)) {
        auto& rhs = bi::check<mat3x3_t>(L, 2);
        bi::create<mat3x3_t>(L) = lhs * rhs;
        return 1;
    } else if (bi::is_type<bi::vec3d_t>(L, 2)) {
        bi::create<bi::vec3d_t>(L) = lhs * bi::check<bi::vec3d_t>(L, 2);
        return 1;
    } else if (is_type<vec>(L, 2)) {
        auto& v = check<vec>(L, 2);
        if (v.size() != 3) return err_invalid_vector_size(L, v.size(), 3);
        create<vec>(L, lhs * v);
        return 1;
    }
    return 0;
}
static int m3x3_add(lua_State* L) {
    auto& lhs = bi::check<mat3x3_t>(L, 1);
    auto& rhs = bi::check<mat3x3_t>(L, 2);
    bi::create<mat3x3_t>(L) = lhs + rhs;
    return 1;
}
static int m3x3_sub(lua_State* L) {
    auto& lhs = bi::check<mat3x3_t>(L, 1);
    auto& rhs = bi::check<mat3x3_t>(L, 2);
    bi::create<mat3x3_t>(L) = lhs - rhs;
    return 1;
}
static int m3x3_tostring(lua_State* L) {
    auto& r = bi::check<mat3x3_t>(L, 1);
    std::stringstream ss{};
    ss << m3x3_tname << ": {[";
    ss << r.at(0, 0) << ", " << r.at(0, 1) << ", " << r.at(0, 2) << "][";
    ss << r.at(1, 0) << ", " << r.at(1, 1) << ", " << r.at(1, 2) << "][";
    ss << r.at(2, 0) << ", " << r.at(2, 1) << ", " << r.at(2, 2) << "]}";
    lua_pushlstring(L, ss.str().data(), ss.str().size());
    return 1;
}
static int m3x3_namecall(lua_State* L) {
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
void bi::init_matrix_types(lua_State *L) {
    luaL_newmetatable(L, bi::metatable_name<mat3x3_t>());
    const luaL_Reg meta[] = {
        {mm::call, m3x3_call},
        {mm::namecall, m3x3_namecall},
        {mm::add, m3x3_add},
        {mm::sub, m3x3_sub},
        {mm::mul, m3x3_mul},
        {mm::tostring, m3x3_tostring},
        {nullptr, nullptr}
    };
    luaL_register(L, nullptr, meta);
    lua_pushstring(L, m3x3_tname);
    lua_setfield(L, -2, mm::type);
    lua_pop(L, 1);
    lua_pushcfunction(L, m3x3_ctor, m3x3_tname);
    lua_setglobal(L, m3x3_tname);
}
