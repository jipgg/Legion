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
declare class SDL_Texture end
declare class TTF_Font end
export type SDL_RenderFlip = 'none'|'vertical'|'horizontal'
export type LibSDLRender = {
    setDrawColor: (Coloru32) -> (),
    renderCopy: ((SDL_Texture, dest: Recti?, src: Recti?) -> ())
        & ((SDL_Texture, dest: Recti?, src: Recti?, angle: number?, center: V2i?, flip: SDL_RenderFlip?)->()),
    drawRect: ((Recti) ->()) & ((Rect) -> ()),
    drawRects: (...Recti) -> (),
    fillRect: ((Recti) ->()) & ((Rect) -> ()),
    fillRects: (...Recti) -> (),
    drawLine: ((V2i, V2i) -> ()) & ((V2, V2) -> ()),
    drawLines: (...V2i) -> (),
    drawPoint: (V2i) -> (),
    drawPoints: (...V2i) -> (),
}
export type LibSDL = {
    openFont: (file: _Path, ptsize: number) -> TTF_Font?,
    loadImage: (file: _Path) -> (SDL_Texture?, number, number),
    loadText: (font: TTF_Font, text: string, fg_or_black: Coloru32?) -> (SDL_Texture?, number, number),
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
    onMousePressed: EventSignal<MouseButtonType, V2i>,
    onMouseReleased: EventSignal<MouseButtonType, V2i>,
    isKeyDown: (Keycode) -> boolean,
    getMousePosition: () -> V2i,
}
declare InputModule: InputModule 
export type RunModule = {
    onUpdate: EventSignal<number>,
    onRender: EventSignal<>,
}
declare RunModule: RunModule
declare class PhysicalComponent
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
declare function PhysicalComponent(): PhysicalComponent

