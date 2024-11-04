#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "builtin/method_atom.h"
#include "builtin/typedefs.h"
#include "common/common.h"
#include <lua.h>
#include <lualib.h>
#include <luaconf.h>
#include <luacode.h>
#include <luacodegen.h>
using namespace std::string_literals;
static constexpr auto v3_tname = "Vec3"; 
static constexpr auto v2_tname = "Vec2";
static constexpr auto v2i_tname = "Vec2i";
static constexpr auto dynamic_tname = "Vec";
namespace bi = builtin;
namespace mm = bi::metamethod;
using method = bi::method_atom;
using v2i = bi::vec2i_t;
using v2d = bi::vec2d_t;
using v3d = bi::vec3d_t;
using vec = blaze::DynamicVector<double>;
using bi::create;
using bi::check;
using bi::is_type;
[[nodiscard]] static int err_invalid_member(lua_State* L, const char* tname) {
    constexpr auto err_index_msg = "invalid %s member '%s'.";
    luaL_error(L, err_index_msg, tname, luaL_checkstring(L, 2));
    return 0;
}
[[nodiscard]] static int err_invalid_method(lua_State* L, const char* tname) {
    luaL_error(L, "invalid %s method '%s'.");
    return 0;
}
[[nodiscard]] static int err_out_of_range(lua_State* L, const char* tname) {
    luaL_error(L, "index '%d' is out of range in %s", luaL_checkinteger(L, 2), tname);
    return 0;
}
[[nodiscard]] static int err_invalid_type(lua_State* L) {
    luaL_error(L, "invalid type '%s'.", luaL_typename(L, 2));
    return 0;
}
[[nodiscard]] static inline bool not_in_range(int index, int size, int min = 0) {
    return index >= size or index < 0;
}
static int dynamic_ctor(lua_State *L) {
    if (lua_isuserdata(L, 1)) {
        const int tag = lua_userdatatag(L, 1);
        if (tag == bi::type_tag<v2d>()) {
            create<vec>(L, check<v2d>(L, 1));
            return 1;
        } else if (tag == bi::type_tag<v2i>()) {
            create<vec>(L, check<v2i>(L, 1));
            return 1;
        } else if (tag == bi::type_tag<v3d>()) {
            create<vec>(L, check<v3d>(L, 1));
            return 1;
        }
        return err_invalid_type(L);
    }
    const int top = lua_gettop(L);
    auto& r = create<vec>(L, vec{});
    r.resize(top);
    for (int i{}; i < top; ++i) {
        r[i] = luaL_checknumber(L, i + 1);
    }
    return 1;
}
static int dynamic_add(lua_State* L) {
    create<vec>(L, check<vec>(L, 1) + check<vec>(L, 2));
    return 1;
}
static int dynamic_mul(lua_State *L) {
    double scalar = luaL_checknumber(L, 2);
    create<vec>(L, check<vec>(L, 1) * scalar);
    return 1;
}
static int dynamic_tostring(lua_State *L) {
    const auto& self = check<vec>(L, 1);
    std::stringstream ss{};
    ss << dynamic_tname << ": {";
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
static int dynamic_sub(lua_State* L) {
    create<vec>(L, check<vec>(L, 1) - check<vec>(L, 2));
    return 1;
}
static int dynamic_div(lua_State *L) {
    double scalar = luaL_checknumber(L, 2);
    create<vec>(L, check<vec>(L, 1) / scalar);
    return 1;
}
static int dynamic_unm(lua_State* L) {
    create<vec>(L, -check<vec>(L, 1));
    return 1;
}
inline static int dynamic_namecall_unit(lua_State* L) {
    create<vec>(L, blaze::normalize(check<vec>(L, 1)));
    return 1;
}
inline static int dynamic_namecall_abs(lua_State* L) {
    create<vec>(L, blaze::abs(check<vec>(L, 1)));
    return 0;
}
inline static int dynamic_namecall_magnitude(lua_State* L) {
    auto& r = check<vec>(L, 1);
    lua_pushnumber(L, blaze::length(r));
    return 1;
}
inline static int dynamic_namecall_dot(lua_State* L) {
    const double dot = blaze::dot(check<vec>(L, 1), check<vec>(L, 2));
    lua_pushnumber(L, dot);
    return 1;
}
static int dynamic_namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    vec& r = check<vec>(L, 1);
    switch(static_cast<method>(atom)) {
        case method::dot: return dynamic_namecall_dot(L);
        case method::unit: return dynamic_namecall_unit(L);
        case method::abs: return dynamic_namecall_abs(L);
        case method::magnitude: return dynamic_namecall_magnitude(L);
        case method::set: {
            const int index = luaL_checkinteger(L, 2);
            const double value = luaL_checknumber(L, 3);
            if (not_in_range(index, r.size())) return err_out_of_range(L, dynamic_tname);
            r[index] = value;
            return 0;
        } case method::get: {
            const int index = luaL_checkinteger(L, 2);
            if (not_in_range(index, r.size())) return err_out_of_range(L, dynamic_tname);
            lua_pushnumber(L, r[index]);
            return 1;
        } case method::extend: {
            const int size = luaL_checkinteger(L, 2);
            r.extend(size, luaL_optboolean(L, 3, true));
            return 0;
        } case method::reset: {
            r.reset();
            return 0;
        } case method::size: {
            lua_pushinteger(L, r.size());
            return 1;
        } case method::capacity: {
            lua_pushinteger(L, r.capacity());
            return 1;
        } case method::resize: {
            r.resize(luaL_checkinteger(L, 2), luaL_optboolean(L, 3, true));
            return 0;
        } case method::reserve: {
            r.reserve(luaL_checkinteger(L, 2));
            return 0;
        }
        default: return err_invalid_method(L, dynamic_tname);
    }
}
static int dynamic_call(lua_State* L) {
    vec& r = check<vec>(L, 1);
    const int index = luaL_checkinteger(L, 2);
    if (not_in_range(index, r.size())) return err_out_of_range(L, dynamic_tname);
    lua_pushnumber(L, r[index]);
    return 1;
}
static int v3_ctor(lua_State *L) {
    const double x = luaL_optnumber(L, 1, 0);
    const double y = luaL_optnumber(L, 2, 0);
    const double z = luaL_optnumber(L, 3, 0);
    bi::create<v3d>(L) = {x, y, z};
    return 1;
}
static int v3_add(lua_State* L) {
    create<v3d>(L) = check<v3d>(L, 1) + check<v3d>(L, 2);
    return 1;
}
static int v3_index(lua_State *L) {
    const char index = *luaL_checkstring(L, 2);
    const auto& self = check<v3d>(L, 1);
    switch (index) {
        case 'x': lua_pushnumber(L, self.at(0)); return 1;
        case 'y': lua_pushnumber(L, self.at(1)); return 1;
        case 'z': lua_pushnumber(L, self.at(2)); return 1;
        default: return err_invalid_member(L, v3_tname);
    }
    return err_invalid_member(L, v3_tname);
}
static int v3_newindex(lua_State *L) {
    const double n = luaL_checknumber(L, 3);
    auto& self = check<v3d>(L, 1);
    if (lua_isstring(L, 2)) {
        switch (*luaL_checkstring(L, 2)) {
            case 'x': self.at(0) = n; return 0;
            case 'y': self.at(1) = n; return 0;
            case 'z': self.at(2) = n; return 0;
            default: return err_invalid_member(L, v3_tname);
        }
    } else if (lua_isnumber(L, 2)) {
        const int index = luaL_checkinteger(L, 2);
        if (index >= self.size() or index < 0) return err_out_of_range(L, v3_tname);
        self[index] = n;
        return 0;
    }
    return err_invalid_type(L);
}
static int v3_mul(lua_State *L) {
    assert(lua_isnumber(L, 2));
    double scalar = luaL_checknumber(L, 2);
    create<v3d>(L) = check<v3d>(L, 1) * scalar;
    return 1;
}
static int v3_tostring(lua_State *L) {
    const auto& self = check<v3d>(L, 1);
    double x_v = self.at(0);
    double y_v = self.at(1);
    double z_v = self.at(2);
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    std::string z = std::to_string(z_v);
    if (std::floor(x_v) == x_v) x.erase(x.find('.'));
    if (std::floor(y_v) == y_v) y.erase(y.find('.'));
    if (std::floor(z_v) == z_v) z.erase(z.find('.'));
    std::string str = v3_tname + ": {"s + x + ", " + y + ", " + z + "}";
    lua_pushstring(L, str.c_str());
    return 1;
}
static int v3_sub(lua_State* L) {
    return 0;
}
static int v3_div(lua_State *L) {
    return 0;
}
static int v3_unm(lua_State* L) {
    create<v3d>(L) = -check<v3d>(L, 1);
    return 1;
}
inline static int v3_namecall_unit(lua_State* L) {
    create<v3d>(L) = blaze::normalize(check<v3d>(L, 1));
    return 1;
}
inline static int v3_namecall_abs(lua_State* L) {
    create<v3d>(L) = blaze::abs(check<v3d>(L, 1));
    return 0;
}
inline static int v3_namecall_magnitude(lua_State* L) {
    auto& r = check<v3d>(L, 1);
    lua_pushnumber(L, blaze::length(r));
    return 1;
}
inline static int v3_namecall_dot(lua_State* L) {
    const double dot = blaze::dot(check<v3d>(L, 1), check<v3d>(L, 2));
    lua_pushnumber(L, dot);
    return 1;
}
static int v3_namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    auto& self = check<v3d>(L, 1);
    switch(static_cast<method>(atom)) {
        case method::dot: return v3_namecall_dot(L);
        case method::unit: return v3_namecall_unit(L);
        case method::abs: return v3_namecall_abs(L);
        case method::magnitude: return v3_namecall_magnitude(L);
        case method::set: {
            const int index = luaL_checkinteger(L, 2);
            const double value = luaL_checknumber(L, 3);
            if (not_in_range(index, self.size())) return err_out_of_range(L, dynamic_tname);
            self[index] = value;
            return 0;
        } case method::get: {
            const int index = luaL_checkinteger(L, 2);
            if (not_in_range(index, self.size())) return err_out_of_range(L, dynamic_tname);
            lua_pushnumber(L, self[index]);
            return 1;
        }
        default: luaL_error(L, "invalid method name");
    }
    return 0;
}
static int v2_ctor(lua_State *L) {
    if (lua_isnone(L, 2) and is_type<v2i>(L, 1)) {
        auto& v = check<v2i>(L, 1);
        create<v2d>(L) = {double(v.at(0)), double(v.at(1))};
        return 1;
    }
    double x{}, y{};
    if (lua_isnumber(L, 1)) x = luaL_checknumber(L, 1);
    if (lua_isnumber(L, 2)) y = luaL_checknumber(L, 2);
    create<v2d>(L) = {x, y};
    return 1;
}
static int v2_add(lua_State* L) {
    const auto& self = check<v2d>(L, 1);
    const auto& other = check<v2d>(L, 2);
    create<v2d>(L) = self + other;
    return 1;
}
static int v2_index(lua_State *L) {
    const char index = *luaL_checkstring(L, 2);
    v2d& r = check<v2d>(L, 1);
    if (lua_isstring(L, 1)) {

    }
    switch (index) {
        case 'x': lua_pushnumber(L, r[0]); return 1;
        case 'y': lua_pushnumber(L, r[1]); return 1;
    }
    return 0;
}
static int v2_newindex(lua_State *L) {
    const double n = luaL_checknumber(L, 3);
    v2d& r = check<v2d>(L, 1);
    if (lua_isstring(L, 2)) {
        switch (*luaL_checkstring(L, 2)) {
            case 'x': r[0] = n; return 0;
            case 'y': r[1] = n; return 0;
            default: return err_invalid_member(L, v3_tname);
        }
    } else if (lua_isnumber(L, 2)) {

    }
    return 0;
}
static int v2_mul(lua_State *L) {
    double scalar = luaL_checknumber(L, 2);
    create<v2d>(L) = check<v2d>(L, 1) * scalar;
    return 1;
}
static int v2_tostring(lua_State *L) {
    v2d& r = check<v2d>(L, 1);
    double x_v = r[0];
    double y_v = r[1];
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    if (std::floor(x_v) == x_v) x.erase(x.find('.'));
    if (std::floor(y_v) == y_v) y.erase(y.find('.'));
    std::string str = v2_tname + ": {"s + x + ", " + y + "}";
    lua_pushlstring(L, str.data(), str.size());
    return 1;
}
static int v2_sub(lua_State* L) {
    return 0;
}
static int v2_div(lua_State *L) {
    return 0;
}
static int v2_unm(lua_State* L) {
    create<v2d>(L) = -check<v2d>(L, 1);
    return 1;
}
static int v2_namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    auto& self = bi::check<v2d>(L, 1);
    switch(static_cast<method>(atom)) {
        case method::dot:
            lua_pushnumber(L, blaze::dot(self, check<v2d>(L, 2)));
            return 1;
        case method::unit:
            create<v2d>(L) = self / blaze::length(self);
            return 1;
        case method::abs:
            create<v2d>(L) = blaze::abs(self);
            return 1;
        case method::magnitude:
            lua_pushnumber(L, blaze::length(self));
            return 1;
        case method::set: {
            const int index = luaL_checkinteger(L, 2);
            const double value = luaL_checknumber(L, 3);
            if (not_in_range(index, self.size())) return err_out_of_range(L, dynamic_tname);
            self[index] = value;
            return 0;
        } case method::get: {
            const int index = luaL_checkinteger(L, 2);
            if (not_in_range(index, self.size())) return err_out_of_range(L, dynamic_tname);
            lua_pushnumber(L, self[index]);
            return 1;
        }
        default:
        return 0;
    }
}
static void v3_init_type(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<v3d>());
    const luaL_Reg v3_meta[] = {
        {mm::index, v3_index},
        {mm::add, v3_add},
        {mm::mul, v3_mul},
        {mm::unm, v3_unm},
        {mm::div, v3_div},
        {mm::sub, v3_sub},
        {mm::namecall, v3_namecall},
        {mm::newindex, v3_newindex},
        {mm::tostring, v3_tostring},
        {nullptr, nullptr}
    };
    lua_pushstring(L, v3_tname);
    lua_setfield(L, -2, mm::type);
    luaL_register(L, nullptr, v3_meta);
    lua_pop(L, 1);
    lua_pushcfunction(L, v3_ctor, v3_tname);
    lua_setglobal(L, v3_tname);
}

static int v2i_ctor(lua_State *L) {
    if (lua_isnone(L, 2) and is_type<v2d>(L, 1)) {
        auto& v = check<v2d>(L, 1);
        create<v2i>(L) = {
            static_cast<int>(v[0]),
            static_cast<int>(v[1])
        };
        return 1;
    }
    const int x = luaL_optinteger(L, 1, 0);
    const int y = luaL_optinteger(L, 2, 0);
    create<v2i>(L) = {x, y};
    return 1;
}
static int v2i_add(lua_State* L) {
    create<v2i>(L) = check<v2i>(L, 1) + check<v2i>(L, 2);
    return 1;
    return 0;
}
static int v2i_index(lua_State *L) {
    v2i& r = check<v2i>(L, 1);
    if (lua_isstring(L, 2)) {
        switch (*luaL_checkstring(L, 2)) {
            case 'x': lua_pushnumber(L, r[0]); return 1;
            case 'y': lua_pushnumber(L, r[1]); return 1;
            default: return err_invalid_member(L, v2i_tname);
        }
    } else if (lua_isnumber(L, 2)) {
        const int index = luaL_checkinteger(L, 2);
        if (index >= r.size()) return err_out_of_range(L, v2i_tname);
        lua_pushnumber(L, r[index]);
        return 1;
    }
    return 0;
}
static int v2i_newindex(lua_State *L) {
    v2i& r = bi::check<v2i>(L, 1);
    const double v = luaL_checknumber(L, 3);
    switch (*luaL_checkstring(L, 2)) {
        case 'x': r[0] = v; return 0;
        case 'y': r[1] = v; return 0;
    }
    return 0;
}
static int v2i_mul(lua_State *L) {
    int scalar = luaL_checknumber(L, 2);
    bi::create<v2i>(L) = bi::check<v2i>(L, 1) * scalar;
    return 1;
}
static int v2i_tostring(lua_State *L) {
    v2i& r = bi::check<v2i>(L, 1);
    int x_v = r[0];
    int y_v = r[1];
    std::string x = std::to_string(x_v);
    std::string y = std::to_string(y_v);
    std::string str = v2i_tname + ": {"s + x + ", " + y + "}";
    lua_pushlstring(L, str.data(), str.size());
    return 1;
}
static int v2i_sub(lua_State* L) {
    return 0;
}
static int v2i_div(lua_State *L) {
    return 0;
}
static int v2i_unm(lua_State* L) {
    create<v2i>(L) = -check<v2i>(L, 1);
    return 1;
}
inline static int v2i_namecall_dot(lua_State* L) {
    lua_pushnumber(L, blaze::dot(check<v2i>(L, 1), check<v2i>(L, 2)));
    return 1;
}
inline static int v2i_namecall_unit(lua_State* L) {
    create<v2i>(L) = blaze::normalize(check<v2i>(L, 1));
    return 1;
}
inline static int v2i_namecall_abs(lua_State* L) {
    create<v2i>(L) = blaze::abs(check<v2i>(L, 1));
    return 1;
}
inline static int v2i_namecall_magnitude(lua_State* L) {
    lua_pushinteger(L, blaze::norm(check<v2i>(L, 1)));
    return 1;
}
static int v2i_namecall(lua_State *L) {
    int atom;
    lua_namecallatom(L, &atom);
    auto& self = check<v2i>(L, 1);
    switch(static_cast<method>(atom)) {
        case method::dot: return v2i_namecall_dot(L);
        case method::unit: return v2i_namecall_unit(L);
        case method::abs: return v2i_namecall_abs(L);
        case method::magnitude: v2i_namecall_abs(L);
        case method::set: {
            const int index = luaL_checkinteger(L, 2);
            const double value = luaL_checknumber(L, 3);
            if (not_in_range(index, self.size())) return err_out_of_range(L, dynamic_tname);
            self[index] = value;
            return 0;
        } case method::get: {
            const int index = luaL_checkinteger(L, 2);
            if (not_in_range(index, self.size())) return err_out_of_range(L, dynamic_tname);
            lua_pushnumber(L, self[index]);
            return 1;
        }
        default:
        return 0;
    }
}
static void v2i_init_type(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<v2i>());
    const luaL_Reg metadata [] = {
        {mm::index, v2i_index},
        {mm::add, v2i_add},
        {mm::mul, v2i_mul},
        {mm::unm, v2i_unm},
        {mm::div, v2i_div},
        {mm::sub, v2i_sub},
        {mm::namecall, v2i_namecall},
        {mm::newindex, v2i_newindex},
        {mm::tostring, v2i_tostring},
        {nullptr, nullptr}
    };
    lua_pushstring(L, v2i_tname);
    lua_setfield(L, -2, mm::type);
    luaL_register(L, nullptr, metadata);
    lua_pop(L, 1);
    lua_pushcfunction(L, v2i_ctor, v2i_tname);
    lua_setglobal(L, v2i_tname);
}
static void v2_init_type(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<v2d>());
    const luaL_Reg v2_meta [] = {
        {mm::index, v2_index},
        {mm::add, v2_add},
        {mm::mul, v2_mul},
        {mm::unm, v2_unm},
        {mm::div, v2_div},
        {mm::sub, v2_sub},
        {mm::namecall, v2_namecall},
        {mm::newindex, v2_newindex},
        {mm::tostring, v2_tostring},
        {nullptr, nullptr}
    };
    lua_pushstring(L, v2_tname);
    lua_setfield(L, -2, mm::type);
    luaL_register(L, nullptr, v2_meta);
    lua_pop(L, 1);
    lua_pushcfunction(L, v2_ctor, v2_tname);
    lua_setglobal(L, v2_tname);
}
static void dynamic_init_type(lua_State* L) {
    luaL_newmetatable(L, bi::metatable_name<vec>());
    const luaL_Reg dynamic_meta [] = {
        {mm::add, dynamic_add},
        {mm::mul, dynamic_mul},
        {mm::unm, dynamic_unm},
        {mm::div, dynamic_div},
        {mm::sub, dynamic_sub},
        {mm::namecall, dynamic_namecall},
        {mm::tostring, dynamic_tostring},
        {mm::call, dynamic_call},
        {nullptr, nullptr}
    };
    lua_pushstring(L, dynamic_tname);
    lua_setfield(L, -2, mm::type);
    luaL_register(L, nullptr, dynamic_meta);
    lua_pop(L, 1);
    lua_pushcfunction(L, dynamic_ctor, dynamic_tname);
    lua_setglobal(L, dynamic_tname);
}
void bi::init_vector_types(lua_State* L) {
    v3_init_type(L);
    v2_init_type(L);
    v2i_init_type(L);
    dynamic_init_type(L);
}
