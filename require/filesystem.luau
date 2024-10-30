--!strict
export type Copy_options = "recursive"|"copy symlinks"|
    "skip symlinks"|"skip existing"|"update existing"|
    "create symlinks"|"directories only"|"create hard links"|
    "overwrite existing"
export type File_type = 'fifo'|'socket'|'block'|'regular'|
    'symlink'|'unknown'|'junction'|'character'|'directory'|
    'not found'|'none'
export type Perms = 'unknown'|'others exec'|'all'|'mask'|
    'set gid'|'set uid'|'group all'|'group exec'|'group read'|
    'owner exec'|'owner read'|'others read'|'others all'|'sticky bit'|
    'others read'|'group write'|'owner write'|'others write'|'none'
--[=[
type _path_metatable = {
    __tostring: (self: path) -> string,
    __div: (self: path, other: string | path) -> path,
    __type: 'fs_path',
}
type _path =  {
    stem:(path) -> path,
    empty:(path) -> boolean,
    filename:(path) -> path,
    has_stem: (path) -> boolean,
    root_path: (path) -> path,
    parent_path: (path) -> path,
    is_absolute: (path) -> boolean,
    is_relative: (path) -> boolean,
    extension: (path) -> path,
    replace_extension: (path, s: string) -> (),
    relative_path: (path) -> path,
    compare: (path) -> number,
    root_name: (path) -> path,
    root_directory: (path) -> path,
    has_root_path: (path) -> boolean,
    has_root_name: (path) -> boolean,
    has_root_directory: (path) -> boolean,
} 
export type path = typeof(setmetatable({}::_path, {}::_path_metatable))
]=]
export type Directory_entry = {
        is_directory:(Directory_entry) -> boolean,
    is_fifo:(Directory_entry) -> boolean,
    path:(Directory_entry) -> Path,
    is_socket:(Directory_entry) -> boolean,
    is_other:(Directory_entry) -> boolean,
    is_symlink:(Directory_entry) -> boolean,
    is_block_file:(Directory_entry) -> boolean,
    is_regular_file:(Directory_entry) -> boolean,
    is_character_file:(Directory_entry) -> boolean,
    }
type _Path = Path | string
type namespace = {
    create_directory: (path: _Path) -> boolean,
    exists: (path: _Path) -> boolean,
    is_character_file: (path: _Path) -> boolean,
    copy_file: (from: _Path, to: _Path, opt: Copy_options?) -> boolean,
    rename: (from: _Path, to: _Path) -> (),
    remove: (path: _Path) -> boolean,
    remove_all: (path: _Path) -> number,
    copy: (from: _Path, to: _Path, opt: Copy_options?) -> (),
    absolute: (path: _Path) -> Path,
    children_of: (path: _Path) -> {Directory_entry},
    descendants_of: (path: _Path) -> {Directory_entry},
    path: (file: string) -> Path,
}
return _G.__builtin_import_filesystem()::namespace
