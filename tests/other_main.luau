--!strict
local resourcePath = Path("resources");
local mainFontPath = resourcePath / "main_font.ttf"
local imagePath = resourcePath / "jw.png"
local mainFont = assert(SDL.openFont(mainFontPath, 80))
local white = Color(0xff, 0xff, 0xff, 0xff)
local Render = SDL.Render
type TextureEntry = {texture: SDL_Texture, srcRect: Recti}
local textureEntries: {TextureEntry} = table.create(0) 
local function insertEntry(txt: SDL_Texture, w: number, h: number)
    table.insert(textureEntries, {texture = txt, srcRect = Recti(0, 0, w, h)})
end

local texture: SDL_Texture?, w: number, h: number
texture, w, h = SDL.loadImage(imagePath)
if texture then
    insertEntry(texture, w, h)
end
texture, w, h = SDL.loadText(mainFont, "Hello World!", white)
if texture then
    insertEntry(texture, w, h)
end

local spinnin_text_font = assert(SDL.openFont(mainFontPath, 25))
local spinning_text, st_w, st_h = assert(SDL.loadText(spinnin_text_font, "spinning text", white))
local spinning_center = V2i(st_w / 2, st_h / 2)
local spinning_angle = 0

local spinning: boolean = true 
InputModule.onMousePressed:connect(function(type: MouseButtonType, mousePosition: V2i)
    spinning = not spinning
end)

RunModule.onUpdate:connect(function(deltaSeconds: number)
    if spinning then
        local spin_speed = 100
        spinning_angle += spin_speed * deltaSeconds
    end
end)

RunModule.onRender:connect(function()
    local dummy = Recti(100, 100)
    for i, entry in textureEntries do
        dummy.w = entry.srcRect.w * .5
        dummy.h = entry.srcRect.h * .5
        Render.renderCopy(entry.texture, dummy)
    end
    local last = textureEntries[#textureEntries]
    Render.setDrawColor(Color(0xff, 0xff, 0x00, 0xff))
    Render.drawRects(Recti(300, 300, 100, 100), Recti(500, 500, 100, 100))
    Render.setDrawColor(Color(0, 0, 0xff))
    Render.drawRects(Recti(300, 500, 100, 100), Recti(500, 300, 100, 100))
    Render.renderCopy(last.texture, last.srcRect);
    Render.setDrawColor(Color(0xff, 0x00, 0x00, 0xff))
    Render.drawLines(V2i(0, 0), V2i(100, 100), V2i(100, 300), V2i(0, 0))
    local offset = V2(70, 400)
    Render.renderGeometry(
        SDL_Vertex(Color(0, 0, 255), offset + V2(100, -50)),
        SDL_Vertex(Color(255, 0, 0), offset + V2(0, 100)),
        SDL_Vertex(Color(0, 255, 0), offset + V2(200, 100))
    )
    local src = Recti(0, 0, st_w, st_h)
    local dst = Recti(550, 150, st_w, st_h)
    Render.renderCopy(spinning_text, dst, src, spinning_angle, spinning_center)
end)
