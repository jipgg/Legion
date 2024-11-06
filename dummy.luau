--!strict
local legion = require '@legion'
local sdl = legion.sdl
local trs = legion.trs
local Image = legion.Image
local resourcePath = Path("resources");
local mainFontPath = resourcePath / "main_font.ttf"
local imagePath = resourcePath / "jw.png"
local mainFont = assert(sdl.openFont(mainFontPath, 80))
local white = Color(0xff, 0xff, 0xff, 0xff)
type TextureEntry = {texture: SDL_Texture, srcRect: Recti}
local textureEntries: {TextureEntry} = table.create(0) 
local function insertEntry(txt: SDL_Texture, w: number, h: number)
    table.insert(textureEntries, {texture = txt, srcRect = Recti(0, 0, w, h)})
end
type sliceV2 = ( (Vec) -> Vec2 )
    & ( (Vec3)-> Vec2 )
    & ( ({get: (unknown, number) -> number}) -> Vec2 )
local sliceV2: sliceV2 = function(v: any)
    return Vec2(
        v:get(0),
        v:get(1)
    )
end

local dyn = Vec(1, 2, 3, 4, 100, 200, -100)
print(`dyn + dyn = {dyn + dyn}`)

print(`dyn size {dyn:size()}`)
dyn:extend(1)
print(`dyn size {dyn:size()}`)
print(`dyn is {dyn}, and dyn[1] {dyn(2)}`)

local v3 = Vec3(100, 100, 100)
print(`magnitude = {v3:magnitude()}, dot = {v3:unit()}`)

local texture: SDL_Texture?, w: number, h: number
texture, w, h = sdl.loadImage(imagePath)
if texture then
    insertEntry(texture, w, h)
end
texture, w, h = sdl.loadText(mainFont, "text", white)
if texture then
    insertEntry(texture, w, h)
end

local image_pack = table.pack(assert(sdl.loadImage(imagePath)))

local spinnin_text_font = assert(sdl.openFont(mainFontPath, 25))
local spinning_text, st_w, st_h = assert(sdl.loadText(spinnin_text_font, "spinning text", white))
local spinning_center = Vec2i(st_w / 2, st_h / 2)
local spinning_angle = 0

local spinning: boolean = true 
legion.input.onMousePressed:connect(function(type: MouseButtonType, mousePosition: Vec2)
    spinning = not spinning
end)

legion.run.onUpdate:connect(function(deltaSeconds: number)
    if spinning then
        local spin_speed = 100
        spinning_angle += spin_speed * deltaSeconds
    end
end)
local image = Image{texturePath = imagePath, anchorPoint = Vec2(.5, .5)}
legion.run.onRender:connect(function()
    image:render(trs.T(300, 300) * trs.R(spinning_angle / 100) * trs.S(.5))
end)

--[=[
local textu = image_pack[1]::SDL_Texture
local width = image_pack[2]::number
local height = image_pack[3]::number
local buf = std.Buffer(90)
--vert
local xy_offset = buf.size
buf:push('f32',
    0, 0,
    width, 0,
    width, height,
    0, height
)
--color
local color_offset = buf.size
buf:push('u8',
    0x00, 0xff, 0xff, 0xff,
    0xff, 0x00, 0xff, 0xff,
    0xff, 0xff, 0x00, 0xff,
    0x00, 0x00, 0xff, 0xff
)
--uv
local uv_offset = buf.size 
buf:push('f32',
    0, 0,
    1, 0,
    1, 1,
    0, 1
)
--indices
local indices_offset = buf.size
buf:push('i8',
    0, 1, 2,
    2, 3, 0
)
std.run.onRender:connect(function()
    sdl.render.renderGeometryRaw(textu, buf.data,
        xy_offset, 8,
        color_offset, 4,
        uv_offset, 8,
        4, indices_offset, 6, 1)
end)

--[=[
RunModule.onRender:connect(function()
    -- local dummy = Recti(100, 100)
    -- for i, entry in textureEntries do
    --     dummy.w = entry.srcRect.w * .5
    --     dummy.h = entry.srcRect.h * .5
    --     Render.renderCopy(entry.texture, dummy)
    -- end
    local scale = .5
    local width = image_pack[2]::number * scale
    local height = image_pack[3]::number * scale
    local blend = Color(0xff, 0xff, 0xff)
    local transform = TRS.translation(250, 200)
        * TRS.scale(math.sin(math.rad(spinning_angle % 360) / 2), .7)
        * TRS.translation(-width / 2, - height / 2)
    SDLRender.renderQuad(image_pack[1]::SDL_Texture,
        SDL_Vertex(Color(), sliceV2(transform * Vec3(60, 0, 1)), Vec2(0, 0)),
        SDL_Vertex(Color(), sliceV2(transform * Vec3(width - 60, 0, 1)), Vec2(1.0, 0)),
        SDL_Vertex(blend, sliceV2(transform * Vec3(width, height, 1)), Vec2(1.0, 1.0)),
        SDL_Vertex(blend, sliceV2(transform * Vec3(0, height, 1)), Vec2(0, 1.0))
    )
    local last = textureEntries[#textureEntries]
    SDLRender.setDrawColor(Color(0xff, 0xff, 0x00, 0xff))
    SDLRender.drawRects(Recti(300, 300, 100, 100), Recti(500, 500, 100, 100))
    SDLRender.setDrawColor(Color(0, 0, 0xff))
    SDLRender.drawRects(Recti(300, 500, 100, 100), Recti(500, 300, 100, 100))
    SDLRender.renderCopy(last.texture, last.srcRect);
    SDLRender.setDrawColor(Color(0xff, 0x00, 0x00, 0xff))
    --Render.drawLines(V2i(0, 0), V2i(100, 100), V2i(100, 300), V2i(0, 0))
    local trs = TRS.translation(150, 450)
    * TRS.rotation(math.rad(spinning_angle) / 4)
    * TRS.scale(math.sin(math.rad(spinning_angle)), math.sin(math.rad(spinning_angle) / 4) * 1.2)
    * TRS.translation(-100, -50);
    SDLRender.renderGeometry(
        SDL_Vertex(Color(0, 0, 255), sliceV2(trs * Vec3(100, -50, 1))),
        SDL_Vertex(Color(255, 0, 0), sliceV2(trs * Vec3(0, 100, 1))),
        SDL_Vertex(Color(0, 255, 0), sliceV2(trs * Vec3(200, 100, 1)))
    )
    local src = Recti(0, 0, st_w, st_h)
    local dst = Recti(550, 150, st_w, st_h)
    SDLRender.renderCopy(spinning_text, dst, src, spinning_angle, spinning_center)
end)
]=]
