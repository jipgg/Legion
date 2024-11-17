declare function collectgarbage(opt: ("collect" | "count")?)
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
declare class FilePath
    function Stem(self): FilePath
    function IsEmpty(self): boolean
    function FileName(self): FilePath
    function HasStem(self): boolean
    function RootPath(self): FilePath
    function ParentPath(self): FilePath
    function IsAbsolute(self): boolean
    function IsRelative(self): boolean
    function Extension(self): FilePath
    function ReplaceExtension(self, s: string)
    function RelativePath(self): FilePath
    function Compare(self): number
    function RootName(self): FilePath
    function RootDirectory(self): FilePath
    function HasRootPath(self): boolean
    function HasRootName(self): boolean
    function HasRootDirectory(self): boolean
    function __tostring(self): string
    function __div(self, other: string | FilePath): FilePath
end
declare class DirectoryEntry
    function IsDirectory(self): boolean
    function IsFifo(self): boolean
    function Path(self): FilePath
    function IsSocket(self): boolean
    function IsOther(self): boolean
    function IsSymlink(self): boolean
    function IsBlockFile(self): boolean
    function IsRegularFile(self): boolean
    function IsCharacterFile(self): boolean
end
declare class Vector2
    X: number
    Y: number
    function DotProduct(self, a: Vector2): number
    function ToUnitVector(self): Vector2
    function Length(self): number
    function Abs(self): Vector2
    function __add(self, a: Vector2): Vector2
    function __mul(self, scalar: number): Vector2
    function __div(self, scalar: number): Vector2
    function __sub(self, a: Vector2): Vector2
    function __unm(self): Vector2
    function __call(self, index: number): number
end
declare class Color
    Red: number
    Green: number
    Blue: number
    Alpha: number
    function Invert(self): Color
    function Multiply(self, src: Color): Color
    function Modulate(self, src: Color): Color
    function AdditiveBlend(self, src: Color): Color
    function AlphaBlend(self, src: Color): Color
end
declare class Rectangle
    X: number
    Y: number
    Width: number
    Height: number
end
declare class TexturePtr end
export type RenderFlip = 'none'|'vertical'|'horizontal'
export type KeyCode = 'esc'|'f1'|'f2'|'f3'|'f4'|'f5'|'f6'|'f7'|'f8'|'f9'|'f10'|'f11'|'f12'|
    '1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'0'|'minus'|'equals'|'backspace'|
    'q'|'w'|'e'|'r'|'t'|'y'|'u'|'i'|'o'|'p'|'backslash'|
    'a'|'s'|'d'|'f'|'g'|'h'|'j'|'k'|'l'|'enter'|
    'z'|'x'|'c'|'v'|'b'|'n'|'n'|'m'|'comma'|'period'|'slash'|
    'lshift'|'lctrl'|'lalt'|'space'|'ralt'|'left'|'up'|'down'|'right'
export type MouseButtonType = "left" | "right"| "middle"
declare class Matrix3
    function Inverse(self): Matrix3
    function Transpose(self): Matrix3
    __mul: ((Matrix3, Matrix3)->Matrix3)
        & ((Matrix3, Vector3)->Vector3)
        & ((Matrix3, Vector)->Vector)
    function __add(self, other: Matrix3): Matrix3
    function __sub(self, other: Matrix3): Matrix3
end
declare class Vector3
    X: number
    Y: number
    Z: number
    function DotProduct(self, a: Vector3): number
    function ToUnitVector(self): Vector3
    function Length(self): number
    function Abs(self): Vector3
    function __add(self, a: Vector3): Vector3
    function __mul(self, scalar: number): Vector3
    function __div(self, scalar: number): Vector3
    function __sub(self, a: Vector3): Vector3
    function __unm(self): Vector3
end
declare class Vector
    function DotProduct(self, a: Vector): number
    function ToUnitVector(self): Vector
    function Length(self): number
    function Abs(self): Vector
    function At(self, index: number): number
    function Set(self, index: number, value: number)
    function Reset(self)
    function Extend(self, size: number, preserve: boolean?)
    function Size(self): number
    function Capacity(self): number
    function Resize(self, size: number, preserve: boolean?)
    function Reserve(self, amount: number)
    function __add(self, a: Vector): Vector
    function __mul(self, scalar: number): Vector
    function __div(self, scalar: number): Vector
    function __sub(self, a: Vector): Vector
    function __unm(self): Vector
end
declare class Font
    FilePath: FilePath
    PointSize: number
end
export type FilesModule = {
    CreateDirectory: (where: FilePath | string) -> boolean,
    Exists: (path: FilePath | string) -> boolean,
    IsCharacterFile: (path: FilePath | string) -> boolean,
    CopyFile: (from: FilePath | string, to: FilePath | string, opt: CopyOptions?) -> boolean,
    Rename: (from: FilePath | string, to: FilePath | string) -> (),
    Remove: (path: FilePath | string) -> boolean,
    RemoveAll: (path: FilePath | string) -> number,
    Copy: (from: FilePath | string, to: FilePath | string, opt: CopyOptions?) -> (),
    Absolute: (path: FilePath | string) -> FilePath,
    GetChildrenOf: (path: FilePath | string) -> {DirectoryEntry},
    GetDescendantsOf: (path: FilePath | string) -> {DirectoryEntry},
    ExecutablePath: ()->FilePath,
    CurrentPath: ()->FilePath,
    Canonical: (path: FilePath)->FilePath,
    Proximate: (path: FilePath, base: FilePath?)->FilePath,
    CreateSymlink: (to: FilePath, new_symlink: FilePath)->(),
    Relative: (path: FilePath, base: FilePath?)->FilePath,
}
export type WindowModule = {
    Size: Vector2,
    Position: Vector2,
    Title: string,
    Opacity: number,
    Borderless: boolean,
    AlwaysOnTop: boolean,
    Maximize: ()->(),
    Minimize: ()->(),
    SetIcon: (file: string)->(),
    Focus: ()->(),
}
export type BlendMode= 'none'|'mul'|'add'|'mod'|'blend'|'invalid'
export type GraphicsModule = {
    DrawColor: Color,
    BlendMode: BlendMode,
    DrawRectangle: (rect: Rectangle)->(),
    DrawRectangles: (...Rectangle)->(),
    DrawPixel: (point: Vector2)->(),
    DrawPixels: (...Vector2)->(),
    FillRectangle: (rect: Rectangle)->(),
    FillRectangles: (...Rectangle)->(),
    DrawLine: (t0: Vector2, t1: Vector2)->(),
    DrawLines: ((...Vector2)->()), 
    FillCircle: ((center: Vector2, radius: number)->())
        & ((transform: Matrix3, radius: number)->()),
    FillEllipse: (center: Vector2, radius: Vector2)->(),
    DrawString: ((ft: Font, str: string, transform: Matrix3?)->())
        & ((text: string, transform: Matrix3?)->()),
    ClearCanvas: (bg: Color?)->(),
    DrawTexture: ((texture: Texture, pos: Vector2?)->())
        & ((texture: Texture, dest: Rectangle)->())
        & ((texture: Texture, trs: Matrix3)->()),
}
declare class EventConnectionId
    function __tostring(self): string
end
declare class Texture
    Width: number
    Height: number
    BlendMode: BlendMode
    Color: Color
end
export type Event<Params...> = {
    Connect: (self: Event<Params...>, (Params...)->())->EventConnectionId,
    Disconnect: (self: Event<Params...>, id: EventConnectionId)->(),
    Fire: (self: Event<Params...>, Params...)->(),
}
export type Luwaw = {
    GetModule: (('Graphics')->GraphicsModule)
        & (('Files')->FilesModule)
        & (('Window')->WindowModule),
    OnUpdate: Event<number>,
    OnRender: Event<>,
    OnKeyDown: Event<KeyCode>,
    OnKeyUp: Event<KeyCode>,
    OnMouseButtonDown: Event<MouseButtonType, Vector2>,
    OnMouseButtonUp: Event<MouseButtonType, Vector2>,
    OnShutdown: Event<>,
}
declare game: Luwaw
declare Rectangle: (x: number?, y: number?, w: number?, h: number?)->Rectangle
declare Vector2: (x: number?, y: number?)->Vector2
declare Vector3: (x: number?, y: number?, z: number?)->Vector3
declare Vector: (...number)->Vector
type Matrix3NamedConstructors = typeof(setmetatable({
    FromScale = function(scale_factor: Vector2 | number): Matrix3 return {} :: any end,
    FromRotation = function(radians: number): Matrix3 return {} :: any end,
    FromTranslation = function(translation: Vector2): Matrix3 return {} :: any end,
}, {__call = function(...): Matrix3 return {} :: any end}))
declare Matrix3: 
        & ((row1: {number}, row2: {number}, row3: {number})->Matrix3)
        & ((e11: number?, e12: number?, e13: number?, e21: number?, e22: number?, e23: number?, e31: number?, e32: number?, e33: number?)->Matrix3)
        & Matrix3NamedConstructors
declare Color: ((r: number?, g: number?, b: number?, a: number?)->Color)
        & ((r: number, g: number, b: number, alpha: number)->Color)
declare FilePath: (path: string)->FilePath
declare Event: <Params...>()->Event<Params...>
declare Font: (file: FilePath, pt_size: number)->Font

type TextureNamedConstructors = typeof(setmetatable({
    FromString = function(text: string, font: Font?, color: Color?): Texture return {} :: any end,
}, {__call = function(...): Texture return {} :: any end}))
declare Texture: ((file: FilePath | string)->Texture) & TextureNamedConstructors

