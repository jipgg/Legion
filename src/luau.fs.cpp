#include "luau.defs.h"
#include <filesystem>
namespace sfs = std::filesystem;

static sfs::copy_options to_copy_options(std::string_view str) {
    using copts = sfs::copy_options;
    copts opt;
    if (str == "recursive") opt = copts::recursive;
    else if (str == "copy_symlinks") opt = copts::copy_symlinks;
    else if (str == "skip_symlinks") opt = copts::skip_symlinks;
    else if (str == "skip_existing") opt = copts::skip_existing;
    else if (str == "update_existing") opt = copts::update_existing;
    else if (str == "create_symlinks") opt = copts::create_symlinks;
    else if (str == "directories_only") opt = copts::directories_only;
    else if (str == "create_hard_links") opt = copts::create_hard_links;
    else if (str == "overwrite_existing") opt = copts::overwrite_existing;
    else opt = copts::none;
    return opt;
}
static int create_directory(lua_State* L) {
    sfs::path path = luaL_checkstring(L, 1);
    if (sfs::create_directory(path)) lua_pushboolean(L, true);
    else lua_pushboolean(L, false);
    return 1;
}
static int exists(lua_State* L) {
    sfs::path path = luaL_checkstring(L, 1);
    lua_pushboolean(L, sfs::exists(path));
    return 1;
}
static int is_character_file(lua_State* L) {
    lua_pushboolean(L, sfs::is_character_file(luaL_checkstring(L, 1)));
    return 1;
}
static int copy_file(lua_State* L) {
    sfs::path from = luaL_checkstring(L, 1);
    sfs::path to = luaL_checkstring(L, 2);
    lua_pushboolean(L, sfs::copy_file(from, to, to_copy_options(luaL_optstring(L, 3, "none"))));
    return 1;
}
static int rename(lua_State* L) {
    sfs::rename(luaL_checkstring(L, 1), luaL_checkstring(L, 2));
    return 0;
}
static int remove(lua_State* L) {
    lua_pushboolean(L, sfs::remove(luaL_checkstring(L, 1)));
    return 1;
}
static int remove_all(lua_State* L) {
    lua_pushnumber(L, sfs::remove_all(luaL_checkstring(L, 1)));
    return 1;
}
static int copy(lua_State* L) {
    using copts = sfs::copy_options;
    std::string_view str = luaL_optstring(L, 3, "none");
    sfs::copy(luaL_checkstring(L, 1), luaL_checkstring(L, 2), to_copy_options(str));
    return 0;
}
static int is_directory(lua_State* L) {
    lua_pushboolean(L, sfs::is_directory(luaL_checkstring(L, 1)));
    return 1;
}
static int absolute(lua_State* L) {
    sfs::path path = luaL_checkstring(L, 1);
    lua_pushstring(L, sfs::absolute(path).string().c_str());
    return 1;
}
static int is_empty(lua_State* L) {
    lua_pushboolean(L, sfs::is_empty(luaL_checkstring(L, 1)));
    return 1;
}
static int status(lua_State* L) {
    auto file_status = sfs::status(luaL_checkstring(L, 1));
    using ft = sfs::file_type;
    switch (file_status.type()) {
        case ft::fifo:

    }
}

void luau::fs::init_lib(lua_State *L) {
    const luaL_Reg lib[] = {
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
        {nullptr, nullptr}
    };
    lua_pushvalue(L, LUA_GLOBALSINDEX);
    luaL_register(L, "fs", lib);
    lua_pop(L, 1);
}
