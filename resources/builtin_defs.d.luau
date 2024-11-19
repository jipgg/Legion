declare function collectgarbage(opt: ("collect" | "count")?)
export type CopyOptions = "Recursive"|"Copy Symlinks"|
    "Skip Symlinks"|"Skip Existing"|"Update Existing"|
    "Create Symlinks"|"Directories Only"|"Create Hard Links"|
    "Overwrite Existing"
export type FileType = 'Fifo'|'Socket'|'block'|'Regular'|
    'Symlink'|'Unknown'|'Junction'|'Character'|'Directory'|
    'Not Found'|'None'
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
    function FilePath(self): FilePath
    function IsSocket(self): boolean
    function IsOther(self): boolean
    function IsSymlink(self): boolean
    function IsBlockFile(self): boolean
    function IsRegularFile(self): boolean
    function IsCharacterFile(self): boolean
end
declare class Vec2
    X: number
    Y: number
    function DotProduct(self, a: Vec2): number
    function ToUnitVector(self): Vec2
function Length(self): number
    function Abs(self): Vec2
    function __add(self, a: Vec2): Vec2
    function __mul(self, scalar: number): Vec2
    function __div(self, scalar: number): Vec2
    function __sub(self, a: Vec2): Vec2
    function __unm(self): Vec2
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
declare class Rect
    X: number
    Y: number
    Width: number
    Height: number
end
declare class TexturePtr end
export type RenderFlip = 'None'|'Vertical'|'Horizontal'
export type KeyCode = 'Escape'|'F1'|'F2'|'F3'|'F4'|'F5'|'F6'|'F7'|'F8'|'F9'|'F10'|'F11'|'F12'|
    '1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'0'|'Minus'|'Equals'|'Backspace'|
    'Q'|'W'|'E'|'R'|'T'|'Y'|'U'|'I'|'O'|'P'|'Backslash'|
    'A'|'S'|'D'|'F'|'G'|'H'|'J'|'K'|'L'|'Enter'|
    'Z'|'X'|'C'|'V'|'B'|'N'|'M'|'Comma'|'Period'|'slash'|
    'Left Shift'|'Left Control'|'Left Alt'|'Space'|'Right Alt'|'Arrow Left'|'Arrow Up'|'Arrow Down'|'Arrow Right'
export type MouseButtonType = "Left" | "Right"| "Middle"
declare class Mat3
    function Inverse(self): Mat3
    function Transpose(self): Mat3
    __mul: ((Mat3, Mat3)->Mat3)
        & ((Mat3, Vec3)->Vec3)
        & ((Mat3, Vec)->Vec)
    function __add(self, other: Mat3): Mat3
    function __sub(self, other: Mat3): Mat3
end
declare class Vec3
    X: number
    Y: number
    Z: number
    function DotProduct(self, a: Vec3): number
    function ToUnitVector(self): Vec3
    function Length(self): number
    function Abs(self): Vec3
    function __add(self, a: Vec3): Vec3
    function __mul(self, scalar: number): Vec3
    function __div(self, scalar: number): Vec3
    function __sub(self, a: Vec3): Vec3
    function __unm(self): Vec3
end
declare class Vec
    function DotProduct(self, a: Vec): number
    function ToUnitVector(self): Vec
    function Length(self): number
    function Abs(self): Vec
    function At(self, index: number): number
    function Set(self, index: number, value: number)
    function Reset(self)
    function Extend(self, size: number, preserve: boolean?)
    function Size(self): number
    function Capacity(self): number
    function Resize(self, size: number, preserve: boolean?)
    function Reserve(self, amount: number)
    function __add(self, a: Vec): Vec
    function __mul(self, scalar: number): Vec
    function __div(self, scalar: number): Vec
    function __sub(self, a: Vec): Vec
    function __unm(self): Vec
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
    ReadFile: (path: FilePath | string)->string?,
    WriteFile: (path: FilePath | string, data: string)->boolean,
    FilePath: (string)->FilePath,
}
export type WindowModule = {
    Size: Vec2,
    Position: Vec2,
    Title: string,
    Opacity: number,
    Borderless: boolean,
    AlwaysOnTop: boolean,
    Maximize: ()->(),
    Minimize: ()->(),
    SetIcon: (file: string)->(),
    Focus: ()->(),
    FocusGained: Event<>,
    FocusLost: Event<>,
    Shown: Event<>,
    Hidden: Event<>,
    Resized: Event<Vec2>,
    MouseEnter: Event<>,
    MouseLeave: Event<>,
    PositionChanged: Event<Vec2>,
    SizeChanged: Event<Vec2>,
    Restored: Event<>,
    Closing: Event<>,
    Exposed: Event<>,
    Minimized: Event<>,
    Maximized: Event<>,
}
export type BlendMode= 'None'|'Multiply'|'Add'|'Modulate'|'Blend'|'Invalid'
type GraphicsModule_DrawFunctions = {
        Rectangle: (rect: Rect)->(),
        Rectangles: (...Rect)->(),
        Pixel: (point: Vec2)->(),
        Pixels: (...Vec2)->(),
        Line: (t0: Vec2, t1: Vec2)->(),
        Lines: ((...Vec2)->()), 
        String: ((ft: Font, str: string, transform: Mat3?)->())
}
type GraphicsModule_FillFunctions = {
    Rectangle: (rect: Rect)->(),
    Rectangles: (...Rect)->(),
    Circle: ((center: Vec2, radius: number)->())
        & ((transform: Mat3, radius: number)->()),
    Ellipse: (center: Vec2, radius: Vec2)->(),
}
type GraphicsModule_RenderFunctions = {
    Text: ((ft: Font, str: string, transform: Mat3?)->())
        & ((text: string, transform: Mat3?)->()),
    Texture: ((texture: Texture, pos: Vec2?)->())
        & ((texture: Texture, dest: Rect)->())
        & ((texture: Texture, trs: Mat3)->()),
}
export type GraphicsModule = {
    Color: Color,
    BlendMode: BlendMode,
    Viewport: Rect,
    Scale: Vec2,
    VSyncEnabled: boolean,
    ClipRect: Rect?,
    IsClipEnabled: ()->boolean,
    ClearCanvas: (bg: Color?)->(),
    Draw: GraphicsModule_DrawFunctions,
    Fill: GraphicsModule_FillFunctions,
    Render: GraphicsModule_RenderFunctions,
}
export type UserInputModule = {
    IsKeyDown: (key: KeyCode)->boolean,
    KeyPressed: Event<KeyCode>,
    KeyReleased: Event<KeyCode>,
    MouseButtonPressed: Event<MouseButtonType, Vec2>,
    MouseButtonReleased: Event<MouseButtonType, Vec2>,
    MouseMoved: Event<Vec2, Vec2>,
    MousePosition: Vec2,
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
        & (('Window')->WindowModule)
        & (('UserInput')->UserInputModule),
    DuringUpdate: Event<number>,
    DuringRender: Event<>,
    ShuttingDown: Event<>,
    BeforeRun: Event<>,
    AfterRun: Event<>,
}
declare core: Luwaw
declare Rect: (x: number?, y: number?, w: number?, h: number?)->Rect
declare Vec2: (x: number?, y: number?)->Vec2
declare Vec3: (x: number?, y: number?, z: number?)->Vec3
declare Vec: (...number)->Vec
type Mat3_named_constructors = typeof(setmetatable({
    FromScale = function(scale_factor: Vec2 | number): Mat3 return {} :: any end,
    FromRotation = function(radians: number): Mat3 return {} :: any end,
    FromTranslation = function(translation: Vec2): Mat3 return {} :: any end,
}, {__call = function(...): Mat3 return {} :: any end}))
declare Mat3: 
        & ((row1: {number}, row2: {number}, row3: {number})->Mat3)
        & ((e11: number?, e12: number?, e13: number?, e21: number?, e22: number?, e23: number?, e31: number?, e32: number?, e33: number?)->Mat3)
        & Mat3_named_constructors 
declare Color: ((r: number?, g: number?, b: number?, a: number?)->Color)
        & ((r: number, g: number, b: number, alpha: number)->Color)
declare FilePath: (path: string)->FilePath
declare Event: <Params...>()->Event<Params...>
declare Font: (file: FilePath, pt_size: number)->Font

type Texture_named_constructors = typeof(setmetatable({
    FromString = function(text: string, font: Font?, color: Color?): Texture return {} :: any end,
}, {__call = function(...): Texture return {} :: any end}))
declare Texture: ((file: FilePath | string)->Texture) & Texture_named_constructors

