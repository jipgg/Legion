--!strict
export type copy_options = "recursive"|"copy symlinks"|
    "skip symlinks"|"skip existing"|"update existing"|
    "create symlinks"|"directories only"|"create hard links"|
    "overwrite existing"
export type file_type = 'fifo'|'socket'|'block'|'regular'|
    'symlink'|'unknown'|'junction'|'character'|'directory'|
    'not found'|'none'
export type perms = 'unknown'|'others exec'|'all'|'mask'|
    'set gid'|'set uid'|'group all'|'group exec'|'group read'|
    'owner exec'|'owner read'|'others read'|'others all'|'sticky bit'|
    'others read'|'group write'|'owner write'|'others write'|'none'
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
export type directory_entry = {
        is_directory:(directory_entry) -> boolean,
    is_fifo:(directory_entry) -> boolean,
    path:(directory_entry) -> path,
    is_socket:(directory_entry) -> boolean,
    is_other:(directory_entry) -> boolean,
    is_symlink:(directory_entry) -> boolean,
    is_block_file:(directory_entry) -> boolean,
    is_regular_file:(directory_entry) -> boolean,
    is_character_file:(directory_entry) -> boolean,
    }
type path_var = path | string
export type filesystem = {
    create_directory: (path: path_var) -> boolean,
    exists: (path: path_var) -> boolean,
    is_character_file: (path: path_var) -> boolean,
    copy_file: (from: path_var, to: path_var, opt: copy_options?) -> boolean,
    rename: (from: path_var, to: path_var) -> (),
    remove: (path: path_var) -> boolean,
    remove_all: (path: path_var) -> number,
    copy: (from: path_var, to: path_var, opt: copy_options?) -> (),
    absolute: (path: path_var) -> path,
    children_of: (path: path_var) -> {directory_entry},
    descendants_of: (path: path_var) -> {directory_entry},
    path: (file: path_var) -> path,
}
return _G.__builtin_import_filesystem()::filesystem
