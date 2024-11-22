declare function collectgarbage(opt: ("collect" | "count")?)
export type PathUnion = string | files_Path
export type files_CopyOptions = "Recursive"|"Copy Symlinks"|
    "Skip Symlinks"|"Skip Existing"|"Update Existing"|
    "Create Symlinks"|"Directories Only"|"Create Hard Links"|
    "Overwrite Existing"
declare class files_Path
    function stem(self): files_Path
    function is_empty(self): boolean
    function file_name(self): files_Path
    function has_stem(self): boolean
    function root_path(self): files_Path
    function parent_path(self): files_Path
    function is_absolute(self): boolean
    function is_relative(self): boolean
    function extension(self): files_Path
    function replace_extension(self, s: string)
    function relative_path(self): files_Path
    function compare(self): number
    function root_name(self): files_Path
    function root_directory(self): files_Path
    function has_root_path(self): boolean
    function has_root_name(self): boolean
    function has_root_directory(self): boolean
    function __tostring(self): string
    function __div(self, other: PathUnion): files_Path
end
declare class files_DirectoryEntry
    function is_directory(self): boolean
    function is_fifo(self): boolean
    function path(self): files_Path
    function is_socket(self): boolean
    function is_other(self): boolean
    function is_symlink(self): boolean
    function is_block_file(self): boolean
    function is_regular_file(self): boolean
    function is_character_file(self): boolean
end
declare class Vec2
    x: number
    y: number
    function dot(self, a: Vec2): number
    function normalized(self): Vec2
    function norm(self): number
    function abs(self): Vec2
    function __add(self, a: Vec2): Vec2
    function __mul(self, scalar: number): Vec2
    function __div(self, scalar: number): Vec2
    function __sub(self, a: Vec2): Vec2
    function __unm(self): Vec2
    function __call(self, index: number): number
end
declare class Color
    red: number
    green: number
    blue: number
    alpha: number
    function inverted(self): Color
    function multiplied(self, src: Color): Color
    function modulated(self, src: Color): Color
    function additive_blend(self, src: Color): Color
    function alpha_blend(self, src: Color): Color
end
declare class Rect
    x: number
    y: number
    width: number
    height: number
end
export type RenderFlip = 'None'|'Vertical'|'Horizontal'
export type KeyCode = 'Escape'|'F1'|'F2'|'F3'|'F4'|'F5'|'F6'|'F7'|'F8'|'F9'|'F10'|'F11'|'F12'|
    '1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'0'|'Minus'|'Equals'|'Backspace'|
    'Q'|'W'|'E'|'R'|'T'|'Y'|'U'|'I'|'O'|'P'|'Backslash'|
    'A'|'S'|'D'|'F'|'G'|'H'|'J'|'K'|'L'|'Enter'|
    'Z'|'X'|'C'|'V'|'B'|'N'|'M'|'Comma'|'Period'|'slash'|
    'Left Shift'|'Left Control'|'Left Alt'|'Space'|'Right Alt'|'Arrow Left'|'Arrow Up'|'Arrow Down'|'Arrow Right'
export type MouseButtonType = "Left" | "Right"| "Middle"
declare class Mat3
    function inverse(self): Mat3
    function transpose(self): Mat3
    __mul: ((Mat3, Mat3)->Mat3)
        & ((Mat3, Vec3)->Vec3)
        & ((Mat3, Vec)->Vec)
    function __add(self, other: Mat3): Mat3
    function __sub(self, other: Mat3): Mat3
end
declare class Vec3
    x: number
    y: number
    z: number
    function dot(self, a: Vec3): number
    function normalized(self): Vec3
    function norm(self): number
    function abs(self): Vec3
    function __add(self, a: Vec3): Vec3
    function __mul(self, scalar: number): Vec3
    function __div(self, scalar: number): Vec3
    function __sub(self, a: Vec3): Vec3
    function __unm(self): Vec3
end
declare class Vec
    function dot(self, a: Vec): number
    function normalized(self): Vec
    function norm(self): number
    function abs(self): Vec
    function at(self, index: number): number
    function set(self, index: number, value: number)
    function reset(self)
    function extend(self, size: number, preserve: boolean?)
    function size(self): number
    function capacity(self): number
    function resize(self, size: number, preserve: boolean?)
    function reserve(self, amount: number)
    function __add(self, a: Vec): Vec
    function __mul(self, scalar: number): Vec
    function __div(self, scalar: number): Vec
    function __sub(self, a: Vec): Vec
    function __unm(self): Vec
end
declare class Font
    pt_size: number
end
export type files = {
    create_directory: (where: PathUnion) -> boolean,
    exists: (path: PathUnion) -> boolean,
    is_character_file: (path: PathUnion) -> boolean,
    copy_file: (from: PathUnion, to: PathUnion, opt: files_CopyOptions?) -> boolean,
    rename: (from: PathUnion, to: PathUnion) -> (),
    remove: (path: PathUnion) -> boolean,
    remove_all: (path: PathUnion) -> number,
    copy: (from: PathUnion, to: PathUnion, opt: files_CopyOptions?) -> (),
    absolute: (path: PathUnion) -> files_Path,
    get_children_of: (path: PathUnion) -> {files_DirectoryEntry},
    get_descendants_of: (path: PathUnion) -> {files_DirectoryEntry},
    exe_path: ()->files_Path,
    current_path: ()->files_Path,
    canonical: (path: files_Path)->files_Path,
    proximate: (path: files_Path, base: files_Path?)->files_Path,
    create_symlink: (to: files_Path, new_symlink: files_Path)->(),
    relative: (path: files_Path, base: files_Path?)->files_Path,
    read_file: (path: PathUnion)->string?,
    write_file: (path: PathUnion, data: string)->boolean,
    Path: (string)->files_Path,
}
export type window = {
    size: Vec2,
    position: Vec2,
    title: string,
    opacity: number,
    borderless: boolean,
    always_on_top: boolean,
    maximize: ()->(),
    minimize: ()->(),
    set_icon: (file: string)->(),
    focus: ()->(),
    focus_gained: EventSignal,
    focus_lost: EventSignal,
    shown: EventSignal,
    hidden: EventSignal,
    resized: GenericEvent<Vec2>,
    mouse_enter: EventSignal,
    mouse_leave: EventSignal,
    position_changed: GenericEvent<Vec2>,
    size_changed: GenericEvent<Vec2>,
    restored: EventSignal,
    closing: EventSignal,
    exposed: EventSignal,
    minimized: EventSignal,
    maximized: EventSignal,
}
export type BlendMode= 'None'|'Multiply'|'Add'|'Modulate'|'Blend'|'Invalid'
type graphics_DrawFunctions = {
        rect: (rect: Rect)->(),
        rects: (...Rect)->(),
        pixel: (point: Vec2)->(),
        pixels: (...Vec2)->(),
        line: (t0: Vec2, t1: Vec2)->(),
        lines: ((...Vec2)->()), 
        string: ((ft: Font, str: string, transform: Mat3?)->())
}
type graphics_FillFunctions = {
    rect: (rect: Rect)->(),
    rects: (...Rect)->(),
    circle: ((center: Vec2, radius: number)->())
        & ((transform: Mat3, radius: number)->()),
    ellipse: (center: Vec2, radius: Vec2)->(),
}
type graphics_RenderFunctions = {
    text: ((ft: Font, str: string, transform: Mat3?)->())
        & ((text: string, transform: Mat3?)->()),
    texture: ((texture: Texture, pos: Vec2?)->())
        & ((texture: Texture, dest: Rect)->())
        & ((texture: Texture, trs: Mat3)->()),
}
export type graphics = {
    color: Color,
    blend_mode: BlendMode,
    viewport: Rect,
    scale: Vec2,
    vsync_enabled: boolean,
    clip_rect: Rect?,
    is_clip_enabled: ()->boolean,
    clear: (bg: Color?)->(),
    draw: graphics_DrawFunctions,
    fill: graphics_FillFunctions,
    render: graphics_RenderFunctions,
}
declare class EventSignal
    function connect(self, fn: ()->()): EventConnectionId
    function disconnect(self, id: EventConnectionId)
end
declare class MouseButtonEventSignal
    connect: (self: MouseButtonEventSignal, (type: MouseButtonType, mouse_pos: Vec2)->())->EventConnectionId
    disconnect: (self: MouseButtonEventSignal, id: EventConnectionId)->EventConnectionId
end
declare class KeyboardEventSignal
    connect: (self: KeyboardEventSignal, (key: KeyCode)->())->EventConnectionId
    disconnect: (self: KeyboardEventSignal, id: EventConnectionId)->()
end
declare class MouseMotionEventSignal
    connect: (self: MouseMotionEventSignal, (new_pos: Vec2, relative_pos: Vec2)->())->EventConnectionId
    disconnect: (self: MouseMotionEventSignal, id: EventConnectionId)->()
end
export type userinput = {
    is_key_down: (key: KeyCode)->boolean,
    key_pressed: KeyboardEventSignal,
    key_released: KeyboardEventSignal,
    mouse_button_pressed: MouseButtonEventSignal,
    mouse_button_released: MouseButtonEventSignal,
    mouse_moved: MouseMotionEventSignal,
    mouse_position: Vec2,
}
declare class EventConnectionId
    function __tostring(self): string
end
declare class Texture
    width: number
    height: number
    blend_mode: BlendMode
    color: Color
end
export type GenericEvent<Ts...> = {
    connect: (self: GenericEvent<Ts...>, (Ts...)->())->EventConnectionId,
    disconnect: (self: GenericEvent<Ts...>, id: EventConnectionId)->(),
    fire: (self: GenericEvent<Ts...>, Ts...)->(),
}
declare class UpdateEventSignal 
    connect: (self: UpdateEventSignal, (delta_sec: number)->())->EventConnectionId
    disconnect: (self: UpdateEventSignal, id: EventConnectionId)->()
end
export type luwaw = {
    get_module: (('graphics')->graphics)
        & (('files')->files)
        & (('window')->window)
        & (('userinput')->userinput),
    during_update: UpdateEventSignal,
    during_render: EventSignal,
    shutting_down: EventSignal,
    before_run: EventSignal,
    after_run: EventSignal,
}
declare core: luwaw
declare Rect: (x: number?, y: number?, w: number?, h: number?)->Rect
declare Vec2: (x: number?, y: number?)->Vec2
declare Vec3: (x: number?, y: number?, z: number?)->Vec3
declare Vec: (...number)->Vec
type Mat3_named_constructors = typeof(setmetatable({
    from_scale = function(scale_factor: Vec2 | number): Mat3 return {} :: any end,
    from_rotation = function(radians: number): Mat3 return {} :: any end,
    from_translation = function(translation: Vec2): Mat3 return {} :: any end,
}, {__call = function(...): Mat3 return {} :: any end}))
declare Mat3: 
        & ((row1: {number}, row2: {number}, row3: {number})->Mat3)
        & ((e11: number?, e12: number?, e13: number?, e21: number?, e22: number?, e23: number?, e31: number?, e32: number?, e33: number?)->Mat3)
        & Mat3_named_constructors 
declare Color: ((r: number?, g: number?, b: number?, a: number?)->Color)
        & ((r: number, g: number, b: number, alpha: number)->Color)
--declare FilePath: (path: string)->FilePath
declare Event: <Params...>()->GenericEvent<Params...>
declare Font: (file: files_Path, pt_size: number)->Font

type Texture_named_constructors = typeof(setmetatable({
    from_string = function(text: string, font: Font?, color: Color?): Texture return {} :: any end,
}, {__call = function(...): Texture return {} :: any end}))
declare Texture: ((file: PathUnion)->Texture) & Texture_named_constructors

