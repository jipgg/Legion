declare class Vec2d
    x: number
    y: number
    function dot(self, a: Vec2d): number
    function unit(self): Vec2d
    function magnitude(self): number
    function abs(self): Vec2d
    function __add(self, a: Vec2d): Vec2d
    function __mul(self, scalar: number): Vec2d
    function __div(self, scalar: number): Vec2d
    function __sub(self, a: Vec2d): Vec2d
    function __unm(self): Vec2d
end
declare function Vec2d(x : number?, y: number?): Vec2d
declare class Vec2i
    x: number
    y: number
    function dot(self, a: Vec2i): number
    function unit(self): Vec2i
    function magnitude(self): number
    function abs(self): Vec2i
    function __add(self, a: Vec2i): Vec2i
    function __mul(self, scalar: number): Vec2i
    function __div(self, scalar: number): Vec2i
    function __sub(self, a: Vec2i): Vec2i
    function __unm(self): Vec2i
end
declare function Vec2i(x : number?, y: number?): Vec2i
declare class Recti64
    x: number
    y: number
    width: number
    height: number
end
declare function Recti64(x: number?, y: number?, w: number?, h: number?): Recti64
declare class Coloru32
    red: number
    green: number
    blue: number
    alpha: number
end
declare function Coloru32(r_or_all: number?, g: number?, b: number?, a: number?): Coloru32
declare class SDL_Point
    x: number
    y: number
end
declare class SDL_Rect
    x: number
    y: number
    w: number
    h: number
end
declare function SDL_Point(x: number?, y: number?): SDL_Point
declare function SDL_Rect(x: number?, y: number?, w: number?, h: number?): SDL_Rect
declare class SDL_Texture end
declare class SDL_Font end
declare renderer: {
    fill: (Recti64) -> (),
    draw: (Recti64) -> (),
    set_color: (Coloru32) -> (),
    render: (SDL_Texture, Recti64) -> (),
}
declare sdl: {
    load_image: (file: string) -> (SDL_Texture?, number, number),
    open_font: (file: string, pt_size: number) -> SDL_Font?,
    render: {
        set_draw_color: (color: Coloru32) -> (),
        render_copy: (texture: SDL_Texture, dest: SDL_Rect?, src: SDL_Rect?) -> (),
    },
}
declare builtin: {
    title: string,
    window_size: Vec2d,
    maximize_window: () -> (),
    mouse_pos: () -> Vec2d,
    key_down: (key: keycode) -> boolean,
    point_in_rect: (point: Vec2d, rect: Recti64) -> boolean,
    read_text_file: (path: string) -> string?,
}
declare function collectgarbage(opt: ("collect" | "count")?): ()
declare class Physical_component
    position: Vec2d
    velocity: Vec2d
    acceleration: Vec2d
    welded: boolean
    falling: boolean
    obstructed: boolean
    elasticity_coeff: number
    friction_coeff: number
    mass: number
    size: Vec2d
    function bounds(self): Recti64
end
declare function Physical_component(): Physical_component
export type mouse_button_type = "left" | "right"| "middle"
export type keycode = 'esc'|'f1'|'f2'|'f3'|'f4'|'f5'|'f6'|'f7'|'f8'|'f9'|'f10'|'f11'|'f12'|
    '1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'0'|'minus'|'equals'|'backspace'|
    'q'|'w'|'e'|'r'|'t'|'y'|'u'|'i'|'o'|'p'|'backslash'|
    'a'|'s'|'d'|'f'|'g'|'h'|'j'|'k'|'l'|'enter'|
    'z'|'x'|'c'|'v'|'b'|'n'|'n'|'m'|'comma'|'period'|'slash'|
    'lshift'|'lctrl'|'lalt'|'space'|'ralt'|'left'|'up'|'down'|'right'

