--!strict
export type CopyOptions = "recursive"|"copy symlinks"|
    "skip symlinks"|"skip existing"|"update existing"|
    "create symlinks"|"directories only"|"create hard links"|
    "overwrite existing"
export type FileType = 'fifo'|'socket'|'block'|'regular'|
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
export type DirectoryEntry = {
    isDirectory:(DirectoryEntry) -> boolean,
    isFifo:(DirectoryEntry) -> boolean,
    path:(DirectoryEntry) -> Path,
    isSocket:(DirectoryEntry) -> boolean,
    isOther:(DirectoryEntry) -> boolean,
    isSymlink:(DirectoryEntry) -> boolean,
    isVlockFile:(DirectoryEntry) -> boolean,
    isRegularFile:(DirectoryEntry) -> boolean,
    isCharacterFile:(DirectoryEntry) -> boolean,
    }
type _Path = Path | string
type Module = {
    createDirectory: (path: _Path) -> boolean,
    exists: (path: _Path) -> boolean,
    isCharacterFile: (path: _Path) -> boolean,
    copyFile: (from: _Path, to: _Path, opt: CopyOptions?) -> boolean,
    rename: (from: _Path, to: _Path) -> (),
    remove: (path: _Path) -> boolean,
    removeAll: (path: _Path) -> number,
    copy: (from: _Path, to: _Path, opt: CopyOptions?) -> (),
    absolute: (path: _Path) -> Path,
    childrenOf: (path: _Path) -> {DirectoryEntry},
    descendantsOf: (path: _Path) -> {DirectoryEntry},
    path: (file: string) -> Path,
}
return _G.__builtin_import_filesystem()::Module
