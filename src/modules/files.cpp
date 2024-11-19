#include "builtin.h"
#include "lua_util.h"
#include "lua_atom.h"
#include <filesystem>
#include "util.h"
namespace fs = std::filesystem;
using path = fs::path;
using directory_entry = fs::directory_entry;

static std::optional<std::string> resolve_type(lua_State* L, int i) {
    if (is_type<fs::path>(L, i)) {
        return std::make_optional(check<path>(L, i).string());
    } else if (lua_isstring(L, i)) {
        return std::make_optional(luaL_checkstring(L, i));
    } else return std::nullopt;
}
static fs::copy_options to_copy_options(std::string_view str) {
    using copts = fs::copy_options;
    copts opt;
    if (str == "Recursive") opt = copts::recursive;
    else if (str == "Copy Symlinks") opt = copts::copy_symlinks;
    else if (str == "Skip Symlinks") opt = copts::skip_symlinks;
    else if (str == "Skip Existing") opt = copts::skip_existing;
    else if (str == "Update Existing") opt = copts::update_existing;
    else if (str == "Create Symlinks") opt = copts::create_symlinks;
    else if (str == "Directories Only") opt = copts::directories_only;
    else if (str == "Create Hard Links") opt = copts::create_hard_links;
    else if (str == "Overwrite Existing") opt = copts::overwrite_existing;
    else opt = copts::none;
    return opt;
}
static fs::file_type to_file_type(std::string_view str) {
    using ft = fs::file_type;
    ft t;
    if (str == "None") t = ft::none;
    else if (str == "Junction") t = ft::junction;
    else if (str == "Fifo") t = ft::fifo;
    else if (str == "Block") t = ft::block;
    else if (str == "Socket") t = ft::socket;
    return t;
}
static int create_directory(lua_State* L) {
    if (auto path = resolve_type(L, 1)) {
        if (fs::create_directory(*path)) lua_pushboolean(L, true);
        else lua_pushboolean(L, false);
        return 1;
    } else {
        luaL_error(L, "unsupported type");
        return 0;
    }
}
static int exists(lua_State* L) {
    if (auto path = resolve_type(L, 1)) {
        lua_pushboolean(L, fs::exists(*path));
        return 1;
    }
    luaL_error(L, "unsupported type");
    return 0;
}
static int is_character_file(lua_State* L) {
    if (auto path = resolve_type(L, 1)) {
        lua_pushboolean(L, fs::is_character_file(*path));
        return 1;
    }
    luaL_error(L, "unsupported type");
    return 0;
}
static int read_text_file(lua_State* L) {
    auto path = resolve_path_type(L, 1);
    if (not path) return 0;
    auto contents = read_file(*path);
    if (not contents) return 0;
    lua_pushlstring(L, contents->data(), contents->size());
    return 1;
}
static int write_text_file(lua_State* L) {
    lua_pushboolean(L, true);
    return 1;
}
static int copy_file(lua_State* L) {
    auto from = resolve_type(L, 1);
    auto to = resolve_type(L, 2);
    if (from and to) {
        lua_pushboolean(L, fs::copy_file(*from, *to, to_copy_options(luaL_optstring(L, 3, "none"))));
        return 1;
    }
    return 0;
}
static int rename(lua_State* L) {
    auto from = resolve_type(L, 1);
    auto to = resolve_type(L, 2);
    if (from and to) {
        fs::rename(*from, *to);
    } else {
        printerr("not renamed");
    }
    return 0;
}
static int remove(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    lua_pushboolean(L, fs::remove(*path));
    return 1;
}
static int remove_all(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    lua_pushnumber(L, fs::remove_all(*path));
    return 1;
}
static int copy(lua_State* L) {
    auto src = resolve_type(L, 1);
    auto dest = resolve_type(L, 2);
    assert(src.has_value() and dest.has_value());
    std::string_view str = luaL_optstring(L, 3, "none");
    fs::copy(*src, *dest, to_copy_options(str));
    return 0;
}
static int is_directory(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    lua_pushboolean(L, fs::is_directory(luaL_checkstring(L, 1)));
    return 1;
}
static int absolute(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    create<fs::path>(L, fs::absolute(*path));
    return 1;
}
static int is_empty(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    lua_pushboolean(L, fs::is_empty(*path));
    return 1;
}
static int children_of(lua_State* L) {
    auto path = resolve_type(L, 1);
    assert(path.has_value());
    lua_newtable(L);
    if (path->empty()) return 1;
    int i{};
    for (auto& entry : fs::directory_iterator(*path)) {
        ++i;
        lua_pushinteger(L, i);
        create<fs::directory_entry>(L, entry);
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
    for (auto& entry : fs::recursive_directory_iterator(*path)) {
        ++i;
        lua_pushinteger(L, i);
        create<fs::directory_entry>(L, entry);
        lua_settable(L, -3);
    }
    return 1;
}

static int directory_entry_namecall(lua_State* L) {
    auto& r = check<fs::directory_entry>(L, 1);
    int atom{};
    lua_namecallatom(L, &atom);
    using la = lua_atom;
    switch (static_cast<la>(atom)) {
        case la::IsDirectory:
            lua_pushboolean(L, r.is_directory());
            return 1;
        case la::IsFifo:
            lua_pushboolean(L, r.is_fifo());
            return 1;
        case la::FilePath:
            create<fs::path>(L, r.path());
            return 1;
        case la::IsSocket:
            lua_pushboolean(L, r.is_socket());
            return 1;
        case la::IsOther:
            lua_pushboolean(L, r.is_other());
            return 1;
        case la::IsSymlink:
            lua_pushboolean(L, r.is_symlink());
            return 1;
        case la::IsBlockFile:
            lua_pushboolean(L, r.is_block_file());
            return 1;
        case la::IsRegularFile:
            lua_pushboolean(L, r.is_regular_file());
            return 1;
        case la::IsCharacterFile:
            lua_pushboolean(L, r.is_character_file());
            return 1;
        default: return 0;
    }
    return 0;
}

static int path_div(lua_State* L) {
    path lhs;
    path rhs;
    if (is_type<path>(L, 1)) {
        lhs = check<path>(L, 1);
    } else if (lua_isstring(L, 1)) {
        lhs = luaL_checkstring(L, 1);
    } else {
        lua_pushstring(L, "invalid lhs type");
        lua_error(L);
        return 0;
    }
    if (is_type<path>(L, 2)) {
        rhs = check<path>(L, 2);
    } else if (lua_isstring(L, 2)) {
        rhs = luaL_checkstring(L, 2);
    } else {
        lua_pushstring(L, "invalid rhs type");
        lua_error(L);
        return 0;
    }
    create<path>(L, lhs / rhs);
    return 1;
}
static int path_namecall(lua_State* L) {
    int atom{};
    lua_namecallatom(L, &atom);
    auto& r = check<path>(L, 1);
    using la = lua_atom;
    switch (static_cast<la>(atom)) {
        case la::Stem:
            create<path>(L, r.stem());
            return 1;
        case la::IsEmpty:
            lua_pushboolean(L, r.empty());
            return 1;
        case la::FileName:
            create<path>(L, r.filename());
            return 1;
        case la::HasStem:
            lua_pushboolean(L, r.has_stem());
            return 1;
        case la::RootPath:
            create<path>(L, r.root_path());
            return 1;
        case la::ParentPath:
            create<path>(L, r.parent_path());
            return 1;
        case la::IsAbsolute:
            lua_pushboolean(L, r.is_absolute());
            return 1;
        case la::IsRelative:
            lua_pushboolean(L, r.is_relative());
            return 1;
        case la::Extension:
            create<path>(L, r.extension());
            return 1;
        case la::HasExtension:
            lua_pushboolean(L, r.has_extension());
            return 1;
        case la::ReplaceExtension:
            r.replace_extension(luaL_checkstring(L, 2));
            return 0;
        case la::RelativePath:
            create<path>(L, r.relative_path());
            return 1;
        case la::HasRelativePath:
            lua_pushboolean(L, r.has_relative_path());
            return 1;
        case la::Compare:
            lua_pushinteger(L, r.compare(check<path>(L, 2)));
            return 1;
        case la::rootName:
            create<path>(L, r.root_name());
            return 1;
        case la::RootDirectory:
            create<path>(L, r.root_directory());
            return 1;
        case la::HasRootPath:
            lua_pushboolean(L, r.has_root_path());
            return 1;
        case la::HasRootName:
            lua_pushboolean(L, r.has_root_name());
            return 1;
        case la::HasRootDirectory:
            lua_pushboolean(L, r.has_root_directory());
            return 1;
        default:
            return 0;
    }
}
static int path_tostring(lua_State* L) {
    auto& r = check<path>(L, 1);
    lua_pushstring(L, r.string().c_str());
    return 1;
}
static int path_ctor(lua_State* L) {
    create<path>(L, luaL_checkstring(L, 1));
    return 1;
}
static int executable_directory(lua_State* L) {
    create<path>(L, util::get_executable_path());
    return 1;
}
static int current_working_directory(lua_State* L) {
    create<path>(L, fs::current_path());
    return 1;
}
static int canonical(lua_State* L) {
    create<path>(L, fs::canonical(*resolve_type(L, 1)));
    return 1;
}
static int proximate(lua_State* L) {
    auto base_opt = resolve_type(L, 2);
    create<path>(L, fs::proximate(*resolve_type(L, 1),
        base_opt ? *base_opt : fs::current_path()));

    return 1;
}
static int create_symlink(lua_State* L) {
    fs::create_symlink(*resolve_type(L, 1), *resolve_type(L, 2));
    return 0;
}
static int relative(lua_State* L) {
    auto base_opt = resolve_type(L, 2);
    create<path>(L, fs::relative(
                 *resolve_type(L, 1),
                 base_opt? *base_opt : fs::current_path()));
    return 1;
}

static const luaL_Reg fs_lib[] = {
    {"CreateDirectory", create_directory},
    {"Exists", exists},
    {"IsCharacterFile", is_character_file},
    {"CopyFile", copy_file},
    {"Rename", rename},
    {"Remove", remove},
    {"RemoveAll", remove_all},
    {"Copy", copy},
    {"IsDirectory", is_directory},
    {"Absolute", absolute},
    {"IsEmpty", is_empty},
    {"GetChildrenOf", children_of},
    {"GetDescendantsOf", descendants_of},
    {"ExecutablePath", executable_directory},
    {"CurrentPath", current_working_directory},
    {"Canonical", canonical},
    {"Proximate", proximate},
    {"CreateSymlink", create_symlink},
    {"FilePath", path_ctor},
    {"Relative", relative},
    {"ReadFile", read_text_file},
    {"WriteFile", write_text_file},
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
    if (luaL_newmetatable(L, metatable_name<path>())) {
        luaL_register(L, nullptr, path_metatable);
        lua_pushstring(L, builtin::tname::path);
        lua_setfield(L, -2, builtin::metamethod::type);
        lua_pop(L, 1);
    }
    if (luaL_newmetatable(L, metatable_name<directory_entry>())) {
        luaL_register(L, nullptr, directory_entry_metatable);
        lua_pushstring(L, builtin::tname::path);
        lua_setfield(L, -2, builtin::metamethod::type);
        lua_pop(L, 1);
    }
}
namespace builtin {
void register_path_type(lua_State *L) {
    if (luaL_newmetatable(L, metatable_name<path>())) {
        luaL_register(L, nullptr, path_metatable);
        lua_pushstring(L, builtin::tname::path);
        lua_setfield(L, -2, builtin::metamethod::type);
        lua_pop(L, 1);
    }
    lua_pushcfunction(L, path_ctor, builtin::tname::path);
    lua_setglobal(L, "FilePath");
}
int files_module(lua_State *L) {
    init_types(L);
    lua_newtable(L);
    luaL_register(L, nullptr, fs_lib);
    return 1;
}
}
