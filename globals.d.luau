declare function collectgarbage(opt: ("collect" | "count")?): ()
export type CopyOptions = "recursive"|"copy symlinks"|
    "skip symlinks"|"skip existing"|"update existing"|
    "create symlinks"|"directories only"|"create hard links"|
    "overwrite existing"
export type FileType = 'fifo'|'socket'|'block'|'regular'|
    'symlink'|'unknown'|'junction'|'character'|'directory'|
    'not found'|'none'
export type Perms = 'unknown'|'others exec'|'all'|'mask'|
    'set gid'|'set uid'|'group all'|'group exec'|'group read'|
    'owner exec'|'owner read'|'others read'|'others all'|'sticky bit'|
    'others read'|'group write'|'owner write'|'others write'|'none'
declare class Path
    function stem(self): Path
    function empty(self): boolean
    function filename(self): Path
    function hasStem(self): boolean
    function rootPath(self): Path
    function parentPath(self): Path
    function isAbsolute(self): boolean
    function isRelative(self): boolean
    function extension(self): Path
    function replaceExtension(self, s: string)
    function relativePath(self): Path
    function compare(self): number
    function rootName(self): Path
    function rootDirectory(self): Path
    function hasRootPath(self): boolean
    function hasRootName(self): boolean
    function hasRootDirectory(self): boolean
    function __tostring(self): string
    function __div(self, other: string | Path): Path
end
declare function Path(path: string): Path
declare class DirectoryEntry
    function isDirectory(self): boolean
    function isFifo(self): boolean
    function path(self): Path
    function isSocket(self): boolean
    function isOther(self): boolean
    function isSymlink(self): boolean
    function isVlockFile(self): boolean
    function isRegularFile(self): boolean
    function isCharacterFile(self): boolean
end
type _Path = Path | string
export type LibFilesystem = {
    createDirectory: (path: _Path) -> boolean,
    exists: (path: _Path) -> boolean,
    isCharacterFile: (path: _Path) -> boolean,
    copyFile: (from: _Path, to: _Path, opt: CopyOptions?) -> boolean,
    rename: (from: _Path, to: _Path) -> (),
    remove: (path: _Path) -> boolean,
    removeAll: (path: _Path) -> number,
    copy: (from: _Path, to: _Path, opt: CopyOptions?) -> (),
    absolute: (path: _Path) -> Path,
    childrenOf: (path: _Path) -> {DirectoryEntry},
    descendantsOf: (path: _Path) -> {DirectoryEntry},
    path: (file: string) -> Path,
}
declare Filesystem: LibFilesystem

declare class Connection
    function disconnect(self): ()
end
declare class Event
    function destroy(self): ()
    connect: <Params...>(self: Event, (Params...)->()) -> Connection
    fire: <Params...>(self: Event, Params...) -> ()
end
declare function Event(): Event

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
    function __call(self, index: number): number
    function get(self, index: number): number
    function set(self, index: number, value: number)
end
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
    function get(self, index: number): number
    function set(self, index: number, value: number)
end
declare Vec2: ((x: number?, y: number?) -> Vec2) & ((v: Vec2i) -> Vec2)
declare Vec2i: ((x: number?, y: number?) -> Vec2i) & ((v: Vec2) -> Vec2i)
declare class Color
    r: number
    g: number
    b: number
    a: number
end
declare Color: (r: number?, g: number?, b: number?, a: number?) -> Color
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
declare class SDL_Texture end
declare class TTF_Font end
declare class SDL_Vertex end
export type SDL_RenderFlip = 'none'|'vertical'|'horizontal'
declare function SDL_Vertex(color: Color, position: Vec2, textCoord: Vec2?): SDL_Vertex
export type LibSDLRender = {
    setDrawColor: (Color) -> (),
    renderCopy: ((SDL_Texture, dest: Recti?, src: Recti?) -> ())
        & ((SDL_Texture, dest: Recti?, src: Recti?, angle: number?, center: Vec2i?, flip: SDL_RenderFlip?)->()),
    drawRect: ((Recti) ->()) & ((Rect) -> ()),
    drawRects: (...Recti) -> (),
    fillRect: ((Recti) ->()) & ((Rect) -> ()),
    fillRects: (...Recti) -> (),
    drawLine: ((Vec2i, Vec2i) -> ()) & ((Vec2, Vec2) -> ()),
    drawLines: (...Vec2i) -> (),
    drawPoint: (Vec2i) -> (),
    drawPoints: (...Vec2i) -> (),
    renderGeometry: ((...SDL_Vertex) -> ()) & ((SDL_Texture, ...SDL_Vertex) -> ()),
    renderQuad: ((SDL_Vertex, SDL_Vertex, SDL_Vertex, SDL_Vertex) -> ())
        & ((SDL_Texture, SDL_Vertex, SDL_Vertex, SDL_Vertex, SDL_Vertex) -> ()),
}
export type LibSDL = {
    openFont: (file: _Path, ptsize: number) -> TTF_Font?,
    loadImage: (file: _Path) -> (SDL_Texture?, number, number),
    loadText: (font: TTF_Font, text: string, fg_or_black: Color?) -> (SDL_Texture?, number, number),
    Render: LibSDLRender,
}
declare SDL: LibSDL
export type Keycode = 'esc'|'f1'|'f2'|'f3'|'f4'|'f5'|'f6'|'f7'|'f8'|'f9'|'f10'|'f11'|'f12'|
    '1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'0'|'minus'|'equals'|'backspace'|
    'q'|'w'|'e'|'r'|'t'|'y'|'u'|'i'|'o'|'p'|'backslash'|
    'a'|'s'|'d'|'f'|'g'|'h'|'j'|'k'|'l'|'enter'|
    'z'|'x'|'c'|'v'|'b'|'n'|'n'|'m'|'comma'|'period'|'slash'|
    'lshift'|'lctrl'|'lalt'|'space'|'ralt'|'left'|'up'|'down'|'right'
export type MouseButtonType = "left" | "right"| "middle"
export type EventSignal<Params...> = {
            connect: (self: EventSignal<Params...>, (Params...)->()) -> Connection,
}
export type InputModule = {
    onKeyPressed: EventSignal<Keycode>,
    onKeyReleased: EventSignal<Keycode>, 
    onMousePressed: EventSignal<MouseButtonType, Vec2i>,
    onMouseReleased: EventSignal<MouseButtonType, Vec2i>,
    isKeyDown: (Keycode) -> boolean,
    getMousePosition: () -> Vec2i,
}
declare InputModule: InputModule 
export type RunModule = {
    onUpdate: EventSignal<number>,
    onRender: EventSignal<>,
}
declare RunModule: RunModule
declare class PhysicalComponent
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
    --function bounds(self): Rect
end
declare function PhysicalComponent(): PhysicalComponent
type Mat3x3_mul = ((Mat3x3, Mat3x3)->Mat3x3) & ((Mat3x3, Vec3)->Vec3) & ((Mat3x3, Vec)->Vec)
declare class Mat3x3
    function inverse(self): Mat3x3
    function transpose(self): Mat3x3
    function __call(self, i: number, j: number): number
    __mul: Mat3x3_mul
    function __add(self, rhs: Mat3x3): Mat3x3
    function __sub(self, rhs: Mat3x3): Mat3x3
end
declare function Mat3x3(
            e1: number?, e2: number?, e3: number?,
            e4: number?, e5: number?, e6: number?,
            e7: number?, e8: number?, e9: number?): Mat3x3
declare class Vec3
    x: number
    y: number
    z: number
    function get(self, index: number): number
    function set(self, index: number, value: number)
    function dot(self, a: Vec3): number
    function unit(self): Vec3
    function magnitude(self): number
    function abs(self): Vec3
    function __add(self, a: Vec3): Vec3
    function __mul(self, scalar: number): Vec3
    function __div(self, scalar: number): Vec3
    function __sub(self, a: Vec3): Vec3
    function __unm(self): Vec3
end
declare function Vec3(x: number?, y: number?, z: number?): Vec3
declare class Vec
    function dot(self, a: Vec): number
    function unit(self): Vec
    function magnitude(self): number
    function abs(self): Vec
    function get(self, index: number): number
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
    function __call(self, i: number): number
end
declare Vec: ((number, ...number) -> Vec)
    & ((v: Vec2)->Vec)
    & ((v: Vec3)->Vec)
    & ((v: Vec2i)->Vec) 
