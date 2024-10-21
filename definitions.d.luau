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
declare function V2(x : number?, y: number?): V2
declare class Rect
    x: number
    y: number
    width: number
    height: number
end
declare function Rect(x: number?, y: number?, w: number?, h: number?): Rect
