export type map<Key, Elem> = {[Key]: Elem}
export type table<Elem> = {Elem}
export type bool = boolean
declare class vec2d
    x: number
    y: number
    function dot(self, a: vec2d): number
    function unit(self): vec2d
    function magnitude(self): number
    function abs(self): vec2d
    function __add(self, a: vec2d): vec2d
    function __mul(self, scalar: number): vec2d
    function __div(self, scalar: number): vec2d
    function __sub(self, a: vec2d): vec2d
    function __unm(self): vec2d
end
declare function vec2d(x : number?, y: number?): vec2d
declare class recti64
    x: number
    y: number
    width: number
    height: number
end
declare function recti64(x: number?, y: number?, w: number?, h: number?): recti64
declare class coloru32
    red: number
    green: number
    blue: number
    alpha: number
end
declare function coloru32(r: number?, g: number?, b: number?, a: number?): coloru32
declare renderer: {
    fill: (recti64) -> (),
    draw: (recti64) -> (),
}
declare builtin: {
    title: string,
    window_size: vec2d,
    maximize_window: () -> (),
    mouse_pos: () -> vec2d,
    key_down: (key: keycode) -> boolean,
    point_in_rect: (point: vec2d, rect: recti64) -> boolean,
    read_text_file: (path: string) -> string?,
}
declare function collectgarbage(opt: ("collect" | "count")?): ()
declare class physical_component
    position: vec2d
    velocity: vec2d
    acceleration: vec2d
    welded: boolean
    falling: boolean
    obstructed: boolean
    elasticity_coeff: number
    friction_coeff: number
    mass: number
    size: vec2d
    function bounds(self): recti64
end
declare function physical_component(): physical_component
export type mouse_button_type = "left" | "right"| "middle"
export type keycode = 'esc'|'f1'|'f2'|'f3'|'f4'|'f5'|'f6'|'f7'|'f8'|'f9'|'f10'|'f11'|'f12'|
    '1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'0'|'minus'|'equals'|'backspace'|
    'q'|'w'|'e'|'r'|'t'|'y'|'u'|'i'|'o'|'p'|'backslash'|
    'a'|'s'|'d'|'f'|'g'|'h'|'j'|'k'|'l'|'enter'|
    'z'|'x'|'c'|'v'|'b'|'n'|'n'|'m'|'comma'|'period'|'slash'|
    'lshift'|'lctrl'|'lalt'|'space'|'ralt'|'left'|'up'|'down'|'right'
export type fs_copy_options = "recursive"|"copy symlinks"|
    "skip symlinks"|"skip existing"|"update existing"|
    "create symlinks"|"directories only"|"create hard links"|
    "overwrite existing"
export type fs_file_type = 'fifo'|'socket'|'block'|'regular'|
    'symlink'|'unknown'|'junction'|'character'|'directory'|
    'not found'|'none'
export type fs_perms = 'unknown'|'others exec'|'all'|'mask'|
    'set gid'|'set uid'|'group all'|'group exec'|'group read'|
    'owner exec'|'owner read'|'others read'|'others all'|'sticky bit'|
    'others read'|'group write'|'owner write'|'others write'|'none'
declare class fs_path
    function stem(self): fs_path
    function empty(self): boolean
    function filename(self): fs_path
    function has_stem(self): boolean
    function root_path(self): fs_path
    function parent_path(self): fs_path
    function is_absolute(self): boolean
    function is_relative(self): boolean
    function extension(self): fs_path
    function replace_extension(self, s: string)
    function relative_path(self): fs_path
    function compare(self): number
    function root_name(self): fs_path
    function root_directory(self): fs_path
    function has_root_path(self): boolean
    function has_root_name(self): boolean
    function has_root_directory(self): boolean
    function __tostring(self): string
    function __div(self, other: string | fs_path): fs_path
end
declare fs: {
    create_directory: (path: fs_path | string) -> boolean,
    exists: (path: fs_path | string) -> boolean,
    is_character_file: (path: fs_path | string) -> boolean,
    copy_file: (from: fs_path | string, to: fs_path | string, opt: fs_copy_options?) -> boolean,
    rename: (from: fs_path | string, to: fs_path | string) -> (),
    remove: (path: fs_path | string) -> boolean,
    remove_all: (path: fs_path | string) -> number,
    copy: (from: fs_path | string, to: fs_path | string, opt: fs_copy_options?) -> (),
    absolute: (path: fs_path | string) -> fs_path,
    path: (str: string) -> fs_path,
}
