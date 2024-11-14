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
declare class texture_ptr end
export type render_flip = 'none'|'vertical'|'horizontal'
export type keycode = 'esc'|'f1'|'f2'|'f3'|'f4'|'f5'|'f6'|'f7'|'f8'|'f9'|'f10'|'f11'|'f12'|
    '1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'0'|'minus'|'equals'|'backspace'|
    'q'|'w'|'e'|'r'|'t'|'y'|'u'|'i'|'o'|'p'|'backslash'|
    'a'|'s'|'d'|'f'|'g'|'h'|'j'|'k'|'l'|'enter'|
    'z'|'x'|'c'|'v'|'b'|'n'|'n'|'m'|'comma'|'period'|'slash'|
    'lshift'|'lctrl'|'lalt'|'space'|'ralt'|'left'|'up'|'down'|'right'
export type mouse_button_type = "left" | "right"| "middle"
type matrix3_mul = ((matrix3, matrix3)->matrix3) & ((matrix3, vector3)->vector3) & ((matrix3, vector)->vector)
declare class matrix3
    function inverse(self): matrix3
    function transpose(self): matrix3
    function __call(self, i: number, j: number): number
    __mul: matrix3_mul
    function __add(self, other: matrix3): matrix3
    function __sub(self, other: matrix3): matrix3
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
declare class font
    file_path: path
    pt_size: number
end
type constructor<obj> = typeof(setmetatable({}, {__call = function(...): obj return ({} :: unknown) :: obj end}))
export type filesystem_module = {
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
    path: ((path: string)->path) & constructor<path>,
    executable_directory: ()->path,
    current_working_directory: ()->path,
    canonical: (path: path)->path,
    proximate: (path: path, base: path?)->path,
    create_symlink: (to: path, new_symlink: path)->(),
    relative: (path: path, base: path?)->path,
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
    render_geometry_raw: (txt: texture_ptr?, buf: buffer,
    xy_offset: number, xy_stride: number, color_offset: number,
        color_stride: number, uv_offset: number, uv_stride: number,
        num_vertices: number, indices_offset: number, num_indices: number,
        indices_size: number)->(),
    clear: (bg: color?)->(),
    flush: ()->(),
    render_texture: ((txt: texture, pos: vector2?)->())
        & ((txt: texture, dest: rectangle)->())
        & ((txt: texture, trs: matrix3)->()),
}
export type graphics_module = {
    set_color: (col: color)->(),
    set_blend_mode: (mode: blend_mode)->(),
    draw_rectangle: ((rect: rectangle)->())
        & ((origin: vector2, size: vector2)->()),
    draw_rectangles: ((...rectangle)->())
        & ((buf: buffer_int32)->()),
    draw_point: (point: vector2)->(),
    draw_points: ((...vector2)->())
        & ((buf: buffer_int32)->()),
    fill_rectangle: ((rect: rectangle)->())
        & ((origin: vector2, size: vector2)->()),
    fill_rectangles: ((...rectangle)->()) & ((buf: buffer_int32)->()),
    draw_line: (t0: vector2, t1: vector2)->(),
    draw_lines: ((...vector2)->()) & ((buf: buffer_int32)->()), 
    draw_polygon: ({vector2})->(),
    fill_polygon: ({vector2})->(),
    fill_circle: (center: vector2, radius: number, smooth_alpha_channel: boolean?)->(),
    fill_ellipse: (center: vector2, radius: vector2, smooth_alpha_channel: boolean?)->(),
    draw_text: ((font: font, text: string, transform: matrix3?)->())
        & ((text: string, transform: matrix3?)->()),
    clear_canvas: (bg: color?)->(),
    draw_image: ((txt: texture, pos: vector2?)->())
        & ((txt: texture, dest: rectangle)->())
        & ((txt: texture, trs: matrix3)->()),
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
declare class texture
    w: number
    h: number
end
export type event<params...> = {
    connect: (self: event<params...>, (params...)->())->event_connection_id,
    disconnect: (self: event<params...>, id: event_connection_id)->(),
    fire: (self: event<params...>, params...)->(),
}
export type engine_module = {
    get_module: (('graphics')->graphics_module)
    & (('rendering')->rendering_module)
    & (('iterator')->luau_iterator_library),
    updating: event<number>,
    rendering: event<>,
    key_pressed: event<keycode>,
    key_released: event<keycode>,
    mouse_pressed: event<mouse_button_type, vector2>,
    mouse_released: event<mouse_button_type, vector2>,
    shutting_down: event<>,
    window: window_module,
    load_image: (file: path | string)->texture,
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
declare fs: filesystem_module
declare rectangle: ((x: number?, y: number?, w: number?, h: number?)->rectangle) & constructor<rectangle>
type x_field = {x: number}
type y_field = {y: number}
type z_field = {z: number}
declare vector2: ((x: number?, y: number?)->vector2)
        & ((fields: x_field | y_field)->vector2) 
        & ((copy: vector2)->vector2)
        & constructor<vector2>
declare vector3: ((x: number?, y: number?, z: number?)->vector3)
        & ((fields: x_field | y_field | z_field)->vector3)
        & ((copy: vector3)->vector3)
        & constructor<vector3>
declare vector: ((...number)->vector)
        & (({number})->vector)
        & constructor<vector>
type matrix3_constructor = typeof(setmetatable({
    from_scale = function(scale_factor: vector2 | number): matrix3 return {} :: any end,
    from_rotation = function(radians: number): matrix3 return {} :: any end,
    from_position = function(translation: vector2): matrix3 return {} :: any end,
        }, {__call = function(...): matrix3 return {} :: any end}))
declare matrix3: ((e11: number?, e12: number?, e13: number?, e21: number?, e22: number?, e23: number?, e31: number?, e32: number?, e33: number?)->matrix3)
        & ((row1: {number}, row2: {number}, row3: {number}))
        & matrix3_constructor
declare color: ((r: number?, g: number?, b: number?, a: number?)->color)
        & constructor<color>
declare path: ((path: string)->path)
        & constructor<path>
declare event: (<params...>()->event<params...>)
        & constructor<event<...unknown>>
declare font: ((file: path, pt_size: number)->font)
        & constructor<font>

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
