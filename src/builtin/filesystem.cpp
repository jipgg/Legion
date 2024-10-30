#include "builtin/definitions.h"
#include "builtin/utility.h"
#include "builtin/method_atom.h"
#include <filesystem>
namespace sfs = std::filesystem;
static constexpr auto lib_name = "fs";
static constexpr auto path_type_name = "Path";
static constexpr auto directory_entry_type_name = "Directory_entry";

static std::optional<std::string> resolve_type(lua_State* L, int i) {
    if (builtin::is_type<sfs::path>(L, i)) {
        return std::make_optional(builtin::check<sfs::path>(L, i).string());
    } else if (lua_isstring(L, i)) {
        return std::make_optional(luaL_checkstring(L, i));
    } else return std::nullopt;
}

static sfs::copy_options to_copy_options(std::string_view str) {
    using copts = sfs::copy_options;
    copts opt;
    if (str == "recursive") opt = copts::recursive;
    else if (str == "copy symlinks") opt = copts::copy_symlinks;
    else if (str == "skip symlinks") opt = copts::skip_symlinks;
    else if (str == "skip existing") opt = copts::skip_existing;
    else if (str == "update existing") opt = copts::update_existing;
    else if (str == "create symlinks") opt = copts::create_symlinks;
    else if (str == "directories only") opt = copts::directories_only;
    else if (str == "create hard links") opt = copts::create_hard_links;
    else if (str == "overwrite existing") opt = copts::overwrite_existing;
    else opt = copts::none;
    return opt;
}
static sfs::file_type to_file_type(std::string_view str) {
    using ft = sfs::file_type;
    ft t;
    if (str == "none") t = ft::none;
    else if (str == "junction") t = ft::junction;
    else if (str == "fifo") t = ft::fifo;
    else if (str == "block") t = ft::block;
    else if (str == "socket") t = ft::socket;
    return t;
}
static int create_directory(lua_State* L) {
    if (auto path = resolve_type(L, 1)) {
        if (sfs::create_directory(*path)) lua_pushboolean(L, true);
        else lua_pushboolean(L, false);
        return 1;
    } else {
        luaL_error(L, "unsupported type");
        return 0;
    }
}
static int exists(lua_State* L) {
    if (auto path = resolve_type(L, 1)) {
        lua_pushboolean(L, sfs::exists(*path));
        return 1;
    }
    luaL_error(L, "unsupported type");
    return 0;
}
static int is_character_file(lua_State* L) {
    if (auto path = resolve_type(L, 1)) {
        lua_pushboolean(L, sfs::is_character_file(*path));
        return 1;
    }
    luaL_error(L, "unsupported type");
    return 0;
}
static int copy_file(lua_State* L) {
    auto from = resolve_type(L, 1);
    auto to = resolve_type(L, 2);
    if (from and to) {
        lua_pushboolean(L, sfs::copy_file(*from, *to, to_copy_options(luaL_optstring(L, 3, "none"))));
        return 1;
    }
    return 0;
}
static int rename(lua_State* L) {
    auto from = resolve_type(L, 1);
    auto to = resolve_type(L, 2);
    if (from and to) {
        sfs::rename(*from, *to);
    } else {
        common::printerr("not renamed");
    }
    return 0;
}
static int remove(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    lua_pushboolean(L, sfs::remove(*path));
    return 1;
}
static int remove_all(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    lua_pushnumber(L, sfs::remove_all(*path));
    return 1;
}
static int copy(lua_State* L) {
    auto src = resolve_type(L, 1);
    auto dest = resolve_type(L, 2);
    assert(src.has_value() and dest.has_value());
    std::string_view str = luaL_optstring(L, 3, "none");
    sfs::copy(*src, *dest, to_copy_options(str));
    return 0;
}
static int is_directory(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    lua_pushboolean(L, sfs::is_directory(luaL_checkstring(L, 1)));
    return 1;
}
static int absolute(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    builtin::create<sfs::path>(L, sfs::absolute(*path));
    return 1;
}
static int is_empty(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    lua_pushboolean(L, sfs::is_empty(*path));
    return 1;
}
static int children_of(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    lua_newtable(L);
    if (path->empty()) return 1;
    int i{};
    for (auto& entry : sfs::directory_iterator(*path)) {
        ++i;
        lua_pushinteger(L, i);
        builtin::create<sfs::directory_entry>(L, entry);
        lua_settable(L, -3);
    }
    return 1;
}
static int descendants_of(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    lua_newtable(L);
    if (path->empty()) return 1;
    int i{};
    for (auto& entry : sfs::recursive_directory_iterator(*path)) {
        ++i;
        lua_pushinteger(L, i);
        builtin::create<sfs::directory_entry>(L, entry);
        lua_settable(L, -3);
    }
    return 1;
}

static int directory_entry_namecall(lua_State* L) {
    auto& r = builtin::check<sfs::directory_entry>(L, 1);
    int atom{};
    lua_namecallatom(L, &atom);
    using ma = builtin::method_atom;
    switch (static_cast<ma>(atom)) {
        case ma::is_directory:
            lua_pushboolean(L, r.is_directory());
            return 1;
        case ma::is_fifo:
            lua_pushboolean(L, r.is_fifo());
            return 1;
        case ma::path:
            builtin::create<sfs::path>(L, r.path());
            return 1;
        case ma::is_socket:
            lua_pushboolean(L, r.is_socket());
            return 1;
        case ma::is_other:
            lua_pushboolean(L, r.is_other());
            return 1;
        case ma::is_symlink:
            lua_pushboolean(L, r.is_symlink());
            return 1;
        case ma::is_block_file:
            lua_pushboolean(L, r.is_block_file());
            return 1;
        case ma::is_regular_file:
            lua_pushboolean(L, r.is_regular_file());
            return 1;
        case ma::is_character_file:
            lua_pushboolean(L, r.is_character_file());
            return 1;
        default: return 0;
    }
    return 0;
}

static int path_div(lua_State* L) {
    sfs::path lhs;
    sfs::path rhs;
    if (builtin::is_type<sfs::path>(L, 1)) {
        lhs = builtin::check<sfs::path>(L, 1);
    } else if (lua_isstring(L, 1)) {
        lhs = luaL_checkstring(L, 1);
    } else {
        lua_pushstring(L, "invalid lhs type");
        lua_error(L);
        return 0;
    }
    if (builtin::is_type<sfs::path>(L, 2)) {
        rhs = builtin::check<sfs::path>(L, 2);
    } else if (lua_isstring(L, 2)) {
        rhs = luaL_checkstring(L, 2);
    } else {
        lua_pushstring(L, "invalid rhs type");
        lua_error(L);
        return 0;
    }
    builtin::create<sfs::path>(L, lhs / rhs);
    return 1;
}
static int path_namecall(lua_State* L) {
    int atom{};
    lua_namecallatom(L, &atom);
    auto& r = builtin::check<sfs::path>(L, 1);
    using ma = builtin::method_atom;
    switch (static_cast<ma>(atom)) {
        case ma::stem:
            builtin::create<sfs::path>(L, r.stem());
            return 1;
        case ma::empty:
            lua_pushboolean(L, r.empty());
            return 1;
        case ma::filename:
            builtin::create<sfs::path>(L, r.filename());
            return 1;
        case ma::has_stem:
            lua_pushboolean(L, r.has_stem());
            return 1;
        case ma::root_path:
            builtin::create<sfs::path>(L, r.root_path());
            return 1;
        case ma::parent_path:
            builtin::create<sfs::path>(L, r.parent_path());
            return 1;
        case ma::is_absolute:
            lua_pushboolean(L, r.is_absolute());
            return 1;
        case ma::is_relative:
            lua_pushboolean(L, r.is_relative());
            return 1;
        case ma::extension:
            builtin::create<sfs::path>(L, r.extension());
            return 1;
        case ma::has_extension:
            lua_pushboolean(L, r.has_extension());
            return 1;
        case ma::replace_extension:
            r.replace_extension(luaL_checkstring(L, 2));
            return 0;
        case ma::relative_path:
            builtin::create<sfs::path>(L, r.relative_path());
            return 1;
        case ma::has_relative_path:
            lua_pushboolean(L, r.has_relative_path());
            return 1;
        case ma::compare:
            lua_pushinteger(L, r.compare(builtin::check<sfs::path>(L, 2)));
            return 1;
            case ma::root_name:
            builtin::create<sfs::path>(L, r.root_name());
            return 1;
        case ma::root_directory:
            builtin::create<sfs::path>(L, r.root_directory());
            return 1;
        case ma::has_root_path:
            lua_pushboolean(L, r.has_root_path());
            return 1;
        case ma::has_root_name:
            lua_pushboolean(L, r.has_root_name());
            return 1;
        case ma::has_root_directory:
            lua_pushboolean(L, r.has_root_directory());
            return 1;
        default:
            return 0;
    }
}
static int path_tostring(lua_State* L) {
    auto& r = builtin::check<sfs::path>(L, 1);
    lua_pushstring(L, r.string().c_str());
    return 1;
}
static int path_ctor(lua_State* L) {
    builtin::create<sfs::path>(L, luaL_checkstring(L, 1));
    return 1;
}

static const luaL_Reg fs_lib[] = {
    {"create_directory", create_directory},
    {"exists", exists},
    {"is_character_file", is_character_file},
    {"copy_file", copy_file},
    {"rename", rename},
    {"remove", remove},
    {"remove_all", remove_all},
    {"copy", copy},
    {"is_directory", is_directory},
    {"absolute", absolute},
    {"is_empty", is_empty},
    {"children_of", children_of},
    {"descendants_of", descendants_of},
    {"path", path_ctor},
    {nullptr, nullptr}
};
static const luaL_Reg path_metatable[] = {
    {builtin::metamethod::tostring, path_tostring},
    {builtin::metamethod::div, path_div},
    {builtin::metamethod::namecall, path_namecall},
    {nullptr, nullptr}
};
const luaL_Reg directory_entry_metatable[] = {
    {builtin::metamethod::namecall, directory_entry_namecall},
    {nullptr, nullptr}
};
static void init_types(lua_State* L) {
    if (luaL_newmetatable(L, builtin::metatable_name<sfs::path>())) {
        luaL_register(L, nullptr, path_metatable);
        lua_pushstring(L, path_type_name);
        lua_setfield(L, -2, builtin::metamethod::type);
        lua_pop(L, 1);
    }
    if (luaL_newmetatable(L, builtin::metatable_name<sfs::directory_entry>())) {
        luaL_register(L, nullptr, directory_entry_metatable);
        lua_pushstring(L, directory_entry_type_name);
        lua_setfield(L, -2, builtin::metamethod::type);
        lua_pop(L, 1);
    }
}

void builtin::fs_init_lib(lua_State *L) {
    init_types(L);
    lua_pushcfunction(L, path_ctor, path_type_name);
    lua_setglobal(L, path_type_name);
}
int builtin::fs_import_lib(lua_State *L) {
    init_global_types(L);
    lua_newtable(L);
    luaL_register(L, nullptr, fs_lib);
    return 1;
}
