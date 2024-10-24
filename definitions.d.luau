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

declare class Sizei32
    width: number
    height: number
end
declare function Sizei32(w: number?, h: number?): Sizei32

declare class Vec2f
x: number
    y: number
    function dot(self, a: Vec2f): number
    function unit(self): Vec2f
    function magnitude(self): number
    function abs(self): Vec2f
    function __add(self, a: Vec2f): Vec2f
    function __mul(self, scalar: number): Vec2f
    function __div(self, scalar: number): Vec2f
    function __sub(self, a: Vec2f): Vec2f
    function __unm(self): Vec2f
end
declare function Vec2f(x: number?, y: number?): Vec2f

declare class Coloru32
    red: number
    green: number
    blue: number
    alpha: number
end
declare function Coloru32(r: number?, g: number?, b: number?, a: number?): Coloru32

declare renderer: {
    fill: (Recti64) -> (),
    draw: (Recti64) -> (),
}
declare class Physical
    position: Vec2f
    velocity: Vec2f
    acceleration: Vec2f
    welded: boolean
    falling: boolean
    obstructed: boolean
    elasticity_coeff: number
    friction_coeff: number
    mass: number
    size: Sizei32
    function bounds(self): Recti64
end
declare function Physical(): Physical
export type Connection = {
    disconnect: (self: Connection) -> (),
}
export type Mouse_event_handler = (button: number, pos: Vec2i) -> ()
declare class Clickable
    function on_mouse_click(self, handler: Mouse_event_handler): Connection
    function on_mouse_down(self, handler: Mouse_event_handler): Connection
end
declare function Clickable(region: Recti64): Clickable
