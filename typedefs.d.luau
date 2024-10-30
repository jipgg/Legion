declare class V2
    x: number
    y: number
    function dot(self, a: V2): number
    function unit(self): V2
    function magnitude(self): number
    function abs(self): V2
    function __add(self, a: V2): V2
    function __mul(self, scalar: number): V2
    function __div(self, scalar: number): V2
    function __sub(self, a: V2): V2
    function __unm(self): V2
end
declare class V2i
    x: number
    y: number
    function dot(self, a: V2i): number
    function unit(self): V2i
    function magnitude(self): number
    function abs(self): V2i
    function __add(self, a: V2i): V2i
    function __mul(self, scalar: number): V2i
    function __div(self, scalar: number): V2i
    function __sub(self, a: V2i): V2i
    function __unm(self): V2i
end
declare V2: ((x: number?, y: number?) -> V2) & ((v: V2i) -> V2)
declare V2i: ((x: number?, y: number?) -> V2i) & ((v: V2) -> V2i)
declare class Coloru32
    r: number
    g: number
    b: number
    a: number
end
declare Coloru32: ((value: number) -> Coloru32) & ((r: number?, g: number?, b: number?, a: number?) -> Coloru32)
declare class Rect
    x: number
    y: number
    w: number
    h: number
end
declare class Recti
    x: number
    y: number
    w: number
    h: number
end
declare function Rect(x: number?, y: number?, w: number?, h: number?): Rect
declare function Recti(x: number?, y: number?, w: number?, h: number?): Recti
declare class Texture end
declare class Font end
declare sdl: {
    load_image: (file: string) -> (Texture?, number, number),
    open_font: (file: string, pt_size: number) -> Font?,
    render: {
        set_draw_color: (color: Coloru32) -> (),
        render_copy: (texture: Texture, dest: Recti?, src: Recti?) -> (),
    },
}
declare builtin: {
    title: string,
    window_size: V2,
    maximize_window: () -> (),
    mouse_pos: () -> V2,
    key_down: (key: Keycode) -> boolean,
    read_text_file: (path: string) -> string?,
}
declare function collectgarbage(opt: ("collect" | "count")?): ()
declare class Physical_component
    position: V2
    velocity: V2
    acceleration: V2
    welded: boolean
    falling: boolean
    obstructed: boolean
    elasticity_coeff: number
    friction_coeff: number
    mass: number
    size: V2
    --function bounds(self): Rect
end
declare function Physical_component(): Physical_component

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
declare function Path(path: string): Path
export type Keycode = 'esc'|'f1'|'f2'|'f3'|'f4'|'f5'|'f6'|'f7'|'f8'|'f9'|'f10'|'f11'|'f12'|
    '1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'0'|'minus'|'equals'|'backspace'|
    'q'|'w'|'e'|'r'|'t'|'y'|'u'|'i'|'o'|'p'|'backslash'|
    'a'|'s'|'d'|'f'|'g'|'h'|'j'|'k'|'l'|'enter'|
    'z'|'x'|'c'|'v'|'b'|'n'|'n'|'m'|'comma'|'period'|'slash'|
    'lshift'|'lctrl'|'lalt'|'space'|'ralt'|'left'|'up'|'down'|'right'
