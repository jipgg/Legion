--!strict
local resourcePath = Path("resources");
local mainFontPath = resourcePath / "main_font.ttf"
local imagePath = resourcePath / "jw.png"
local mainFont = assert(SDL.openFont(tostring(mainFontPath), 80))
local white = Coloru32(0xff, 0xff, 0xff, 0xff)
type TextureEntry = {texture: SDL_Texture, srcRect: Recti}
local textureEntries: {TextureEntry} = table.create(0) 
local function insertEntry(txt: SDL_Texture, w: number, h: number)
    table.insert(textureEntries, {texture = txt, srcRect = Recti(0, 0, w, h)})
end

local texture: SDL_Texture?, w: number, h: number
texture, w, h = SDL.loadImage(tostring(imagePath))
if texture then
    insertEntry(texture, w, h)
end
texture, w, h = SDL.loadText(mainFont, "Hello World!", white)
if texture then
    insertEntry(texture, w, h)
end

InputModule.onMousePressed:connect(function(type: MouseButtonType, mousePosition: V2i)
    print(type, mousePosition)
end)

RunModule.onRender:connect(function()
    local render = SDL.render.renderCopy
    local dummy = Recti(100, 100)
    for i, entry in textureEntries do
        dummy.w = entry.srcRect.w * 1.5
        dummy.h = entry.srcRect.h * 1.5
        render(entry.texture, dummy)
    end
    local last = textureEntries[#textureEntries]
    render(last.texture, last.srcRect)
end)
