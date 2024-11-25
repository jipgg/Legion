declare function collectgarbage(opt: ("collect" | "count")?)
export type PathUnion = string | Files_Path
export type CopyOptions = "Recursive"|"Copy Symlinks"|
    "Skip Symlinks"|"Skip Existing"|"Update Existing"|
    "Create Symlinks"|"Directories Only"|"Create Hard Links"|
    "Overwrite Existing"
declare class Files_Path
    function stem(self): Files_Path
    function is_empty(self): boolean
    function file_name(self): Files_Path
    function has_stem(self): boolean
    function root_path(self): Files_Path
    function parent_path(self): Files_Path
    function is_absolute(self): boolean
    function is_relative(self): boolean
    function extension(self): Files_Path
    function replace_extension(self, s: string)
    function relative_path(self): Files_Path
    function compare(self): number
    function root_name(self): Files_Path
    function root_directory(self): Files_Path
    function has_root_path(self): boolean
    function has_root_name(self): boolean
    function has_root_directory(self): boolean
    function __tostring(self): string
    function __div(self, other: PathUnion): Files_Path
end
declare class Files_DirectoryEntry
    function is_directory(self): boolean
    function is_fifo(self): boolean
    function path(self): Files_Path
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
declare class Recti
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
    ptSize: number
end
export type Files = {
    create_directory: (where: PathUnion) -> boolean,
    exists: (path: PathUnion) -> boolean,
    is_character_file: (path: PathUnion) -> boolean,
    copy_file: (from: PathUnion, to: PathUnion, opt: CopyOptions?) -> boolean,
    rename: (from: PathUnion, to: PathUnion) -> (),
    remove: (path: PathUnion) -> boolean,
    remove_all: (path: PathUnion) -> number,
    copy: (from: PathUnion, to: PathUnion, opt: CopyOptions?) -> (),
    absolute: (path: PathUnion) -> Files_Path,
    get_children_of: (path: PathUnion) -> {Files_DirectoryEntry},
    get_descendants_of: (path: PathUnion) -> {Files_DirectoryEntry},
    exe_path: ()->Files_Path,
    current_path: ()->Files_Path,
    canonical: (path: Files_Path)->Files_Path,
    proximate: (path: Files_Path, base: Files_Path?)->Files_Path,
    create_symlink: (to: Files_Path, new_symlink: Files_Path)->(),
    relative: (path: Files_Path, base: Files_Path?)->Files_Path,
    read_file: (path: PathUnion)->string?,
    write_file: (path: PathUnion, data: string)->boolean,
    Path: (string)->Files_Path,
}
export type Window = {
    size: Vec2,
    position: Vec2,
    title: string,
    opacity: number,
    borderless: boolean,
    alwaysOnTop: boolean,
    maximize: ()->(),
    minimize: ()->(),
    set_icon: (file: string)->(),
    focus: ()->(),
    onFocusGain: Event,
    onFocusLoss: Event,
    onShow: Event,
    onHide: Event,
    onResize: GenericEvent<Vec2>,
    onMouseEnter: Event,
    onMouseLeave: Event,
    onPositionChange: GenericEvent<Vec2>,
    onSizeChange: GenericEvent<Vec2>,
    onRestore: Event,
    onClose: Event,
    onExpose: Event,
    onMinimize: Event,
    onMaximize: Event,
}
export type BlendMode= 'None'|'Multiply'|'Add'|'Modulate'|'Blend'|'Invalid'
type GraphicsModule_DrawFunctions = {
        rect: (rect: Rect)->(),
        rects: (...Rect)->(),
        pixel: (point: Vec2)->(),
        pixels: (...Vec2)->(),
        line: (t0: Vec2, t1: Vec2)->(),
        lines: ((...Vec2)->()), 
        string: ((ft: Font, str: string, transform: Mat3?)->())
}
type GraphicsModule_FillFunctions = {
    rect: (rect: Rect)->(),
    rects: (...Rect)->(),
    circle: ((center: Vec2, radius: number)->())
        & ((transform: Mat3, radius: number)->()),
    ellipse: (center: Vec2, radius: Vec2)->(),
}
type GraphicsModule_RenderFunctions = {
    text: ((ft: Font, str: string, transform: Mat3?)->())
        & ((text: string, transform: Mat3?)->()),
    texture: ((texture: Texture, pos: Vec2?)->())
        & ((texture: Texture, dest: Rect)->())
        & ((texture: Texture, trs: Mat3)->()),
}
export type Graphics = {
    color: Color,
    blendMode: BlendMode,
    viewport: Rect,
    scale: Vec2,
    vsyncOn: boolean,
    clipRect: Rect?,
    is_clip_enabled: ()->boolean,
    clear: (bg: Color?)->(),
    draw: GraphicsModule_DrawFunctions,
    fill: GraphicsModule_FillFunctions,
    render: GraphicsModule_RenderFunctions,
}
declare class Event
    function connect(self, fn: ()->()): EventConnectionId
    function disconnect(self, id: EventConnectionId)
end
declare class MouseButtonEvent extends Event
    connect: (self: MouseButtonEvent, (type: MouseButtonType, mouse_pos: Vec2)->())->EventConnectionId
    disconnect: (self: MouseButtonEvent, id: EventConnectionId)->EventConnectionId
end
declare class KeyboardEvent extends Event
    connect: (self: KeyboardEvent, (key: KeyCode)->())->EventConnectionId
    disconnect: (self: KeyboardEvent, id: EventConnectionId)->()
end
declare class MouseMotionEvent extends Event
    connect: (self: MouseMotionEvent, (new_pos: Vec2, relative_pos: Vec2)->())->EventConnectionId
    disconnect: (self: MouseMotionEvent, id: EventConnectionId)->()
end
export type UserInput = {
    is_key_down: (key: KeyCode)->boolean,
    onKeyDown: KeyboardEvent,
    onkeyUp: KeyboardEvent,
    onMouseDown: MouseButtonEvent,
    onMouseUp: MouseButtonEvent,
    onMouseMotion: MouseMotionEvent,
    mousePosition: Vec2,
}
declare class EventConnectionId
    function __tostring(self): string
end
declare class Texture
    width: number
    height: number
    blendMode: BlendMode
    color: Color
end
export type GenericEvent<Ts...> = {
    connect: (self: GenericEvent<Ts...>, (Ts...)->())->EventConnectionId,
    disconnect: (self: GenericEvent<Ts...>, id: EventConnectionId)->(),
    fire: (self: GenericEvent<Ts...>, Ts...)->(),
}
declare class UpdateEvent 
    connect: (self: UpdateEvent, (delta_sec: number)->())->EventConnectionId
    disconnect: (self: UpdateEvent, id: EventConnectionId)->()
end
export type Luwaw = {
    get_builtin_module: (('Graphics')->Graphics)
        & (('Files')->Files),
    window: Window,
    userinput: UserInput,
    onUpdate: UpdateEvent,
    onRender: Event,
    onQuit: Event,
    beforeRun: Event,
    afterRun: Event,
}
declare waw: Luwaw
declare Rect: (x: number?, y: number?, w: number?, h: number?)->Rect
declare Vec2: (x: number?, y: number?)->Vec2
declare Vec3: (x: number?, y: number?, z: number?)->Vec3
declare Vec: (...number)->Vec
declare Recti: (x: number?, y: number?, w: number?, h: number?)->Recti
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
declare Font: (file: Files_Path, pt_size: number)->Font

type Texture_named_constructors = typeof(setmetatable({
    from_string = function(text: string, font: Font?, color: Color?): Texture return {} :: any end,
}, {__call = function(...): Texture return {} :: any end}))
declare Texture: ((file: PathUnion)->Texture) & Texture_named_constructors

