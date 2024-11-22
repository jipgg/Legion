--!strict
local graphics = core.get_module('graphics')
local gdraw = graphics.draw
local gfill = graphics.fill
local window = core.get_module('window')
local fpsCounter = require('@example/fpsCounter')
local dummyImage = require('@example/dummyImage')
local files = core.get_module("files")
local Path = files.Path
type Path = files_Path
local RESOURCE_DIR: Path = Path("resources")
local UserInput = core.get_module('userinput')
print(`is clip enabled {graphics.is_clip_enabled()}`)
print(`is clip enabled {graphics.is_clip_enabled()}`)
graphics.blend_mode = 'Blend'

window.set_icon(tostring(RESOURCE_DIR / "coin.png"))
window.title = "Yoooooo"
window.position = Vec2(100, 100)
window.maximize()
print(window.size)
--Window.Borderless = true

local myEvent = Event()


myEvent:connect(function(num: number)
    window.title = `Yo check this number out {num}`
    window.opacity = 0.5
    window.borderless = false
end)
local currentAngle = 0
local myFont = Font(RESOURCE_DIR / "main_font.ttf", 100)
local helloText = {'h','e','l','l','o', ' ', 'w', 'o', 'r', 'l','d'}
local textChangeAccumulated = 0
core.during_update:connect(function(dt: number)
    textChangeAccumulated += dt
    if textChangeAccumulated < .02 then
        return
    end
    local c1 = math.random(1, #helloText)
    local c2 = math.random(1, #helloText)
    local temp = helloText[c1]
    helloText[c1] = helloText[c2]
    helloText[c2] = temp
    textChangeAccumulated = 0
end)

-- core.during_render:connect(function()
--     local color
--     graphics.clear(Color(0, 0, 0, 255))
--     color = Color(
--         math.abs(math.cos(currentAngle)) * 255,
--         math.abs(math.sin(currentAngle - 45)) * 255,
--         math.abs(math.sin(currentAngle)) * 255
--     )
--     --Graphics.ClipRect = Rectangle(0, 0, Window.Size.X / 2, Window.Size.Y / 2)
--     local SIZE = window.size
--     --graphics.viewport = Rect(0, 0, SIZE.x / 2, SIZE.y)
--     graphics.color = color:alpha_blend(color)
--     gfill.circle(Vec2(100, 100), 40)
--     graphics.color = color:multiplied(color:modulated(color))
--     gfill.rect(Rect(200-40, 100-40, 80, 80))
--     graphics.color = color:multiplied(color:inverted())
--
--     gfill.rect(Rect(200-40, 200-40, 80, 80))
--     graphics.color = color:modulated(color)
--     gfill.circle(Vec2(100, 200), 40)
--     graphics.color = Color(color.red, color.green, color.blue, 100):inverted()
--     graphics.blend_mode = 'Blend'
--     gfill.circle(Vec2(500, 400), 450)
--     graphics.color = Color(color.red, color.green, color.blue, 50)
--     gfill.ellipse(Vec2(400, 480), Vec2(400, 100));
--     graphics.color = Color(color.red, color.green, color.blue, 50):inverted():modulated(color)
--     gfill.ellipse(Vec2(400, 300), Vec2(200, 400));
--     graphics.color = Color(color.red, color.green, color.blue, 50):inverted():multiplied(color)
--     gfill.ellipse(Vec2(400, 480), Vec2(400, 300));
--     graphics.color = Color(100, 100, 100):additive_blend(color)
--     local transformation = Mat3.from_translation(Vec2(50, 400))
--     transformation *= Mat3.from_scale(Vec2(1, math.cos(currentAngle * 4) * .1 + 1))
--
--     local S = Rect(SIZE.x / 2, 0, SIZE.x / 2, SIZE.y / 2)
--     --graphics.viewport = S
--     graphics.blend_mode = "None"
--     graphics.color = Color(255, 255, 255)
--     gfill.rect(Rect(0, 0, SIZE.x / 2, SIZE.y / 2))
--     graphics.blend_mode = "Blend"
--     graphics.color = Color(100, 100, 100):additive_blend(color)
--     gdraw.string(myFont, table.concat(helloText), transformation)
--     --graphics.viewport = Rect(0, 0, SIZE.x, SIZE.y)
-- end)
core.during_update:connect(function(dt: number)
    currentAngle += math.rad(30) * dt
end)
fpsCounter.init()
dummyImage.init()
UserInput.mouse_button_pressed:connect(function(type: "Left" | "Middle" | "Right", mouse_pos: Vec2)

end)
UserInput.key_pressed:connect(function(key)
    if key == 'W' then
        print(`Mouse position: {UserInput.mouse_position}`)
    elseif key == 'A' then
        graphics.vsync_enabled = not graphics.vsync_enabled
    elseif key == 'D' then
        graphics.clip_rect = nil
        print(`is clip enabled {graphics.is_clip_enabled()}`)
    end
end)
