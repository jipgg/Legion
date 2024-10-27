export type map<Key, Elem> = {[Key]: Elem}
export type table<Elem> = {Elem}
declare class Vec2
    x: number
    y: number
    function dot(self, a: Vec2): number
    function unit(self): Vec2
    function magnitude(self): number
    function abs(self): Vec2
    function __add(self, a: Vec2): Vec2
    function __mul(self, scalar: number): Vec2
    function __div(self, scalar: number): Vec2
    function __sub(self, a: Vec2): Vec2
    function __unm(self): Vec2
end
declare function Vec2(x : number?, y: number?): Vec2
declare class Rect
    x: number
    y: number
    width: number
    height: number
end
declare function Rect(x: number?, y: number?, w: number?, h: number?): Rect
declare class Color
    red: number
    green: number
    blue: number
    alpha: number
end
declare function Color(r: number?, g: number?, b: number?, a: number?): Color
declare renderer: {
    fill: (Rect) -> (),
    draw: (Rect) -> (),
}
declare builtin: {
    title: string,
    window_size: Vec2,
    maximize_window: () -> (),
    mouse_pos: () -> Vec2,
    key_down: (key: Keycode) -> boolean,
    point_in_rect: (point: Vec2, rect: Rect) -> boolean,
    read_text_file: (path: string) -> string?,
}
declare function collectgarbage(opt: ("collect" | "count")?): ()
declare class Physical
    position: Vec2
    velocity: Vec2
    acceleration: Vec2
    welded: boolean
    falling: boolean
    obstructed: boolean
    elasticity_coeff: number
    friction_coeff: number
    mass: number
    size: Vec2
    function bounds(self): Rect
end
declare function Physical(): Physical
export type Mousebutton = "Left" | "Right"| "Middle"
export type Keycode = 'esc'|'f1'|'f2'|'f3'|'f4'|'f5'|'f6'|'f7'|'f8'|'f9'|'f10'|'f11'|'f12'|
    '1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'0'|'minus'|'equals'|'backspace'|
    'q'|'w'|'e'|'r'|'t'|'y'|'u'|'i'|'o'|'p'|'backslash'|
    'a'|'s'|'d'|'f'|'g'|'h'|'j'|'k'|'l'|'enter'|
    'z'|'x'|'c'|'v'|'b'|'n'|'n'|'m'|'comma'|'period'|'slash'|
    'lshift'|'lctrl'|'lalt'|'space'|'ralt'|'left'|'up'|'down'|'right'
export type Weak_k = 'k'
export type Weak_v = 'v'
export type Weak_kv = 'kv'
export type copy_options = "recursive" | "copy_symlinks"|
    "skip_symlinks" | "skip_existing" | "update_existing" |
    "create_symlinks" | "directories_only" | "create_hard_links" |
    "overwrite_existing"
declare fs: {
    create_directory: (path: string) -> boolean,
    exists: (path: string) -> boolean,
    is_character_file: (path: string) -> boolean,
    copy_file: (from: string, to: string, opt: copy_options?) -> boolean,
    rename: (from: string, to: string) -> (),
    remove: (path: string) -> boolean,
    remove_all: (path: string) -> number,
    copy: (from: string, to: string, opt: copy_options?) -> (),
    absolute: (path: string) -> string,
}
