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
declare class Path
    function stem(self): Path
    function empty(self): boolean
    function filename(self): Path
    function has_stem(self): boolean
    function root_path(self): Path
    function parent_path(self): Path
    function is_absolute(self): boolean
    function is_relative(self): boolean
    function extension(self): Path
    function replace_extension(self, s: string)
    function relative_path(self): Path
    function compare(self): number
    function root_name(self): Path
    function root_directory(self): Path
    function has_root_path(self): boolean
    function has_root_name(self): boolean
    function has_root_directory(self): boolean
    function __tostring(self): string
    function __div(self, other: string | Path): Path
end
declare class Directory_entry
    function is_directory(self): boolean
    function is_fifo(self): boolean
    function path(self): Path
    function is_socket(self): boolean
    function is_other(self): boolean
    function is_symlink(self): boolean
    function is_block_file(self): boolean
    function is_regular_file(self): boolean
    function is_character_file(self): boolean
end
declare class Vector2
    x: number
    y: number
    function dot(self, a: Vector2): number
    function unit(self): Vector2
    function magnitude(self): number
    function abs(self): Vector2
    function __add(self, a: Vector2): Vector2
    function __mul(self, scalar: number): Vector2
    function __div(self, scalar: number): Vector2
    function __sub(self, a: Vector2): Vector2
    function __unm(self): Vector2
    function __call(self, index: number): number
end
declare class Color
    r: number
    g: number
    b: number
    a: number
end
declare class Rectangle
    x: number
    y: number
    w: number
    h: number
end
declare class Texture_ptr end
export type render_flip = 'none'|'vertical'|'horizontal'
export type keycode = 'esc'|'f1'|'f2'|'f3'|'f4'|'f5'|'f6'|'f7'|'f8'|'f9'|'f10'|'f11'|'f12'|
    '1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'0'|'minus'|'equals'|'backspace'|
    'q'|'w'|'e'|'r'|'t'|'y'|'u'|'i'|'o'|'p'|'backslash'|
    'a'|'s'|'d'|'f'|'g'|'h'|'j'|'k'|'l'|'enter'|
    'z'|'x'|'c'|'v'|'b'|'n'|'n'|'m'|'comma'|'period'|'slash'|
    'lshift'|'lctrl'|'lalt'|'space'|'ralt'|'left'|'up'|'down'|'right'
export type mouse_button_type = "left" | "right"| "middle"
type matrix33_mul = ((Matrix33, Matrix33)->Matrix33) & ((Matrix33, Vector3)->Vector3) & ((Matrix33, Vector)->Vector)
declare class Matrix33
    function inverse(self): Matrix33
    function transpose(self): Matrix33
    function __call(self, i: number, j: number): number
    __mul: matrix33_mul
    function __add(self, other: Matrix33): Matrix33
    function __sub(self, other: Matrix33): Matrix33
end
declare class Vector3
    x: number
    y: number
    z: number
    function dot(self, a: Vector3): number
    function unit(self): Vector3
    function magnitude(self): number
    function abs(self): Vector3
    function __add(self, a: Vector3): Vector3
    function __mul(self, scalar: number): Vector3
    function __div(self, scalar: number): Vector3
    function __sub(self, a: Vector3): Vector3
    function __unm(self): Vector3
end
declare class Vector
    function dot(self, a: Vector): number
    function unit(self): Vector
    function magnitude(self): number
    function abs(self): Vector
    function at(self, index: number): number
    function set(self, index: number, value: number)
    function reset(self)
    function extend(self, size: number, preserve: boolean?)
    function size(self): number
    function capacity(self): number
    function resize(self, size: number, preserve: boolean?)
    function reserve(self, amount: number)
    function __add(self, a: Vector): Vector
    function __mul(self, scalar: number): Vector
    function __div(self, scalar: number): Vector
    function __sub(self, a: Vector): Vector
    function __unm(self): Vector
end
export type filesystem_module = {
    create_directory: (Path | string) -> boolean,
    exists: (path: Path | string) -> boolean,
    is_character_file: (path: Path | string) -> boolean,
    copy_file: (from: Path | string, to: Path | string, opt: copy_options?) -> boolean,
    rename: (from: Path | string, to: Path | string) -> (),
    remove: (path: Path | string) -> boolean,
    remove_all: (path: Path | string) -> number,
    copy: (from: Path | string, to: Path | string, opt: copy_options?) -> (),
    absolute: (path: Path | string) -> Path,
    children_of: (path: Path | string) -> {Directory_entry},
    descendants_of: (path: Path | string) -> {Directory_entry},
    Path: (file: string) -> Path,
}
export type window_module = {
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
    focus: ()->(),
    on_resize: ((w: number, h: number)->())?,
    on_move: ((x: number, y: number)->())?,
    on_sizechange: ((w: number, h: number)->())?,
}
export type blend_mode = 'none'|'mul'|'add'|'mod'|'blend'|'invalid'
export type rendering_module = {
    render_geometry_raw: (txt: Texture_ptr?, buf: buffer,
    xy_offset: number, xy_stride: number, color_offset: number,
        color_stride: number, uv_offset: number, uv_stride: number,
        num_vertices: number, indices_offset: number, num_indices: number,
        indices_size: number)->(),
    clear: (bg: Color?)->(),
    flush: ()->(),
}
export type drawing_module = {
    set_color: (col: Color)->(),
    set_blend_mode: (mode: blend_mode)->(),
    draw_rectangle: ((rect: Rectangle)->())
        & ((origin: Vector2, size: Vector2)->()),
    draw_rectangles: ((...Rectangle)->())
        & ((buf: buffer_int32)->()),
    draw_point: (point: Vector2)->(),
    draw_points: ((...Vector2)->())
        & ((buf: buffer_int32)->()),
    fill_rectangle: ((rect: Rectangle)->())
        & ((origin: Vector2, size: Vector2)->()),
    fill_rectangles: ((...Rectangle)->()) & ((buf: buffer_int32)->()),
    draw_line: (t0: Vector2, t1: Vector2)->(),
    draw_lines: ((...Vector2)->()) & ((buf: buffer_int32)->()), 
    draw_polygon: ({Vector2})->(),
    fill_polygon: ({Vector2})->(),
    fill_circle: (center: Vector2, radius: number, smooth_alpha_channel: boolean?)->(),
    fill_ellipse: (center: Vector2, radius: Vector2, smooth_alpha_channel: boolean?)->(),
}
export type luau_iterator_library = {
    ireverse: <T>(t:{T})->(()->T?),
    ifilter: <T>(t:{T}, pred: (e: T)->boolean)->(()->T?),
    filter: <T>(t:{[any]: T}, pred: (e: T)->boolean)->(()->T?),
    elements: <T>(t: {[any]: T})->(()->T?),
    imap: <T, U>(t: {T}, u: {U})->(()->(T?, U?)),
}

-- declare class Event
--     connect: <Ts...>(self: Event, (Ts...)->())->number
--     disconnect: (self: Event, id: number)->()
--     fire: <Ts...>(self: Event, Ts...)->()
-- end
declare class event_connection_id
    function __tostring(self): string
end
export type Event<params...> = {
    connect: (self: Event<params...>, (params...)->())->event_connection_id,
    disconnect: (self: Event<params...>, id: event_connection_id)->(),
    fire: (self: Event<params...>, params...)->(),
}
export type engine_module = {
    get_module: (('filesystem') -> filesystem_module)
    & (('drawing')->drawing_module)
    & (('rendering')->rendering_module)
    & (('iterator')->luau_iterator_library),
    updating: Event<number>,
    rendering: Event<>,
    key_pressed: Event<keycode>,
    key_released: Event<keycode>,
    mouse_pressed: Event<mouse_button_type, Vector2>,
    mouse_released: Event<mouse_button_type, Vector2>,
    shutting_down: Event<>,
    window: window_module,
}
export type luau_task_library = {
    spawn: <Ts...>(fn: (Ts...)->(), Ts...)->(),
    wait: (count: number)->(),
}
declare class buffer_float end
declare class buffer_double end
declare class buffer_int8 end
declare class buffer_int16 end
declare class buffer_int32 end
declare class buffer_uint8 end
declare class buffer_uint16 end
declare class buffer_uint32 end
declare task: luau_task_library
declare engine: engine_module
declare Rectangle: (x: number?, y: number?, w: number?, h: number?)->Rectangle
type x_field = {x: number}
type y_field = {y: number}
type z_field = {z: number}
declare Vector2: ((x: number?, y: number?)->Vector2)
        & ((fields: x_field | y_field)->Vector2) 
        & ((copy: Vector2)->Vector2)
declare Vector3: ((x: number?, y: number?, z: number?)->Vector3)
        & ((fields: x_field | y_field | z_field)->Vector3)
        & ((copy: Vector3)->Vector3)
declare Vector: ((...number)->Vector)
        & (({number})->Vector)
declare Matrix33: (...number)->Matrix33
declare Color: (r: number?, g: number?, b: number?, a: number?)->Color
declare Path: (path: string)->Path
declare Event: <params...>()->Event<params...>

type float = 'float'
type double = 'double'
type int8 = 'int8'
type uint8 = 'uint8'
type int16 = 'int16'
type uint16 = 'uint16'
type int32 = 'int32'
type uint32 = 'uint32'
export type fixed_width_buffer_library = {
    create: ((float, size: number)->buffer_float)
        & ((double, size: number)->buffer_double)
        & ((int8, size: number)->buffer_int8)
        & ((uint8, size: number)->buffer_uint8)
        & ((int16, size: number)->buffer_int16)
        & ((uint16, size: number)->buffer_uint16)
        & ((int32, size: number)->buffer_int32)
        & ((uint32, size: number)->buffer_uint32),
    read: ((float, b: buffer_float, idx: number)->number)
        & ((double, b: buffer_double, idx: number)->number)
        & ((int8, b: buffer_int8, idx: number)->number)
        & ((uint8, b: buffer_uint8, idx: number)->number)
        & ((int16, b: buffer_int16, idx: number)->number)
        & ((uint16, b: buffer_uint16, idx: number)->number)
        & ((int32, b: buffer_int32, idx: number)->number)
        & ((uint32, b: buffer_uint32, idx: number)->number),
    write: ((float, b: buffer_float, idx: number, val: number)->number)
        & ((double, b: buffer_double, idx: number, val: number)->number)
        & ((int8, b: buffer_int8, idx: number, val: number)->number)
        & ((uint8, b: buffer_uint8, idx: number, val: number)->number)
        & ((int16, b: buffer_int16, idx: number, val: number)->number)
        & ((uint16, b: buffer_uint16, idx: number, val: number)->number)
        & ((int32, b: buffer_int32, idx: number, val: number)->number)
        & ((uint32, b: buffer_uint32, idx: number, val: number)->number),
    len: ((float, b: buffer_float)->number)
        & ((double, b: buffer_double)->number)
        & ((int8, b: buffer_int8)->number)
        & ((uint8, b: buffer_uint8)->number)
        & ((int16, b: buffer_int16)->number)
        & ((uint16, b: buffer_uint16)->number)
        & ((int32, b: buffer_int32)->number)
        & ((uint32, b: buffer_uint32)->number),
    copy: ((float, target: buffer_float, target_idx: number, source: buffer_float, source_idx: number?, count: number?)->())
        & ((double, target: buffer_double, target_idx: number, source: buffer_double, source_idx: number?, count: number?)->())
        & ((int8, target: buffer_int8, target_idx: number, source: buffer_int8, source_idx: number?, count: number?)->())
        & ((uint8, target: buffer_uint8, target_idx: number, source: buffer_uint8, source_idx: number?, count: number?)->())
        & ((int16, target: buffer_int16, target_idx: number, source: buffer_int16, source_idx: number?, count: number?)->())
        & ((uint16, target: buffer_uint16, target_idx: number, source: buffer_uint16, source_idx: number?, count: number?)->())
        & ((int32, target: buffer_int32, target_idx: number, source: buffer_int32, source_idx: number?, count: number?)->())
        & ((uint32, target: buffer_uint32, target_idx: number, source: buffer_uint32, source_idx: number?, count: number?)->())
}
declare fixed_buffer: fixed_width_buffer_library
