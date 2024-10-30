--!strict
local state = require '@Legion/state'
local SDL = require '@Legion/SDL'
local resourcePath = Path("resources");
local mainFontPath = resourcePath / "main_font.ttf"
local imagePath = resourcePath / "jw.png"
local mainFont = assert(SDL.openFont(tostring(mainFontPath), 40))
type TextureEntry = {texture: SDL_Texture, srcRect: Recti}
local textureEntries: {TextureEntry} = table.create(0) 
local function insertEntry(txt: SDL_Texture, w: number, h: number)
    table.insert(textureEntries, {texture = txt, srcRect = Recti(0, 0, w, h)})
end

local texture: SDL_Texture?, w: number, h: number
texture, w, h = SDL.loadImage(tostring(imagePath))
if texture then insertEntry(texture, w, h) end
texture, w, h = SDL.loadText(mainFont, "Hello World", Coloru32(0xff, 0xff, 0xff, 0xff))
if texture then insertEntry(texture, w, h) end

state.rendering:connect(function()
    local render = SDL.render.renderCopy
    for i, entry in textureEntries do
        local realRect = entry.srcRect
        realRect.x = 100
        realRect.y = 100
        
        render(entry.texture, realRect)
    end
end)
