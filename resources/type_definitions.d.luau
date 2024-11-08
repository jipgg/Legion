declare function collectgarbage(opt: ("collect" | "count")?)
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
declare class path
    function stem(self): path
    function empty(self): boolean
    function filename(self): path
    function has_stem(self): boolean
    function root_path(self): path
    function parent_path(self): path
    function is_absolute(self): boolean
    function is_relative(self): boolean
    function extension(self): path
    function replace_extension(self, s: string)
    function relative_path(self): path
    function compare(self): number
    function root_name(self): path
    function root_directory(self): path
    function has_root_path(self): boolean
    function has_root_name(self): boolean
    function has_root_directory(self): boolean
    function __tostring(self): string
    function __div(self, other: string | path): path
end
declare class directory_entry
    function is_directory(self): boolean
    function is_fifo(self): boolean
    function path(self): path
    function is_socket(self): boolean
    function is_other(self): boolean
    function is_symlink(self): boolean
    function is_block_file(self): boolean
    function is_regular_file(self): boolean
    function is_character_file(self): boolean
end
declare class vector2
    x: number
    y: number
    function dot(self, a: vector2): number
    function unit(self): vector2
    function magnitude(self): number
    function abs(self): vector2
    function __add(self, a: vector2): vector2
    function __mul(self, scalar: number): vector2
    function __div(self, scalar: number): vector2
    function __sub(self, a: vector2): vector2
    function __unm(self): vector2
    function __call(self, index: number): number
end
declare class color
    r: number
    g: number
    b: number
    a: number
end
declare class rectangle
    x: number
    y: number
    w: number
    h: number
end
declare class opaque_texture end
declare class opaque_font end
declare class opaque_vertex end
export type render_flip = 'none'|'vertical'|'horizontal'
export type keycode = 'esc'|'f1'|'f2'|'f3'|'f4'|'f5'|'f6'|'f7'|'f8'|'f9'|'f10'|'f11'|'f12'|
    '1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'0'|'minus'|'equals'|'backspace'|
    'q'|'w'|'e'|'r'|'t'|'y'|'u'|'i'|'o'|'p'|'backslash'|
    'a'|'s'|'d'|'f'|'g'|'h'|'j'|'k'|'l'|'enter'|
    'z'|'x'|'c'|'v'|'b'|'n'|'n'|'m'|'comma'|'period'|'slash'|
    'lshift'|'lctrl'|'lalt'|'space'|'ralt'|'left'|'up'|'down'|'right'
export type mouse_button_type = "left" | "right"| "middle"
type matrix33_mul = ((matrix33, matrix33)->matrix33) & ((matrix33, vector3)->vector3) & ((matrix33, vector)->vector)
declare class matrix33
    function inverse(self): matrix33
    function transpose(self): matrix33
    function __call(self, i: number, j: number): number
    __mul: matrix33_mul
    function __add(self, other: matrix33): matrix33
    function __sub(self, other: matrix33): matrix33
end
declare class vector3
    x: number
    y: number
    z: number
    function dot(self, a: vector3): number
    function unit(self): vector3
    function magnitude(self): number
    function abs(self): vector3
    function __add(self, a: vector3): vector3
    function __mul(self, scalar: number): vector3
    function __div(self, scalar: number): vector3
    function __sub(self, a: vector3): vector3
    function __unm(self): vector3
end
declare class vector
    function dot(self, a: vector): number
    function unit(self): vector
    function magnitude(self): number
    function abs(self): vector
    function at(self, index: number): number
    function set(self, index: number, value: number)
    function reset(self)
    function extend(self, size: number, preserve: boolean?)
    function size(self): number
    function capacity(self): number
    function resize(self, size: number, preserve: boolean?)
    function reserve(self, amount: number)
    function __add(self, a: vector): vector
    function __mul(self, scalar: number): vector
    function __div(self, scalar: number): vector
    function __sub(self, a: vector): vector
    function __unm(self): vector
end
export type filesystem = {
    create_directory: (path | string) -> boolean,
    exists: (path: path | string) -> boolean,
    is_character_file: (path: path | string) -> boolean,
    copy_file: (from: path | string, to: path | string, opt: copy_options?) -> boolean,
    rename: (from: path | string, to: path | string) -> (),
    remove: (path: path | string) -> boolean,
    remove_all: (path: path | string) -> number,
    copy: (from: path | string, to: path | string, opt: copy_options?) -> (),
    absolute: (path: path | string) -> path,
    children_of: (path: path | string) -> {directory_entry},
    descendants_of: (path: path | string) -> {directory_entry},
    Path: (file: string) -> path,
}
export type window = {
    size: ()->(number, number),
    set_size: (w: number, h: number)->(),
    maximize: ()->(),
    minimize: ()->(),
    title: ()->string,
    set_title: (string)->(),
    set_always_on_top: (boolean)->(),
    set_borderless: (boolean)->(),
    position: ()->(number, number),
    set_position: (x: number, y: number)->(),
    set_opacity: (opacity: number)->(),
    opacity: ()->number,
    set_icon: (file: string)->(),
    size_changed: ((width: number, height: number)->())?,
    moved: ((x: number, y: number)->())?,
    focus: ()->(),
}
export type blend_mode = 'none'|'mul'|'add'|'mod'|'blend'|'invalid'
export type rendering = {
    render_geometry_raw: (txt: opaque_texture?, buf: buffer,
    xy_offset: number, xy_stride: number, color_offset: number,
        color_stride: number, uv_offset: number, uv_stride: number,
        num_vertices: number, indices_offset: number, num_indices: number,
        indices_size: number)->(),
    clear: (bg: color?)->(),
    flush: ()->(),
}
export type drawing = {
    set_color: (col: color)->(),
    set_blend_mode: (mode: blend_mode)->(),
    draw_rectangle: (rect: rectangle)->(),
    draw_rectangles: ((...rectangle)->()) & ((buf: buffer)->()),
    draw_point: (point: vector2)->(),
    draw_points: ((...vector2)->()) & ((buf: buffer)->()),
    fill_rectangle: (rect: rectangle)->(),
    fill_rectangles: ((...rectangle)->()) & ((buf: buffer)->()),
    draw_line: (t0: vector2, t1: vector2)->(),
    draw_lines: ((...vector2)->()) & ((buf: buffer)->()), 
    draw_polygon: ({vector2})->(),
    fill_polygon: ({vector2})->(),
}
export type keyboard_callback = ((key: keycode)->())?
export type mouse_callback = ((button: mouse_button_type, pos: vector2)->())?
export type void_callback = (()->())?
export type update_callback = ((deltasec: number)->())?
export type builtin = {
    load_module: (('filesystem') -> filesystem)
    & (('window')->window)
    & (('drawing')->drawing)
    & (('rendering')->rendering),
    key_pressed: keyboard_callback,
    key_released: keyboard_callback,
    mouse_pressed: mouse_callback,
    mouse_released: mouse_callback,
    updating: update_callback,
    rendering: void_callback,
    shutting_down: void_callback,
}
declare builtin: builtin
declare Rectangle: (x: number?, y: number?, w: number?, h: number?)->rectangle
declare Vector2: (x: number?, y: number?)->vector2
declare Vector3: (x: number?, y: number?, z: number?)->vector3
declare Vector: (...number)->vector
declare Matrix33: (...number)->matrix33
declare Color: (r: number?, g: number?, b: number?, a: number?)->color
